#include "speed_generator.h"
#include "../frenet/cartesian_frenet_converter.h"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace localplanner {
namespace speed_planner {

SpeedGenerator::SpeedGenerator(const Config& config) : config_(config) {}

void SpeedGenerator::Generate(const std::vector<ref_line::ReferencePoint>& ref_line,
                              const std::vector<path_planner::PathPointSL>& path_points,
                              std::vector<trajectory::TrajectoryPoint>& trajectory,
                              const double speed_limit_override) const {
    if (path_points.empty()) return;

    trajectory.clear();
    // 利用 C++ 特性，默认构造函数会自动将所有成员初始化为 0.0，杜绝了危险的 memset
    std::vector<trajectory::TrajectoryPoint> raw_tp(path_points.size());
    double g_dis_to_end = 0.0;

   // std::cout <<"path_points.size():"<<path_points.size()<<std::endl;
    // 1. 空间反求与基于曲率的物理限速 (Raw Speed Limit)
    for (size_t i = 0; i < path_points.size(); ++i) {
        const auto& p_sl = path_points[i];
        double x = 0.0, y = 0.0;
        
        if (!frenet::CartesianFrenetConverter::frenet_to_cartesian(ref_line, {p_sl.s, p_sl.l}, x, y)) {
            // 防止投影失败导致的坐标丢失，退化继承上一帧
            if (i > 0) {
                x = raw_tp[i - 1].x; 
                y = raw_tp[i - 1].y; 
            }
        }
        
        raw_tp[i].x = x; 
        raw_tp[i].y = y; 
        raw_tp[i].s = p_sl.s; 
        raw_tp[i].l = p_sl.l;
        raw_tp[i].kappa = p_sl.ddl; // 近似物理曲率
        
        double abs_kappa = std::abs(raw_tp[i].kappa);
        double active_max_v = (speed_limit_override > 0.0) ? speed_limit_override : config_.max_v;
        
        double v_safe = active_max_v;
        if (abs_kappa > 0.002) { 
            v_safe = std::min(v_safe, std::sqrt(config_.max_ay / abs_kappa));
        }
        
        // 动态保底速度匹配
        double actual_min_v = std::min(config_.min_v, active_max_v);
        raw_tp[i].v = std::max(v_safe, actual_min_v);
        
        // 基于距离终点的强制接管降速
       // double dynamic_end_s = ref_line.empty() ? p_sl.s : ref_line.back().s; 
        double dynamic_end_s = path_points.back().s;
        double dist_to_end = dynamic_end_s - raw_tp[i].s;
        
        if (dist_to_end <= 0.0) {
            raw_tp[i].v = 0.0; 
        } else if (dist_to_end < 20.0) {
            // 最后 20 米规划平滑降速曲线 (a = -1.0 m/s^2)
            double v_stop = std::max(0.0, std::sqrt(2.0 * 1.0 * dist_to_end)); 
            raw_tp[i].v = std::min(raw_tp[i].v, v_stop);
        }
        
        g_dis_to_end = dist_to_end;
         
    }

    // 2. 向前扫描 (Forward Pass): 限制底盘最大加速能力
    const double max_acc = 2.0; 
    for (size_t i = 1; i < raw_tp.size(); ++i) {
        double ds = raw_tp[i].s - raw_tp[i-1].s;
        if (ds > 0) {
            double v_forward = std::sqrt(raw_tp[i-1].v * raw_tp[i-1].v + 2.0 * max_acc * ds);
            raw_tp[i].v = std::min(raw_tp[i].v, v_forward);
            
        }
    }

    // 3. 向后扫描 (Backward Pass): 入弯前瞻减速 & 盲区保底刹停
    raw_tp.back().v = 0.0; 
    const double max_dec = 2.0; 
    for (int i = static_cast<int>(raw_tp.size()) - 2; i >= 0; --i) {
        double ds = raw_tp[i+1].s - raw_tp[i].s;
        if (ds > 0) {
            double v_backward = std::sqrt(raw_tp[i+1].v * raw_tp[i+1].v + 2.0 * max_dec * ds);
            raw_tp[i].v = std::min(raw_tp[i].v, v_backward);
        
      }  

    }
    
    // for (int i = static_cast<int>(raw_tp.size()) - 2; i >= 0; --i)
    // {
    //     raw_tp[i].v = std::max(raw_tp[i].v,0.2);
    // }
    
    // 4. 时序对齐与运动学推导  
    double current_t = 0.0;
    for (size_t i = 0; i < raw_tp.size(); ++i) {
        if (i > 0) {
            double ds = raw_tp[i].s - raw_tp[i-1].s;
            double avg_v = (raw_tp[i].v + raw_tp[i-1].v) / 2.0;
            
            // 设定极小速度下限防止除零或时间无限拉长
            current_t += ds / std::max(0.01, avg_v); 
            
            raw_tp[i].theta = std::atan2(raw_tp[i].y - raw_tp[i-1].y, raw_tp[i].x - raw_tp[i-1].x);
            
            double dt = current_t - raw_tp[i-1].t;
            raw_tp[i].a = (raw_tp[i].v - raw_tp[i-1].v) / std::max(dt, 1e-6);
        } else {
            raw_tp[i].theta = 0.0; 
            raw_tp[i].a = 0.0;
        }
        raw_tp[i].t = current_t;
        trajectory.push_back(raw_tp[i]);
    }
} 
} // namespace localplanner
} // namespace speed_planner


// #include "speed_generator.h"
// #include "../frenet/cartesian_frenet_converter.h"
// #include <cmath>
// #include <algorithm>
// #include <iostream>

// namespace localplanner {
// namespace speed_planner {

// SpeedGenerator::SpeedGenerator(const Config& config) : config_(config) {}

// void SpeedGenerator::Generate(
//     const std::vector<ref_line::ReferencePoint>& ref_line,
//     const std::vector<path_planner::PathPointSL>& path_points,
//     std::vector<trajectory::TrajectoryPoint>& trajectory,
//     const double speed_limit_override,
//     const double init_speed,    // 【新增】车辆当前速度（上一帧末速度）
//     const double init_acc       // 【新增】车辆当前加速度
// ) const {
//     if (path_points.empty()) return;

//     trajectory.clear();
//     std::vector<trajectory::TrajectoryPoint> raw_tp(path_points.size());
//     double g_dis_to_end = 0.0;

//     // ==============================================
//     // 1. 空间反求 + 曲率物理限速 + 终点减速（不变）
//     // ==============================================
//     for (size_t i = 0; i < path_points.size(); ++i) {
//         const auto& p_sl = path_points[i];
//         double x = 0.0, y = 0.0;
        
//         if (!frenet::CartesianFrenetConverter::frenet_to_cartesian(ref_line, {p_sl.s, p_sl.l}, x, y)) {
//             if (i > 0) {
//                 x = raw_tp[i - 1].x;
//                 y = raw_tp[i - 1].y;
//             }
//         }
        
//         raw_tp[i].x = x;
//         raw_tp[i].y = y;
//         raw_tp[i].s = p_sl.s;
//         raw_tp[i].l = p_sl.l;
//         raw_tp[i].kappa = p_sl.ddl;
        
//         double abs_kappa = std::abs(raw_tp[i].kappa);
//         double active_max_v = (speed_limit_override > 0.0) ? speed_limit_override : config_.max_v;
        
//         double v_safe = active_max_v;
//         if (abs_kappa > 0.002) {
//             v_safe = std::min(v_safe, std::sqrt(config_.max_ay / abs_kappa));
//         }
        
//         double actual_min_v = std::min(config_.min_v, active_max_v);
//         raw_tp[i].v = std::max(v_safe, actual_min_v);
        
//         double dynamic_end_s = ref_line.empty() ? p_sl.s : ref_line.back().s;
//         double dist_to_end = dynamic_end_s - raw_tp[i].s;
        
//         if (dist_to_end <= 0.0) {
//             raw_tp[i].v = 0.0;
//         } else if (dist_to_end < 20.0) {
//             double v_stop = std::max(0.0, std::sqrt(2.0 * 1.0 * dist_to_end));
//             raw_tp[i].v = std::min(raw_tp[i].v, v_stop);
//         }
//         g_dis_to_end = dist_to_end;
//     }

//     // ==============================================
//     // 2. 前向扫描：从【当前车速】开始加速，不是从0！
//     // ==============================================
//     const double max_acc = 2.0;
//     // 【关键修复】第一个点必须继承当前速度，不能重新从0起步
//     raw_tp[0].v = std::clamp(init_speed, 0.0, config_.max_v);

//     for (size_t i = 1; i < raw_tp.size(); ++i) {
//         double ds = raw_tp[i].s - raw_tp[i-1].s;
//         if (ds > 0) {
//             double v_forward = std::sqrt(raw_tp[i-1].v * raw_tp[i-1].v + 2.0 * max_acc * ds);
//             raw_tp[i].v = std::min(raw_tp[i].v, v_forward);
//         }
//     }

//     // ==============================================
//     // 3. 后向扫描：保证入弯减速 + 终点刹停（不变，但更平滑）
//     // ==============================================
//     const double max_dec = 2.0;
//     for (int i = static_cast<int>(raw_tp.size()) - 2; i >= 0; --i) {
//         double ds = raw_tp[i+1].s - raw_tp[i].s;
//         if (ds > 0) {
//             double v_backward = std::sqrt(raw_tp[i+1].v * raw_tp[i+1].v + 2.0 * max_dec * ds);
//             raw_tp[i].v = std::min(raw_tp[i].v, v_backward);
//         }
//     }

//     // 【可选】全局保底速度，防止车停死
//     const double min_speed_floor = 0.1;
//     for (size_t i = 0; i < raw_tp.size() - 1; ++i) {
//         raw_tp[i].v = std::max(raw_tp[i].v, min_speed_floor);
//     }
//     raw_tp.back().v = 0.0; // 最后一点必须0

//     // ==============================================
//     // 4. 时序推导：从当前时间开始，连续平滑
//     // ==============================================
//     double current_t = 0.0;
//     // 【关键修复】第一点加速度继承当前车辆加速度
//     raw_tp[0].a = init_acc;
//     raw_tp[0].t = current_t;

//     for (size_t i = 1; i < raw_tp.size(); ++i) {
//         double ds = raw_tp[i].s - raw_tp[i-1].s;
//         double avg_v = (raw_tp[i].v + raw_tp[i-1].v) / 2.0;
//         current_t += ds / std::max(0.01, avg_v);
        
//         raw_tp[i].theta = std::atan2(raw_tp[i].y - raw_tp[i-1].y,
//                                      raw_tp[i].x - raw_tp[i-1].x);
        
//         double dt = current_t - raw_tp[i-1].t;
//         raw_tp[i].a = (raw_tp[i].v - raw_tp[i-1].v) / std::max(dt, 1e-6);
//         raw_tp[i].t = current_t;
//     }

//     // 输出最终轨迹
//     trajectory.swap(raw_tp);
// }

// } // namespace speed_planner
// } // namespace localplanner
