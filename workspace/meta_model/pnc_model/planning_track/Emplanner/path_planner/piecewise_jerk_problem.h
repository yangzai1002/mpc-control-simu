#pragma once

#include <vector>
#include <array>
#include <utility>
#include <Eigen/Dense>
#include <OsqpEigen/OsqpEigen.h>

namespace localplanner {
namespace path_planner {

/**
 * @brief 分段 Jerk (加加速度) 二次规划求解器
 * @details 将路径平滑与避障问题建模为基于弧长 s 的二次规划 (QP) 问题，求解满足车辆运动学极限的最优 L, DL, DDL
 */
class PiecewiseJerkProblem {
public:
    /**
     * @brief 构造函数
     * @param[in] n 离散采样点总数
     * @param[in] delta_s 离散点间的弧长间隔 (m)
     * @param[in] init_state 车辆初始状态 [l0, dl0, ddl0]
     */
    PiecewiseJerkProblem(size_t n, double delta_s, const std::array<double, 3>& init_state);

    // --- 权重设置接口 ---
    void set_weight_l(double w) { w_l_ = w; }
    void set_weight_dl(double w) { w_dl_ = w; }
    void set_weight_ddl(double w) { w_ddl_ = w; }
    void set_weight_dddl(double w) { w_dddl_ = w; }

    // --- 物理约束设置接口 ---
    // 1. 设置横向位置（L）的绝对边界约束
    // 物理意义：限制车辆在道路上的左右活动范围，即“安全行驶走廊”。
    // l_bounds 是一个数组，包含了路径上每一个采样点处，车辆允许的最右侧和最左侧的 L 坐标。
    // 这个边界是提前扣除了障碍物占据空间、车辆自身半宽以及安全缓冲距离后计算出来的，保证轨迹绝对不会撞墙或撞车。
    void set_x_bounds(const std::vector<std::pair<double, double>>& bounds) { x_bounds_ = bounds; }

    // 2. 设置一阶导数（dL/dS）的运动学极限约束
    // 物理意义：限制车辆的“横向变化率”，它近似等价于车辆相对于参考线的“航向角差”（tan(Δθ)）。
    // max_dl_kinematic 限制了车辆不能像螃蟹一样瞬间横向平移，也不能以极其夸张的角度猛烈变道。
    // 它保证了规划出的路径在航向上是平缓可控的。
    void set_dx_bounds(double max_dx) { max_dx_ = max_dx; }

    // 3. 设置二阶导数（ddL/dS^2）的运动学极限约束
    // 物理意义：限制路径的“横向加速度”，它近似等价于路径的“曲率（Kappa）”。
    // max_ddl_kinematic 是根据阿克曼底盘的物理极限（最大前轮转角、最小转弯半径）计算出来的。
    // 这个约束极其重要，它能确保 QP 求解器算出来的平滑曲线，车辆在现实世界中真的能打得过方向盘转过去，防止规划出超出物理极限的“急弯”。
    void set_ddx_bounds(double max_ddx) { max_ddx_ = max_ddx; }
    void set_ref_kappa(const std::vector<double>& ref_kappa) { ref_kappa_ = ref_kappa; }

    /**
     * @brief 执行 OSQP 求解
     * @param[out] x 优化后的横向偏移量集合 (L)
     * @param[out] dx 优化后的横向变化率集合 (DL)
     * @param[out] ddx 优化后的横向加速度集合 (DDL)
     * @return 求解是否成功收敛
     */
    bool Solve(std::vector<double>* x, std::vector<double>* dx, std::vector<double>* ddx);

private:
    size_t n_ = 0;
    double delta_s_ = 0.0;
    std::array<double, 3> init_state_ = {0.0, 0.0, 0.0};

    // 目标函数权重
    double w_l_ = 1.0;
    double w_dl_ = 1.0;
    double w_ddl_ = 10.0;
    double w_dddl_ = 1000.0;

    // 运动学与空间走廊边界
    std::vector<std::pair<double, double>> x_bounds_;
    double max_dx_ = 0.5;
    double max_ddx_ = 0.2;
    std::vector<double> ref_kappa_;
};

} // namespace path_planner
} // namespace localplanner