#pragma once

#include <vector>
#include <array>
#include <memory>

#include "reference_line/smoother.h"
#include "path_planner/path_decider_dp.h"
#include "path_planner/path_optimizer_qp.h"
#include "speed_planner/speed_generator.h"
#include "speed_planner/speed_optimizer_qp.h"
#include "trajectory/trajectory_data_types.h"

namespace localplanner {

/**
 * @brief 局部规划器大管家 (Planner Manager)
 * @details 负责统筹调用参考线平滑、路径 DP+QP 优化、速度生成与 QP 优化，并维护 E-M 交替优化大闭环与紧急接管 (AEB) 逻辑
 */
class PlannerManager {
public:
    PlannerManager();
    ~PlannerManager() = default;

    /**
     * @brief 规划器主干线单帧执行入口
     * @param[in] raw_ref_points 粗糙的全局路由局部参考点集
     * @param[in] obstacles_sl 投影到 Frenet 坐标系的有效障碍物列表
     * @param[in,out] current_path_state 车辆当前真实空间状态 [s, l, dl]
     * @param[in,out] current_speed_state 车辆当前真实纵向状态 [s, v, a]
     * @param[in] vmax 当前路段或指令下发的最高限速 (m/s)
     * @param[out] final_trajectory 输出最终可执行的时空平滑轨迹
     * @return 正常规控流程是否成功 (若失败，将触发并输出安全的 AEB 刹停轨迹)
     */
    bool RunOnce(const std::vector<ref_line::RawPoint>& raw_ref_points,
                 const std::vector<path_planner::ObstacleSL>& obstacles_sl,
                 std::array<double, 3>& current_path_state,  
                 std::array<double, 3>& current_speed_state, 
                 double vmax,
                 std::vector<trajectory::TrajectoryPoint>& final_trajectory);

private:
    // --- 规控核心组件实例 ---
    path_planner::VehicleConfig veh_cfg_;
    std::unique_ptr<ref_line::ReferenceLineSmoother> smoother_;
    std::unique_ptr<path_planner::PathDeciderDP> dp_decider_;
    std::unique_ptr<speed_planner::SpeedGenerator> s_gen_;
    std::unique_ptr<speed_planner::SpeedOptimizerQP> qp_speed_opt_;

    // --- 内部状态缓存与常量配置 ---
    std::vector<trajectory::TrajectoryPoint> last_trajectory_; ///< 缓存上一帧的轨迹，用于拼接与防抖
    const double PLANNING_CYCLE_TIME = 0.1; ///< 规划系统周期 (100ms)
    const int MAX_EM_ITER = 2;              ///< E-M 算法最大交替迭代次数
};

} // namespace localplanner