#pragma once

#include <vector>
#include "map_data_types.h"
#include "../frenet/frenet_data_types.h"
#include "../reference_line/ref_data_types.h"

namespace localplanner {
namespace map_processor {

/**
 * @brief 障碍物提取器
 * @details 将上游庞大的 1D 占据栅格数组，结合参考线，降维提取为 Frenet 坐标系下的有效危险障碍物点集
 */
class ObstacleExtractor {
public:
    /**
     * @brief 提取器过滤配置
     * @details 定义了车辆关注的“相对危险视野区域” (ROI - Region of Interest)
     */
    struct ExtractConfig {
        double s_lookahead = 150.0;  ///< 纵向关注距离：扫描车头前方多远的障碍物 (m)
        double l_half_width = 6.0;   ///< 横向关注距离：左右两侧的扫描半宽，通常为车道宽+路肩 (m)
    };

    /**
     * @brief 构造函数
     * @param[in] config 提取器的视野边界配置
     */
    explicit ObstacleExtractor(const ExtractConfig& config);

    /**
     * @brief 核心业务函数：执行栅格到 Frenet 散点的转换与过滤
     * @param[in] occupancy_map 一维化展开的占据栅格数据 (概率值或二值化)
     * @param[in] map_cfg 地图的几何与分辨率配置
     * @param[in] ref_line 当前使用的平滑参考线 (用于坐标系投影)
     * @return 经过视野过滤后的 Frenet 坐标系障碍物点集
     */
    std::vector<frenet::FrenetPoint> Extract(
        const std::vector<double>& occupancy_map,
        const GridMapConfig& map_cfg,
        const std::vector<ref_line::ReferencePoint>& ref_line) const;

private:
    ExtractConfig config_; ///< 提取器配置参数
};

} // namespace map_processor
} // namespace localplanner