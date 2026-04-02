#pragma once

#include <vector>
#include "trajectory_data_types.h"

namespace localplanner {
namespace trajectory {

/**
 * @brief 连续轨迹评估器
 * @details 将离散的轨迹点集封装为连续的时空函数，支持按给定时间戳 (t) 线性插值提取车辆期望状态
 */
class TrajectoryEvaluator {
public:
    /**
     * @brief 构造函数
     * @param[in] trajectory 规划器生成的离散最优时空轨迹点集
     */
    explicit TrajectoryEvaluator(const std::vector<TrajectoryPoint>& trajectory);

    /**
     * @brief 评估给定时间戳的轨迹状态
     * @details 若时间戳超出轨迹覆盖范围，将自动钳位(Clamp)到起点或终点状态
     * @param[in] t 目标相对时间戳 (s)
     * @return 经过线性/运动学插值计算后的连续轨迹点状态
     */
    TrajectoryPoint Evaluate(double t) const;

private:
    std::vector<TrajectoryPoint> trajectory_; ///< 内部缓存的离散轨迹数据
};

} // namespace trajectory
} // namespace localplanner