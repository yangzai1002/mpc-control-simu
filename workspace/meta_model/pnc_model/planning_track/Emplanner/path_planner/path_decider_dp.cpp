#include "path_decider_dp.h"
#include <cmath>
#include <algorithm>

namespace localplanner {
namespace path_planner {

namespace {
// 内部辅助函数：在历史轨迹中插值寻找对应 S 处的 L 值
double GetHistoricalL(const std::vector<trajectory::TrajectoryPoint>& last_traj, double target_s) {
    if (last_traj.empty()) return 0.0;
    if (target_s <= last_traj.front().s) return last_traj.front().l;
    if (target_s >= last_traj.back().s) return last_traj.back().l;
    auto it = std::lower_bound(last_traj.begin(), last_traj.end(), target_s,
                               [](const trajectory::TrajectoryPoint& p, double s) { return p.s < s; });
    auto prev = std::prev(it);
    double ratio = (target_s - prev->s) / (it->s - prev->s + 1e-6);
    return prev->l + ratio * (it->l - prev->l);
}
} // namespace

bool PathDeciderDP::Execute(const std::vector<ObstacleSL>& obstacles,
                            const PathPointSL& init_sl,
                            double current_v, 
                            const std::vector<trajectory::TrajectoryPoint>& last_trajectory, 
                            std::vector<PathPointSL>& dp_path) const {
    
    double lookahead_s = std::max(60.0, (current_v * current_v) / (2.0 * 2.0) + 20.0);
    int dynamic_s_nodes = static_cast<int>(lookahead_s / config_.s_interval) + 1;
    dynamic_s_nodes = std::min(dynamic_s_nodes, 60); 

    std::vector<std::vector<SamplePoint>> grid(config_.l_nodes_num, 
                                               std::vector<SamplePoint>(dynamic_s_nodes));

    // 1. 初始化第一列
    for (int r = 0; r < config_.l_nodes_num; ++r) {
        auto& node = grid[r][0];
        node.point.s = init_sl.s + config_.s_interval;
        node.point.l = (r - config_.l_nodes_num / 2) * config_.l_interval;
        
        double obs_cost = CalculateCollisionCost(node.point, obstacles);
        double ref_cost = std::abs(node.point.l) * config_.weight_ref;
        
        // 🚀 核心改造：历史平滑继承与滞后环
        double hist_cost = 0.0;
        if (!last_trajectory.empty()) {
            double hist_l = GetHistoricalL(last_trajectory, node.point.s);
            double dl_hist = node.point.l - hist_l;
            // 二次方惩罚：倾向于贴合历史轨迹
            hist_cost = dl_hist * dl_hist * config_.weight_history;
            
            // 滞后环 (Hysteresis Loop)：惩罚跨越中心线的拓扑横跳
            if (hist_l * node.point.l < 0.0 && std::abs(hist_l) > 0.3) {
                hist_cost += 3000.0; // 3000的阻力，可被碰撞极值(1e8)轻松击穿
            }
        } else {
            // 无历史时，保留轻微交规偏好 (向左绕)
            if (node.point.l < 0.0) ref_cost += std::abs(node.point.l) * 2.0; 
        }

        double inertia_cost = std::abs(node.point.l - init_sl.l) * 5.0;
        node.min_cost = obs_cost + ref_cost + inertia_cost + hist_cost;
    }

    // 2. 规划递推
    for (int s_idx = 1; s_idx < dynamic_s_nodes; ++s_idx) {
        for (int l_curr = 0; l_curr < config_.l_nodes_num; ++l_curr) {
            auto& curr_node = grid[l_curr][s_idx];
            curr_node.point.s = init_sl.s + (s_idx + 1) * config_.s_interval;
            curr_node.point.l = (l_curr - config_.l_nodes_num / 2) * config_.l_interval;
            
            double curr_obs_cost = CalculateCollisionCost(curr_node.point, obstacles);
            double ref_cost = std::abs(curr_node.point.l) * config_.weight_ref;
            
            // 同步应用历史平滑继承与滞后环
            double hist_cost = 0.0;
            if (!last_trajectory.empty()) {
                double hist_l = GetHistoricalL(last_trajectory, curr_node.point.s);
                double dl_hist = curr_node.point.l - hist_l;
                hist_cost = dl_hist * dl_hist * config_.weight_history;
                
                if (hist_l * curr_node.point.l < 0.0 && std::abs(hist_l) > 0.3) {
                    hist_cost += 3000.0;
                }
            } else {
                if (curr_node.point.l < 0.0) ref_cost += std::abs(curr_node.point.l) * 2.0;
            }

            for (int l_prev = 0; l_prev < config_.l_nodes_num; ++l_prev) {
                const auto& prev_node = grid[l_prev][s_idx - 1];
                double dl = std::abs(curr_node.point.l - prev_node.point.l) / config_.s_interval;
                double edge_cost = dl * dl * config_.weight_dl;
                
                double total_cost = prev_node.min_cost + curr_obs_cost + edge_cost + ref_cost + hist_cost;
                if (total_cost < curr_node.min_cost) {
                    curr_node.min_cost = total_cost;
                    curr_node.parent_row = l_prev;
                }
            }
        }
    }

    // 3. 回溯最优路径
    int best_l_idx = 0;
    double min_final_cost = 1e20;
    for (int r = 0; r < config_.l_nodes_num; ++r) {
        if (grid[r][dynamic_s_nodes - 1].min_cost < min_final_cost) {
            min_final_cost = grid[r][dynamic_s_nodes - 1].min_cost;
            best_l_idx = r;
        }
    }

    for (int s_idx = dynamic_s_nodes - 1; s_idx >= 0; --s_idx) {
        dp_path.push_back(grid[best_l_idx][s_idx].point);
        best_l_idx = grid[best_l_idx][s_idx].parent_row;
    }
    std::reverse(dp_path.begin(), dp_path.end());

    return !dp_path.empty();
}

double PathDeciderDP::CalculateCollisionCost(const PathPointSL& point, 
                                            const std::vector<ObstacleSL>& obstacles) const {
    double total_cost = 0.0;
    const double safe_buffer = 1.0; 
    
    for (double offset : veh_cfg_.disc_offsets) {
        double disc_s = point.s + offset;
        double disc_l = point.l; 

        for (const auto& obs : obstacles) {
            if (disc_s > obs.start_s - safe_buffer && disc_s < obs.end_s + safe_buffer) {
                double dist_to_left = disc_l - obs.left_l;
                double dist_to_right = obs.right_l - disc_l;
                double min_dist = 1e6;
                
                if (disc_l < obs.left_l && disc_l > obs.right_l) min_dist = 0.0; 
                else if (disc_l >= obs.left_l) min_dist = dist_to_left;
                else min_dist = dist_to_right;

                if (min_dist <= veh_cfg_.disc_radius) return config_.weight_obs; 
                else if (min_dist < veh_cfg_.disc_radius + safe_buffer) {
                    double danger_dist = (veh_cfg_.disc_radius + safe_buffer) - min_dist;
                    total_cost += 1000.0 * std::exp(danger_dist * 2.0); 
                }
            }
        }
    }
    return total_cost;
}

} // namespace path_planner
} // namespace localplanner