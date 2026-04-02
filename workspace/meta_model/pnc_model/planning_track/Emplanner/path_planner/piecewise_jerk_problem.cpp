#include "piecewise_jerk_problem.h"
#include <iostream>
#include <algorithm>

namespace localplanner {
namespace path_planner {

PiecewiseJerkProblem::PiecewiseJerkProblem(size_t n, double delta_s, const std::array<double, 3>& init_state)
    : n_(n), delta_s_(delta_s), init_state_(init_state) {
    x_bounds_.resize(n_, {-1e10, 1e10}); // 默认无边界约束
}

bool PiecewiseJerkProblem::Solve(std::vector<double>* x, std::vector<double>* dx, std::vector<double>* ddx) {
    if (x == nullptr || dx == nullptr || ddx == nullptr) return false;

    // 1. 变量维度：每个节点包含 [l, dl, ddl] 三个状态变量
    int num_vars = 3 * n_;
    // 约束维度：初始约束(2) + 连续性约束(2*(n-1)) + 变量范围约束(3*n)
    int num_cons = 3 * n_ + 3 * (n_ - 1) + 2;

    // 2. 构造目标函数矩阵 P (二次项) 和梯度向量 q (一次项)
    Eigen::SparseMatrix<double> P(num_vars, num_vars);
    std::vector<Eigen::Triplet<double>> P_triplets;
    Eigen::VectorXd q = Eigen::VectorXd::Zero(num_vars);

    for (size_t i = 0; i < n_; ++i) {
        // OSQP 标准型为 0.5 * x^T * P * x，因此预乘 2.0 抵消系数
        P_triplets.emplace_back(3 * i, 3 * i, 2.0 * w_l_);
        P_triplets.emplace_back(3 * i + 1, 3 * i + 1, 2.0 * w_dl_);
        P_triplets.emplace_back(3 * i + 2, 3 * i + 2, 2.0 * w_ddl_);

        // 引入参考线曲率补偿，目标是最小化绝对曲率：0.5 * w_ddl * (ddl - kappa)^2
        if (!ref_kappa_.empty() && i < ref_kappa_.size()) {
            q(3 * i + 2) = -2.0 * w_ddl_ * ref_kappa_[i];
        }
        
        // Jerk (dddl) 惩罚项
        if (i < n_ - 1) {
            double w_jerk = 2.0 * w_dddl_ / (delta_s_ * delta_s_);
            P_triplets.emplace_back(3 * i + 2, 3 * i + 2, w_jerk);
            P_triplets.emplace_back(3 * (i + 1) + 2, 3 * (i + 1) + 2, w_jerk);
            P_triplets.emplace_back(3 * i + 2, 3 * (i + 1) + 2, -w_jerk);
            P_triplets.emplace_back(3 * (i + 1) + 2, 3 * i + 2, -w_jerk);
        }
    }
    P.setFromTriplets(P_triplets.begin(), P_triplets.end());

    // 3. 构造约束矩阵 A 及上下界
    Eigen::SparseMatrix<double> A(num_cons, num_vars);
    std::vector<Eigen::Triplet<double>> A_triplets;
    Eigen::VectorXd lower_bound = Eigen::VectorXd::Zero(num_cons);
    Eigen::VectorXd upper_bound = Eigen::VectorXd::Zero(num_cons);

    int con_idx = 0;

    // --- 初始状态等式约束 ---
    for (int j = 0; j < 2; ++j) {
        A_triplets.emplace_back(con_idx, j, 1.0);
        lower_bound(con_idx) = upper_bound(con_idx) = init_state_[j];
        con_idx++;
    }

    // --- 节点间运动学连续性约束 ---
    for (size_t i = 0; i < n_ - 1; ++i) {
        // l_{i+1} - l_i - dl_i * ds - 0.5 * ddl_i * ds^2 = 0
        A_triplets.emplace_back(con_idx, 3 * (i + 1), 1.0);
        A_triplets.emplace_back(con_idx, 3 * i, -1.0);
        A_triplets.emplace_back(con_idx, 3 * i + 1, -delta_s_);
        A_triplets.emplace_back(con_idx, 3 * i + 2, -0.5 * delta_s_ * delta_s_);
        lower_bound(con_idx) = upper_bound(con_idx) = 0.0;
        con_idx++;

        // dl_{i+1} - dl_i - ddl_i * ds = 0
        A_triplets.emplace_back(con_idx, 3 * (i + 1) + 1, 1.0);
        A_triplets.emplace_back(con_idx, 3 * i + 1, -1.0);
        A_triplets.emplace_back(con_idx, 3 * i + 2, -delta_s_);
        lower_bound(con_idx) = upper_bound(con_idx) = 0.0;
        con_idx++;
    }

    // --- 变量范围不等式约束 (包含避障空间走廊) ---
    for (size_t i = 0; i < n_; ++i) {
        // L 边界约束
        A_triplets.emplace_back(con_idx, 3 * i, 1.0); 
        lower_bound(con_idx) = x_bounds_[i].first;
        upper_bound(con_idx) = x_bounds_[i].second;
        con_idx++;
        
        // DL 边界约束 (放宽初始点的一阶导数约束，避免起步瞬间无解)
        A_triplets.emplace_back(con_idx, 3 * i + 1, 1.0); 
        lower_bound(con_idx) = (i == 0) ? std::min(-max_dx_, init_state_[1] - 0.1) : -max_dx_;
        upper_bound(con_idx) = (i == 0) ? std::max(max_dx_, init_state_[1] + 0.1) : max_dx_;
        con_idx++;

        // DDL 边界约束 (放宽初始点的二阶导数约束)
        A_triplets.emplace_back(con_idx, 3 * i + 2, 1.0); 
        lower_bound(con_idx) = (i == 0) ? std::min(-max_ddx_, init_state_[2] - 0.1) : -max_ddx_;
        upper_bound(con_idx) = (i == 0) ? std::max(max_ddx_, init_state_[2] + 0.1) : max_ddx_;
        con_idx++;
    }
    A.setFromTriplets(A_triplets.begin(), A_triplets.end());

    // 4. 调用 OSQP 求解器
    OsqpEigen::Solver solver;
    solver.settings()->setWarmStart(true);
    solver.settings()->setVerbosity(false);
    solver.data()->setNumberOfVariables(num_vars);
    solver.data()->setNumberOfConstraints(num_cons);
    
    if (!solver.data()->setHessianMatrix(P)) return false;
    if (!solver.data()->setGradient(q)) return false;
    if (!solver.data()->setLinearConstraintsMatrix(A)) return false;
    if (!solver.data()->setLowerBound(lower_bound)) return false;
    if (!solver.data()->setUpperBound(upper_bound)) return false;

    if (!solver.initSolver()) return false;
    //if (solver.solveProblem() != OsqpEigen::ErrorExitFlag::NoError) return false;
    // 新版接口：solve() 返回 bool，直接判断
    if (!solver.solve()) {
        return false;
    }

    // auto osqp_status = solver.getStatus(); 
    // int status_code = static_cast<int>(osqp_status);

    // // 1: OSQP_SOLVED (最优解), 2: OSQP_SOLVED_INACCURATE (近似解)
    // if (status_code != 1 && status_code != 2) {
    //     std::cerr << "[PiecewiseJerkProblem] Solver failed. Status Code: " << status_code << std::endl;
    //     return false;
    // }

    Eigen::VectorXd solution = solver.getSolution();
    x->clear(); dx->clear(); ddx->clear();
    for (size_t i = 0; i < n_; ++i) {
        x->push_back(solution(3 * i));
        dx->push_back(solution(3 * i + 1));
        ddx->push_back(solution(3 * i + 2));
    }
    return true;
}

} // namespace path_planner
} // namespace localplanner