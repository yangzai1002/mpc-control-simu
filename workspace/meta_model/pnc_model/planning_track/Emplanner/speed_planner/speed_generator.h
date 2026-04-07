#pragma once

#include <vector>
#include "../path_planner/path_data_types.h"
#include "../trajectory/trajectory_data_types.h"
#include "../reference_line/ref_data_types.h"

namespace localplanner {
namespace speed_planner {

/**
 * @brief 初始速度轮廓生成器
 * @details 根据平滑后的空间路径，结合曲率限速、动力学前向/后向扫描，生成带时间戳 (T) 的初始粗轨迹
 */
class SpeedGenerator {
public:
    struct Config {
        double max_v = 15.0;     ///< 默认巡航限速 (m/s)
        double max_ay = 2.0;     ///< 最大侧向加速度限制，用于曲率降速防侧翻 (m/s^2)
        double min_v = 1.0;      ///< 避障时的保底蠕行速度 (m/s)
    };

    /**
     * @brief 构造函数
     */
    explicit SpeedGenerator(const Config& config);

    /**
     * @brief 核心生成函数：Path -> 粗糙 Trajectory
     * @param[in] ref_line 平滑参考线
     * @param[in] path_points 优化后的空间 SL 路径
     * @param[out] trajectory 输出带有 S, V, A, T 信息的初始轨迹
     * @param[in] speed_limit_override 外部强制临时限速指令 (m/s)，若小于0则使用默认配置
     */
    void Generate(const std::vector<ref_line::ReferencePoint>& ref_line,
                  const std::vector<path_planner::PathPointSL>& path_points,
                  std::vector<trajectory::TrajectoryPoint>& trajectory,
                  const double speed_limit_override = -1.0) const;
    // void Generate(
    // const std::vector<ref_line::ReferencePoint>& ref_line,
    // const std::vector<path_planner::PathPointSL>& path_points,
    // std::vector<trajectory::TrajectoryPoint>& trajectory,
    // const double speed_limit_override,
    // const double init_speed,    // 【新增】车辆当前速度（上一帧末速度）
    // const double init_acc       // 【新增】车辆当前加速度
    // ) const;

private:
    Config config_;
};

} // namespace speed_planner
} // namespace localplanner