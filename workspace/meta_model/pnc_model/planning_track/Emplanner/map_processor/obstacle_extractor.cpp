#include "obstacle_extractor.h"
#include "../frenet/cartesian_frenet_converter.h"
#include <iostream>
#include <cmath>

namespace localplanner {
namespace map_processor {

ObstacleExtractor::ObstacleExtractor(const ExtractConfig& config) 
    : config_(config) {}

std::vector<frenet::FrenetPoint> ObstacleExtractor::Extract(
    const std::vector<double>& occupancy_map,
    const GridMapConfig& map_cfg,
    const std::vector<ref_line::ReferencePoint>& ref_line) const {

    std::vector<frenet::FrenetPoint> dangerous_obs;

    if (ref_line.empty()) {
        std::cerr << "[ObstacleExtractor] Reference line is empty!" << std::endl;
        return dangerous_obs;
    }

    // 1. 获取自车在当前滑动窗口下的真实 S 坐标
    // 注意：在此局部坐标系下，假设自车始终位于原点 (0.0, 0.0)
    frenet::FrenetPoint ego_sl;
    if (!frenet::CartesianFrenetConverter::cartesian_to_frenet(ref_line, 0.0, 0.0, ego_sl)) {
        std::cerr << "[ObstacleExtractor] Failed to get ego Frenet state!" << std::endl;
        return dangerous_obs;
    }

    // 2. 遍历栅格地图并进行坐标系降维与视野过滤
    for (size_t i = 0; i < occupancy_map.size(); ++i) {
        // 仅处理占据概率大于 0.5 的实体栅格
        if (occupancy_map[i] > 0.5) {
            int col = i % map_cfg.width;
            int row = i / map_cfg.width;
            
            // 计算栅格中心点的真实笛卡尔坐标
            double x = map_cfg.origin_x + col * map_cfg.resolution + map_cfg.resolution / 2.0;
            double y = map_cfg.origin_y + row * map_cfg.resolution + map_cfg.resolution / 2.0;

            frenet::FrenetPoint sl_pt;
            if (frenet::CartesianFrenetConverter::cartesian_to_frenet(ref_line, x, y, sl_pt)) {
                
                // 核心过滤逻辑：量产级相对视野过滤 (ROI Filtering)
                // 扫描范围：自车尾部后方 10 米 到 车头前方 s_lookahead，且处于横向关注范围内
                if (sl_pt.s > ego_sl.s - 10.0 && 
                    sl_pt.s < ego_sl.s + config_.s_lookahead && 
                    std::abs(sl_pt.l) < config_.l_half_width) {
                    
                    dangerous_obs.push_back(sl_pt);
                }
            }
        }
    }
    
    return dangerous_obs;
}

} // namespace map_processor
} // namespace localplanner