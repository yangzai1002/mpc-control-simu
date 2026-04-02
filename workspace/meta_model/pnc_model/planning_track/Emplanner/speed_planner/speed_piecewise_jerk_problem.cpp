#include "speed_piecewise_jerk_problem.h"
#include <iostream>
#include <algorithm>

namespace localplanner {
namespace speed_planner {

SpeedPiecewiseJerkProblem::SpeedPiecewiseJerkProblem(size_t n, double delta_t, const std::array<double, 3>& init_state)
    : n_(n), dt_(delta_t), init_state_(init_state) {}

bool SpeedPiecewiseJerkProblem::Solve(std::vector<double>* s, std::vector<double>* v, std::vector<double>* a) {
    if (s == nullptr || v == nullptr || a == nullptr) return false;

    int num_vars = 3 * n_;
    // 约束维度：初始状态(2) + 连续性约束(2*(n-1)) + 边界约束(3*n)
    int num_cons = 2 + 2 * (n_ - 1) + 3 * n_;

    // 1. 构造 Hessian 矩阵 P (二次项) 和梯度向量 q (一次项)
    // 最小化目标: w_v * (v - v_ref)^2 => 0.5 * (2*w_v) * v^2 - (2*w_v*v_ref) * v
    Eigen::SparseMatrix<double> P(num_vars, num_vars);
    std::vector<Eigen::Triplet<double>> P_triplets;
    Eigen::VectorXd q = Eigen::VectorXd::Zero(num_vars);

    for (size_t i = 0; i < n_; ++i) {
        P_triplets.emplace_back(3 * i, 3 * i, 2.0 * w_s_);               // s^2
        P_triplets.emplace_back(3 * i + 1, 3 * i + 1, 2.0 * w_v_);       // v^2
        q(3 * i + 1) = -2.0 * w_v_ * v_max_;                             // 一次项: -2 * w_v * v_max
        P_triplets.emplace_back(3 * i + 2, 3 * i + 2, 2.0 * w_a_);       // a^2

        // Jerk 惩罚项: w_jerk * ((a_{i+1} - a_i) / dt)^2
        if (i < n_ - 1) {
            double w_jerk_eff = 2.0 * w_jerk_ / (dt_ * dt_);
            P_triplets.emplace_back(3 * i + 2, 3 * i + 2, w_jerk_eff);
            P_triplets.emplace_back(3 * (i + 1) + 2, 3 * (i + 1) + 2, w_jerk_eff);
            P_triplets.emplace_back(3 * i + 2, 3 * (i + 1) + 2, -w_jerk_eff);
            P_triplets.emplace_back(3 * (i + 1) + 2, 3 * i + 2, -w_jerk_eff);
        }
    }
    P.setFromTriplets(P_triplets.begin(), P_triplets.end());

    // 2. 构造线性约束矩阵 A 与上下界
    Eigen::SparseMatrix<double> A(num_cons, num_vars);
    std::vector<Eigen::Triplet<double>> A_triplets;
    Eigen::VectorXd lb = Eigen::VectorXd::Zero(num_cons);
    Eigen::VectorXd ub = Eigen::VectorXd::Zero(num_cons);

    int c_idx = 0;

    // --- 初始状态约束 ---
    A_triplets.emplace_back(c_idx, 0, 1.0); lb(c_idx) = ub(c_idx) = init_state_[0]; c_idx++;
    A_triplets.emplace_back(c_idx, 1, 1.0); lb(c_idx) = ub(c_idx) = init_state_[1]; c_idx++;

    // --- 运动学连续性约束 ---
    for (size_t i = 0; i < n_ - 1; ++i) {
        // s_{i+1} - s_i - v_i * dt - 0.5 * a_i * dt^2 = 0
        A_triplets.emplace_back(c_idx, 3 * (i + 1), 1.0);
        A_triplets.emplace_back(c_idx, 3 * i, -1.0);
        A_triplets.emplace_back(c_idx, 3 * i + 1, -dt_);
        A_triplets.emplace_back(c_idx, 3 * i + 2, -0.5 * dt_ * dt_);
        lb(c_idx) = ub(c_idx) = 0.0; 
        c_idx++;

        // v_{i+1} - v_i - a_i * dt = 0
        A_triplets.emplace_back(c_idx, 3 * (i + 1) + 1, 1.0);
        A_triplets.emplace_back(c_idx, 3 * i + 1, -1.0);
        A_triplets.emplace_back(c_idx, 3 * i + 2, -dt_);
        lb(c_idx) = ub(c_idx) = 0.0; 
        c_idx++;
    }

    // --- 状态边界约束 ---
    for (size_t i = 0; i < n_; ++i) {
        // S 必须单调递增
        A_triplets.emplace_back(c_idx, 3 * i, 1.0); 
        lb(c_idx) = 0.0; 
        ub(c_idx) = 1e6; 
        c_idx++;
        
        // V 限制 (软化上限，防止因初始超速导致无解)
        A_triplets.emplace_back(c_idx, 3 * i + 1, 1.0); 
        lb(c_idx) = 0.0; 
        ub(c_idx) = std::max(v_max_, init_state_[1] + 0.1); 
        c_idx++;

        // A 限制 (物理加减速度极限)
        A_triplets.emplace_back(c_idx, 3 * i + 2, 1.0); 
        lb(c_idx) = a_bounds_.empty() ? -3.0 : a_bounds_[i].first; 
        ub(c_idx) = a_bounds_.empty() ?  3.0 : a_bounds_[i].second; 
        c_idx++;
    }
    A.setFromTriplets(A_triplets.begin(), A_triplets.end());

    // 3. 配置并执行 OSQP 求解器
    OsqpEigen::Solver solver;
    solver.settings()->setWarmStart(true);
    solver.settings()->setVerbosity(false);
    solver.data()->setNumberOfVariables(num_vars);
    solver.data()->setNumberOfConstraints(num_cons);
    
    if(!solver.data()->setHessianMatrix(P)) return false;
    if(!solver.data()->setGradient(q)) return false;
    if(!solver.data()->setLinearConstraintsMatrix(A)) return false;
    if(!solver.data()->setLowerBound(lb)) return false;
    if(!solver.data()->setUpperBound(ub)) return false;

    if(!solver.initSolver()) return false;
    //if(solver.solveProblem() != OsqpEigen::ErrorExitFlag::NoError) return false;

    if (!solver.solve()) {
    return false;
    }


    Eigen::VectorXd sol = solver.getSolution();
    s->clear(); v->clear(); a->clear();
    for (size_t i = 0; i < n_; ++i) {
        s->push_back(sol(3 * i)); 
        v->push_back(sol(3 * i + 1)); 
        a->push_back(sol(3 * i + 2));
    }
    return true;
}

} // namespace speed_planner
} // namespace localplanner