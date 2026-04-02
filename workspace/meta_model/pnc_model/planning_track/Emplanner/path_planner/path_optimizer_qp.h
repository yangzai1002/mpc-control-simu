#pragma once

#include <vector>
#include <array>
#include "path_data_types.h"
#include "piecewise_jerk_problem.h"
#include "../reference_line/ref_data_types.h"

namespace localplanner {
namespace path_planner {

/**
 * @brief 基于二次规划的路径平滑优化器
 * @details 接收 DP 生成的粗糙路径折线和障碍物边界，结合车辆运动学极限，生成平滑且连续的 SL 轨迹
 */
class PathOptimizerQP {
public:
    struct Config {
        double delta_s = 2.0;         ///< QP 优化的空间离散步长 (m)
        double weight_l = 15.0;       ///< 偏离中心参考线权重
        double weight_dl = 80.0;      ///< 航向角变化权重
        double weight_ddl = 100.0;    ///< 曲率权重
        double weight_dddl = 1000.0;  ///< Jerk 权重，保证高速行驶时的方向盘稳定性
    };

    /**
     * @brief 构造函数
     * @param[in] config 优化器配置
     */
    explicit PathOptimizerQP(const Config& config);

    /**
     * @brief 优化主函数
     * @param[in] dp_path DP 产生的粗糙参考折线
     * @param[in] obstacles 环境障碍物列表
     * @param[in] init_state 初始状态 [l, dl, ddl]
     * @param[in] current_v 当前车速 (m/s)，用于动态计算前瞻距离
     * @param[in] veh_cfg 车辆物理尺寸配置
     * @param[in] ref_line 平滑参考线，用于提取基准曲率
     * @param[out] final_path 输出的最优平滑 SL 轨迹
     * @param[in] obstacle_margin 动态安全边距 (m)
     * @return 优化是否成功
     */
    bool Optimize(const std::vector<PathPointSL>& dp_path,
                  const std::vector<ObstacleSL>& obstacles,
                  const std::array<double, 3>& init_state,
                  const double current_v, 
                  const VehicleConfig& veh_cfg,
                  const std::vector<ref_line::ReferencePoint>& ref_line,
                  std::vector<PathPointSL>& final_path,
                  const double obstacle_margin = 0.8) const;

private:
    Config config_;
};

} // namespace path_planner
} // namespace localplanner