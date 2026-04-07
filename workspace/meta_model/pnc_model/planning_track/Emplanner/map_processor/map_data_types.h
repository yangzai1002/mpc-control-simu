#pragma once

namespace localplanner {
namespace map_processor {

/**
 * @brief 栅格地图配置参数
 * @details 定义了感知模块或上游输入的占据栅格地图 (Occupancy Grid Map) 的物理属性
 */
struct GridMapConfig {
    double resolution = 0.2;     ///< 栅格分辨率 (m/grid)
    double x_size = 100.0;       ///< 地图物理总宽度 (m)
    double y_size = 100.0;       ///< 地图物理总高度 (m)
    double origin_x = -50.0;    ///< 地图原点 X 坐标 (m)，通常自车位于几何中心
    double origin_y = -50.0;    ///< 地图原点 Y 坐标 (m)
    int width = 500;            ///< 栅格像素总宽度 (x_size / resolution)
    int height = 500;           ///< 栅格像素总高度 (y_size / resolution)
};

} // namespace map_processor
} // namespace localplanner