#include "planner_manager.h"
#include "trajectory/trajectory_evaluator.h"
#include "frenet/cartesian_frenet_converter.h"
#include <iostream>
#include <cmath>

namespace localplanner {

PlannerManager::PlannerManager() {
    veh_cfg_ = path_planner::VehicleConfig(); 
    smoother_ = std::make_unique<ref_line::ReferenceLineSmoother>(ref_line::SmootherConfig{});
    dp_decider_ = std::make_unique<path_planner::PathDeciderDP>(path_planner::PathDeciderDP::Config{}, veh_cfg_);
    s_gen_ = std::make_unique<speed_planner::SpeedGenerator>(speed_planner::SpeedGenerator::Config{0.0, 1.0, 0.3});
    qp_speed_opt_ = std::make_unique<speed_planner::SpeedOptimizerQP>(speed_planner::SpeedOptimizerQP::Config{});
}

bool PlannerManager::RunOnce(const std::vector<ref_line::RawPoint>& raw_ref_points,
                             const std::vector<path_planner::ObstacleSL>& obstacles_sl,
                             std::array<double, 3>& current_path_state,
                             std::array<double, 3>& current_speed_state,
                             double vmax,
                             std::vector<trajectory::TrajectoryPoint>& final_trajectory) {
    
    std::vector<ref_line::ReferencePoint> smooth_ref;
    if (!smoother_->Execute(raw_ref_points, smooth_ref)) {
        std::cerr << "[Manager] Reference Line smoothing failed." << std::endl;
        return false;
    }

    if (!last_trajectory_.empty()) {
        trajectory::TrajectoryEvaluator evaluator(last_trajectory_);
        trajectory::TrajectoryPoint stitch_pt = evaluator.Evaluate(PLANNING_CYCLE_TIME);
        trajectory::TrajectoryPoint next_pt = evaluator.Evaluate(PLANNING_CYCLE_TIME + 0.05);
        
        double current_dl = current_path_state[2]; 
        double ds = next_pt.s - stitch_pt.s;
        if (ds > 1e-3) {
            current_dl = std::max(-0.75, std::min(0.75, (next_pt.l - stitch_pt.l) / ds));
        }
    }

    int em_iter = 0;
    bool em_converged = false;
    std::vector<path_planner::PathPointSL> final_path_sl;
    std::vector<trajectory::TrajectoryPoint> ref_speed_traj;
    double dynamic_weight_ddl = 100.0; 

    double active_speed_limit = vmax; 
    double current_v = current_speed_state[1];

    while (em_iter < MAX_EM_ITER && !em_converged) {
        path_planner::PathOptimizerQP::Config path_cfg;
        path_cfg.weight_ddl = dynamic_weight_ddl;
        path_planner::PathOptimizerQP qp_path_opt(path_cfg);

        std::vector<path_planner::PathPointSL> dp_path;
        
        // 🚀 核心修改：将 last_trajectory_ 喂给 DP 决策器
        dp_decider_->Execute(obstacles_sl, {current_path_state[0], current_path_state[1], 0}, current_v, last_trajectory_, dp_path);
        
        bool path_success = qp_path_opt.Optimize(dp_path, obstacles_sl, current_path_state, current_v, veh_cfg_, smooth_ref, final_path_sl, 0.8);
        
        if (path_success && !final_path_sl.empty()) {
            for (const auto& p : final_path_sl) {
                if (std::isnan(p.l) || std::abs(p.l) > 20.0) { 
                    std::cout << "[Manager] 🚨 FATAL: OSQP Solver outputted GARBAGE! Forcing fallback." << std::endl;
                    path_success = false;
                    final_path_sl.clear(); 
                    break;
                }
            }
        }
        if (!path_success) {
            std::cout << "[Manager] ⚠️ Narrow corridor detected! Triggering survival mode (Margin 0.2m)." << std::endl;
            path_success = qp_path_opt.Optimize(dp_path, obstacles_sl, current_path_state, current_v, veh_cfg_, smooth_ref, final_path_sl, 0.2);
            if (path_success && !final_path_sl.empty()) {
                int count = 0;
                for (const auto& p : final_path_sl) {
                    if (std::isnan(p.l) || std::abs(p.l) > 20.0) { 
                        std::cout << "[Manager] 🚨 FATAL: OSQP Solver outputted GARBAGE! Forcing fallback." << std::endl;
                        std::cout << count << std::endl;
                        path_success = false;
                        final_path_sl.clear(); 
                        break;
                    }
                    count ++;
                }
            }
            if (path_success) {
                active_speed_limit = 5.0 / 3.6; 
            } else {
                std::cerr << "[Manager] ❌ Path QP Failed even in survival mode!" << std::endl;
                break; 
            }
        } else {
            active_speed_limit = vmax; 
        }

        ref_speed_traj.clear();
        s_gen_->Generate(smooth_ref, final_path_sl, ref_speed_traj, active_speed_limit); 
        
        double min_v = 99.0;
        for (const auto& p : ref_speed_traj) if (p.v < min_v) min_v = p.v;

        if (min_v < 4.1 && em_iter < MAX_EM_ITER - 1) {
            dynamic_weight_ddl *= 10.0; 
            em_iter++;
        } else {
            em_converged = true;
        }
    }

    if (!em_converged || final_path_sl.empty() || ref_speed_traj.empty()) {
        std::cout << "\n[Manager] 🚨 FATAL: Path blocked! Engaging AEB (Emergency Brake)!" << std::endl;
        final_trajectory.clear();

        double current_s = current_path_state[0];
        double current_l = current_path_state[1];
        double current_v_state = current_speed_state[1];
        double brake_decel = -4.0; 
        
        for (int i = 0; i < 40; ++i) { 
            double t = i * 0.1;
            trajectory::TrajectoryPoint tp;
            tp.t = t;
            
            tp.v = std::max(0.0, current_v_state + brake_decel * t);
            tp.a = (tp.v > 0.0) ? brake_decel : 0.0;
            
            double move_dist = 0.0;
            if (tp.v > 0.0) {
                move_dist = current_v_state * t + 0.5 * brake_decel * t * t;
            } else {
                double stop_t = current_v_state / std::abs(brake_decel);
                move_dist = current_v_state * stop_t + 0.5 * brake_decel * stop_t * stop_t;
            }
            tp.s = current_s + move_dist;
            
            tp.l = current_l; 
            
            frenet::CartesianFrenetConverter::frenet_to_cartesian(smooth_ref, {tp.s, tp.l}, tp.x, tp.y);
            final_trajectory.push_back(tp);
        }

        double fallback_theta = 0.0;
        if (!last_trajectory_.empty()) {
            fallback_theta = last_trajectory_.size() > 1 ? last_trajectory_[1].theta : last_trajectory_.front().theta;
        }

        for (size_t i = 0; i < final_trajectory.size(); ++i) {
            size_t j = i + 1;
            bool found_far_point = false;
            while (j < final_trajectory.size()) {
                double dist = std::hypot(final_trajectory[j].x - final_trajectory[i].x, 
                                         final_trajectory[j].y - final_trajectory[i].y);
                if (dist > 0.2) { 
                    found_far_point = true;
                    break;
                }
                j++;
            }
            
            if (found_far_point) {
                double dx = final_trajectory[j].x - final_trajectory[i].x;
                double dy = final_trajectory[j].y - final_trajectory[i].y;
                final_trajectory[i].theta = std::atan2(dy, dx);
            } else if (i > 0) {
                final_trajectory[i].theta = final_trajectory[i-1].theta;
            } else {
                final_trajectory[i].theta = fallback_theta;
            }
        }
        
        if (final_trajectory.size() > 1) {
            final_trajectory.back().theta = final_trajectory[final_trajectory.size()-2].theta;
        }

        last_trajectory_ = final_trajectory;
        return false; 
    }

    double v_cruise = ref_speed_traj.front().v;
    
    bool success = qp_speed_opt_->Optimize(v_cruise, current_speed_state, ref_speed_traj, final_path_sl, smooth_ref, final_trajectory);
    if (success) {
        last_trajectory_ = final_trajectory; 
    } else {
        std::cerr << "[Manager] Speed QP Failed! Outputting internal emergency trajectory." << std::endl;
        last_trajectory_ = final_trajectory; 
    }

    return success;
}

} // namespace localplanner