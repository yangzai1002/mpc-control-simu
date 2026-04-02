#pragma once

#include <vector>
#include <string>
#include <cmath>

namespace localplanner {
namespace path_planner {

/**
 * @brief 车辆运动学与几何参数配置
 * @details 针对 40-80km/h 越野环境调优的阿克曼底盘参数及多圆盘碰撞模型
 */
struct VehicleConfig {
    double length = 1.2;          ///< 车身总长 (m)
    double width = 0.5;           ///< 车身总宽 (m)
    double wheel_base = 0.4;      ///< 轴距 (m)
    double front_hang = 0.2;      ///< 前悬长度 (m)
    double rear_hang = 0.2;       ///< 后悬长度 (m)

    double max_steer_angle = 0.6; ///< 最大前轮转角 (约34度，rad)
    double min_turn_radius = 0.0; ///< 最小转弯半径 (m)
    
    // 多圆盘模型参数：用3个圆覆盖车身以进行快速碰撞检测
    double disc_radius = 0.3;    ///< 碰撞圆盘半径：(Width/2) + 安全缓冲 (m)
    std::vector<double> disc_offsets = {0.0, 0.4, 0.8}; ///< 圆心相对后轴中心的纵向偏移 (m)

    VehicleConfig() {
        // 根据阿克曼转向几何 R_min = L / tan(max_steer) 初始化最小转弯半径
        min_turn_radius = wheel_base / std::tan(max_steer_angle); 
    }
};

/**
 * @brief SL 坐标系下的路径点
 * @details 记录路径的空间位置及其各阶导数，同时透传安全边界供后续模块使用
 */
struct PathPointSL {
    double s = 0.0;           ///< 纵向累计弧长 (m)
    double l = 0.0;           ///< 横向偏移量 (m)
    double dl = 0.0;          ///< 侧向变化率 (dl/ds)，反映瞬时航向差
    double ddl = 0.0;         ///< 二阶导数 (ddl/ds^2)，近似反映路径曲率

    double left_bound = 0.0;  ///< 动态计算的左侧虚拟安全边界 (m)
    double right_bound = 0.0; ///< 动态计算的右侧虚拟安全边界 (m)
};

/**
 * @brief 障碍物 SL 边界框
 * @details 由感知点云/栅格聚类降维而成的矩形包围盒
 */
struct ObstacleSL {
    double start_s = 0.0; ///< 障碍物后端 S 坐标 (m)
    double end_s = 0.0;   ///< 障碍物前端 S 坐标 (m)
    double left_l = 0.0;  ///< 障碍物左侧边界 L 坐标 (m)
    double right_l = 0.0; ///< 障碍物右侧边界 L 坐标 (m)
};

/**
 * @brief 动态规划 (DP) 采样节点状态
 */
struct SamplePoint {
    PathPointSL point;       ///< 节点的空间状态
    double min_cost = 1e20;  ///< 到达该节点的最小累计代价 (初始为极小概率/极大代价)
    int parent_row = -1;     ///< 前驱节点的行索引，用于最优路径回溯
};

} // namespace path_planner
} // namespace localplanner