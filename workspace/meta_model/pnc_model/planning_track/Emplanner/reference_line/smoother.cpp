#include "smoother.h"
#include "ref_math_utils.h"
#include <iostream>

namespace localplanner {
namespace ref_line {

ReferenceLineSmoother::ReferenceLineSmoother(const SmootherConfig& config) 
    : config_(config) {}

bool ReferenceLineSmoother::Execute(const std::vector<RawPoint>& input, 
                                   std::vector<ReferencePoint>& output) {
    if (input.size() < 3) {
        std::cerr << "[ReferenceLineSmoother] Input points too few, require at least 3 points." << std::endl;
        return false;
    }

    // 1. 数据致密化
    std::vector<RawPoint> dense_points;
    RefMath::Densify(input, dense_points, config_.sample_step);

    // 2. 求解二次规划
    std::vector<RawPoint> opt_points;
    if (!SolveQP(dense_points, opt_points)) {
        std::cerr << "[ReferenceLineSmoother] OSQP Solver failed to find a smoothed path." << std::endl;
        return false;
    }

    // 3. 将 (x, y) 转换为 ReferencePoint 格式并计算几何属性
    output.clear();
    output.reserve(opt_points.size());
    for (const auto& p : opt_points) {
        ReferencePoint pt;
        pt.x = p.x;
        pt.y = p.y;
        output.push_back(pt);
    }
    RefMath::ComputeAttributes(output);
    
    return true;
}

bool ReferenceLineSmoother::SolveQP(const std::vector<RawPoint>& dense, 
                                   std::vector<RawPoint>& opt) {
    const int n = static_cast<int>(dense.size());
    const int vars = 2 * n; // 状态变量是 [x0, y0, x1, y1, ...]

    // 1. 构造 Hessian 矩阵 P (稀疏矩阵)
    Eigen::SparseMatrix<double> P(vars, vars);
    std::vector<Eigen::Triplet<double>> p_triplets;

    // 权重预乘 2.0，因为标准型是 1/2 * x'Px
    const double w_ref = 2.0 * config_.weight_ref;
    const double w_len = 2.0 * config_.weight_len;
    const double w_cur = 2.0 * config_.weight_cur;

    for (int i = 0; i < n; ++i) {
        // [贴合度] 惩罚偏离原始点 (x_i - x_ref)^2
        p_triplets.emplace_back(2 * i, 2 * i, w_ref);
        p_triplets.emplace_back(2 * i + 1, 2 * i + 1, w_ref);

        // [紧凑度] 惩罚一阶导数 (x_i+1 - x_i)^2
        if (i < n - 1) {
            for (int j : {0, 1}) { // 遍历 x 和 y
                int idx = 2 * i + j;
                p_triplets.emplace_back(idx, idx, w_len);
                p_triplets.emplace_back(idx + 2, idx + 2, w_len);
                p_triplets.emplace_back(idx, idx + 2, -w_len);
                p_triplets.emplace_back(idx + 2, idx, -w_len);
            }
        }

        // [平滑度] 惩罚二阶导数 (x_i-1 - 2x_i + x_i+1)^2
        if (i > 0 && i < n - 1) {
            for (int j : {0, 1}) {
                int im = 2 * (i - 1) + j;
                int ic = 2 * i + j;
                int ip = 2 * (i + 1) + j;
                p_triplets.emplace_back(im, im, w_cur);
                p_triplets.emplace_back(ic, ic, 4.0 * w_cur);
                p_triplets.emplace_back(ip, ip, w_cur);
                p_triplets.emplace_back(im, ic, -2.0 * w_cur);
                p_triplets.emplace_back(ic, im, -2.0 * w_cur);
                p_triplets.emplace_back(ic, ip, -2.0 * w_cur);
                p_triplets.emplace_back(ip, ic, -2.0 * w_cur);
                p_triplets.emplace_back(im, ip, w_cur);
                p_triplets.emplace_back(ip, im, w_cur);
            }
        }
    }
    P.setFromTriplets(p_triplets.begin(), p_triplets.end());

    // 2. 构造梯度向量 q (目标函数的一次项系数)
    Eigen::VectorXd q = Eigen::VectorXd::Zero(vars);
    for (int i = 0; i < n; ++i) {
        q(2 * i) = -w_ref * dense[i].x;
        q(2 * i + 1) = -w_ref * dense[i].y;
    }

    // 3. 构造线性约束 A 矩阵与上下界 [l, u]
    Eigen::SparseMatrix<double> A(vars, vars);
    A.setIdentity();
    Eigen::VectorXd l(vars), u(vars);
    for (int i = 0; i < n; ++i) {
        l(2 * i) = dense[i].x - config_.boundary;
        u(2 * i) = dense[i].x + config_.boundary;
        l(2 * i + 1) = dense[i].y - config_.boundary;
        u(2 * i + 1) = dense[i].y + config_.boundary;
    }

    // 4. OSQP 求解器初始化与配置
    OsqpEigen::Solver solver;
    solver.settings()->setWarmStart(true); 
    solver.settings()->setVerbosity(false); 
    solver.data()->setNumberOfVariables(vars);
    solver.data()->setNumberOfConstraints(vars);
    
    if (!solver.data()->setHessianMatrix(P)) return false;
    if (!solver.data()->setGradient(q)) return false;
    if (!solver.data()->setLinearConstraintsMatrix(A)) return false;
    if (!solver.data()->setLowerBound(l)) return false;
    if (!solver.data()->setUpperBound(u)) return false;

    if (!solver.initSolver()) return false;
    //if (solver.solveProblem() != OsqpEigen::ErrorExitFlag::NoError) return false;
    if (!solver.solve()) {
    return false;
    }


    Eigen::VectorXd solution = solver.getSolution();
    opt.clear();
    opt.reserve(n);
    for (int i = 0; i < n; ++i) {
        opt.push_back({solution(2 * i), solution(2 * i + 1)});
    }
    return true;
}

} // namespace ref_line
} // namespace localplanner