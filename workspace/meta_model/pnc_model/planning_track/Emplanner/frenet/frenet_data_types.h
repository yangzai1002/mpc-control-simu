#pragma once

namespace localplanner {
namespace frenet {

/**
 * @brief Frenet 坐标系下的状态点数据结构
 * @details 用于描述车辆或障碍物相对于平滑参考线的位置状态
 */
struct FrenetPoint {
    double s = 0.0;    ///< 纵向位移：沿着参考线投影的累计弧长 (m)
    double l = 0.0;    ///< 横向偏移：偏离参考线中心的距离，左正右负 (m)
};

} // namespace frenet
} // namespace localplanner