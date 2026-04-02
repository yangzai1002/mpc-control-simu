#pragma once

#include <vector>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>

#include "planner_manager.h"
#include "reference_line/ref_data_types.h"
#include "path_planner/path_data_types.h"
#include "trajectory/trajectory_data_types.h"
#include "map_processor/obstacle_extractor.h"
#include "map_processor/map_data_types.h"

namespace localplanner {
namespace interface {

/**
 * @brief 实车传感器输入定位数据
 */
struct Location {
    double longitude;   ///< WGS84 经度 (deg)
    double latitude;    ///< WGS84 纬度 (deg)
    double height;      ///< 高程 (m)
    double acc;         ///< 纵向加速度 (m/s^2)
    double heading;     ///< 航向角 (deg，0~360，正北为0，顺时针)
    double veloity;     ///< 绝对线速度 (m/s) (注: 保持原有接口拼写兼容)
};

/**
 * @brief 全局路由路径点
 */
struct GlobalPathPoint {
    double longitude;   ///< WGS84 经度 (deg)
    double latitude;    ///< WGS84 纬度 (deg)
    double height;      ///< 高程 (m)
};

/**
 * @brief 输出至底盘控制的轨迹点
 */
struct PathPoint {
    double x;           ///< 期望 UTM X 或 局部 X 坐标 (m)
    double y;           ///< 期望 UTM Y 或 局部 Y 坐标 (m)
    double v;           ///< 期望速度 (m/s)
    double a;           ///< 期望加速度 (m/s^2)
    double theta;       ///< 期望航向角 (rad)
    double kappa;       ///< 期望曲率 (1/m)
    double l;           ///< 相对参考线的 Frenet 横向偏移 (m)
    double left_bound;  ///< 左侧安全走廊边界 (m)
    double right_bound; ///< 右侧安全走廊边界 (m)
};

/**
 * @brief 规划器对外交互接口适配层
 * @details 负责解析实车传感器数据、执行 WGS84 与 UTM 坐标转换、并调用核心规控大脑 (PlannerManager)
 */
class PlannerInterface {
public:
    PlannerInterface();
    ~PlannerInterface() = default;

    /**
     * @brief 核心规划主循环接口
     * @param[in] loc 当前高精定位数据
     * @param[in] global_path 全局路径点 (经纬度)
     * @param[in] occupancy_source 栅格地图一维数组 (以自车为原点的局部坐标系)
     * @param[in] vmax 当前最大允许速度 (m/s)
     * @param[out] out_trajectory 输出的底盘控制轨迹序列
     * @return 规划是否成功
     */
    bool RunPlanner(const Location& loc,
                    const std::vector<GlobalPathPoint>& global_path,
                    const std::vector<double>& occupancy_source,
                    double vmax,
                    std::vector<PathPoint>& out_trajectory);
    void LatLon2XY(double lat, double lon, double& x, double& y);
private:
    // --- 坐标系与投影核心数学方法 ---
    double Deg2Rad(double deg) const;
    double Rad2Deg(double rad) const;
    int CalcZone(double lon) const;
    
    void XY2LatLon(double x, double y, double& lat, double& lon);
    double ConvertHeadingToMathYaw(double vehicle_heading) const;

    // --- 内部数据处理流程 ---
    std::vector<path_planner::ObstacleSL> ProcessOccupancyMap(
        const std::vector<double>& occupancy_source,
        const std::vector<ref_line::ReferencePoint>& ref_line) const;

    // --- WGS84 椭球体常数配置 ---
    static constexpr double WGS84_A = 6378137.0;
    static constexpr double WGS84_F = 1.0 / 298.257223563;
    static constexpr double UTM_K0  = 0.9996;

    // 内部状态缓存
    int cached_zone_ = 0;
    bool is_northern_hemisphere_ = true;

    // 规控大脑实例
    std::unique_ptr<PlannerManager> planner_manager_;
};

} // namespace interface
} // namespace localplanner