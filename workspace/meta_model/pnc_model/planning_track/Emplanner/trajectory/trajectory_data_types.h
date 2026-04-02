#pragma once

#include <vector>

namespace localplanner {
namespace trajectory {

/**
 * @brief 时空轨迹点数据结构
 * @details 包含空间几何、Frenet投影、时间与运动学全维度的信息，是规划器输出给底层控制模块的最终数据载体
 */
struct TrajectoryPoint {
    // 1. 笛卡尔坐标系几何属性 (用于底盘 LQR/MPC 直接横纵向跟踪)
    double x = 0.0;          ///< 真实世界 X 坐标 (m)
    double y = 0.0;          ///< 真实世界 Y 坐标 (m)
    double theta = 0.0;      ///< 航向角 (rad)
    double kappa = 0.0;      ///< 曲率 (1/m)，用于控制前馈

    // 2. Frenet 坐标系投影属性 (主要用于算法内部回溯、安全校验与显示)
    double s = 0.0;          ///< 累计参考线弧长 (m)
    double l = 0.0;          ///< 横向偏移量 (m)

    // 3. 时间与纵向运动学属性 (时空同步的核心)
    double t = 0.0;          ///< 相对当前规划起始帧的时间戳 (s)
    double v = 0.0;          ///< 绝对线速度 (m/s)
    double a = 0.0;          ///< 纵向加速度 (m/s^2)
    double jerk = 0.0;       ///< 纵向加加速度 (m/s^3)，反映乘坐舒适度

    // 4. 虚拟边界 (透传供 UI 渲染或下游二次碰撞校验)
    double left_bound = 0.0; ///< 动态左边界相对于参考线的 L 值 (m)
    double right_bound = 0.0;///< 动态右边界相对于参考线的 L 值 (m)
};

} // namespace trajectory
} // namespace localplanner