#pragma once

#include <vector>
#include <array>
#include <utility>
#include <Eigen/Dense>
#include <OsqpEigen/OsqpEigen.h>

namespace localplanner {
namespace speed_planner {

/**
 * @brief 纵向分段 Jerk 二次规划求解器
 * @details 在给定的 S-T (空间-时间) 域上，求解满足车辆纵向动力学约束（速度、加速度、Jerk极限）的最优 S, V, A
 */
class SpeedPiecewiseJerkProblem {
public:
    /**
     * @brief 构造函数
     * @param[in] n 时间维度的离散采样点总数
     * @param[in] delta_t 离散时间间隔 (s)
     * @param[in] init_state 车辆初始纵向状态 [s0, v0, a0]
     */
    SpeedPiecewiseJerkProblem(size_t n, double delta_t, const std::array<double, 3>& init_state);

    // --- 权重设置接口 ---
    void set_weight_s(double w) { w_s_ = w; }
    void set_weight_v(double w) { w_v_ = w; }
    void set_weight_a(double w) { w_a_ = w; }
    void set_weight_jerk(double w) { w_jerk_ = w; }

    // --- 约束设置接口 ---
    void set_s_bounds(const std::vector<std::pair<double, double>>& s_bounds) { s_bounds_ = s_bounds; }
    void set_v_bounds(double v_max) { v_max_ = v_max; }
    void set_a_bounds(const std::vector<std::pair<double, double>>& a_bounds) { a_bounds_ = a_bounds; }

    /**
     * @brief 执行 OSQP 求解
     * @param[out] s 优化后的累计弧长集合
     * @param[out] v 优化后的速度集合
     * @param[out] a 优化后的加速度集合
     * @return 求解是否成功
     */
    bool Solve(std::vector<double>* s, std::vector<double>* v, std::vector<double>* a);

private:
    size_t n_ = 0;
    double dt_ = 0.0;
    std::array<double, 3> init_state_ = {0.0, 0.0, 0.0};

    // 目标函数权重
    double w_s_ = 0.0;
    double w_v_ = 1.0;
    double w_a_ = 10.0;
    double w_jerk_ = 100.0;

    // 状态边界
    std::vector<std::pair<double, double>> s_bounds_;
    double v_max_ = 25.0;
    std::vector<std::pair<double, double>> a_bounds_;
};

} // namespace speed_planner
} // namespace localplanner