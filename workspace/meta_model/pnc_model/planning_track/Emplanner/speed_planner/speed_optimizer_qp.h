#pragma once

#include <vector>
#include <array>
#include "../trajectory/trajectory_data_types.h"
#include "../path_planner/path_data_types.h"      
#include "../reference_line/ref_data_types.h"     

namespace localplanner {
namespace speed_planner {

/**
 * @brief 速度平滑优化与时空融合器
 * @details 承担规控后处理的最后一道工序，将独立的 Path (空间) 和 Speed (时间) 结果融合成高频底盘直接可用的时空 Trajectory
 */
class SpeedOptimizerQP {
public:
    struct Config {
        double delta_t = 0.2;       ///< QP 优化的时间离散步长 (s)
        int n = 40;                 ///< 优化前瞻点数 (总时长 = n * delta_t)
        
        double weight_v_ref = 10.0; ///< 速度跟随权重
        double weight_a = 50.0;     ///< 加速度惩罚权重
        double weight_jerk = 500.0; ///< Jerk 惩罚权重 (提升乘坐舒适性)
    };

    explicit SpeedOptimizerQP(const Config& config);

    /**
     * @brief 优化主函数：时空融合
     * @param[in] v_limit 当前路段巡航限速 (m/s)
     * @param[in] init_state 初始纵向状态 [s0, v0, a0]
     * @param[in] ref_traj 粗糙参考轨迹 (由 SpeedGenerator 提供)
     * @param[in] path_sl 优化后的空间路径 (用于映射横向状态)
     * @param[in] ref_line 平滑参考线 (用于坐标系反求)
     * @param[out] optimized_tp 输出完整、平滑、可执行的最优时空轨迹
     * @return 优化是否成功
     */
    bool Optimize(const double v_limit,
                  const std::array<double, 3>& init_state,
                  const std::vector<trajectory::TrajectoryPoint>& ref_traj,
                  const std::vector<path_planner::PathPointSL>& path_sl,
                  const std::vector<ref_line::ReferencePoint>& ref_line,
                  std::vector<trajectory::TrajectoryPoint>& optimized_tp) const;

private:
    /**
     * @brief 生成紧急保底刹停轨迹
     * @details 当常规规划失败时，直接接管并生成极限刹停轨迹以保障安全
     */
    bool GenerateFallbackTrajectory(const std::array<double, 3>& init_state, 
                                    const std::vector<path_planner::PathPointSL>& path_sl,
                                    const std::vector<ref_line::ReferencePoint>& ref_line,
                                    std::vector<trajectory::TrajectoryPoint>& fallback_tp) const;

    /**
     * @brief 内部核心逻辑：执行时空投影融合 (SL -> XY -> Theta)
     */
    void SpatiotemporalFusion(const std::vector<path_planner::PathPointSL>& path_sl,
                              const std::vector<ref_line::ReferencePoint>& ref_line,
                              std::vector<trajectory::TrajectoryPoint>& trajectory) const;

    Config config_;
};

} // namespace speed_planner
} // namespace localplanner