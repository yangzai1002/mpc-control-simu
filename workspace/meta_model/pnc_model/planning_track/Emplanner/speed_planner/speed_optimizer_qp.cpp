#include "speed_optimizer_qp.h"
#include "speed_piecewise_jerk_problem.h"
#include "../frenet/cartesian_frenet_converter.h" 
#include "../trajectory/trajectory_evaluator.h" 
#include <iostream>
#include <cmath>
#include <algorithm>

namespace localplanner {
namespace speed_planner {

namespace { // 匿名命名空间，封装内部辅助函数

/**
 * @brief 基于纵向弧长 S 的线性插值，映射获取路径的 SL 状态
 */
path_planner::PathPointSL InterpolatePath(const std::vector<path_planner::PathPointSL>& path, double s) {
    if (path.empty()) return {};
    if (s <= path.front().s) return path.front();
    if (s >= path.back().s) return path.back();

    auto it = std::lower_bound(path.begin(), path.end(), s,
                               [](const path_planner::PathPointSL& p, double val) { return p.s < val; });
    auto prev = std::prev(it);
    double ratio = (s - prev->s) / (it->s - prev->s);
    
    path_planner::PathPointSL res;
    res.s = s;
    res.l = prev->l + ratio * (it->l - prev->l);
    res.dl = prev->dl + ratio * (it->dl - prev->dl);
    res.ddl = prev->ddl + ratio * (it->ddl - prev->ddl);

    res.left_bound = prev->left_bound + ratio * (it->left_bound - prev->left_bound);
    res.right_bound = prev->right_bound + ratio * (it->right_bound - prev->right_bound);

    return res;
}

} // namespace anonymous

SpeedOptimizerQP::SpeedOptimizerQP(const Config& config) : config_(config) {}

bool SpeedOptimizerQP::Optimize(const double v_limit,
                                const std::array<double, 3>& init_state,
                                const std::vector<trajectory::TrajectoryPoint>& ref_traj,
                                const std::vector<path_planner::PathPointSL>& path_sl,
                                const std::vector<ref_line::ReferencePoint>& ref_line,
                                std::vector<trajectory::TrajectoryPoint>& optimized_tp) const {
    
    if (ref_traj.empty()) {
        std::cerr << "[SpeedOptimizer] Ref trajectory empty. Triggering Fallback." << std::endl;
        return GenerateFallbackTrajectory(init_state, path_sl, ref_line, optimized_tp);
    }

    trajectory::TrajectoryEvaluator ref_eval(ref_traj);
    optimized_tp.clear();
    optimized_tp.reserve(config_.n);

    // 1. 直接按时间维度等距采样提取粗轨迹物理曲线
    for (int i = 0; i < config_.n; ++i) {
        double t = i * config_.delta_t;
        optimized_tp.push_back(ref_eval.Evaluate(t));
    }

    // 2. 状态强行对齐：将起点对齐到底盘当前真实的物理状态
    optimized_tp[0].s = init_state[0];
    optimized_tp[0].v = init_state[1];
    optimized_tp[0].a = init_state[2];

    // 3. 正向动力学推演修复 (防止 QP 求解在低速起步死锁)
    const double MAX_ACC = 2.0; 
    for (int i = 1; i < config_.n; ++i) {
        double dt = config_.delta_t;
        double v_acc_limit = optimized_tp[i-1].v + MAX_ACC * dt;
        
        // 取期望规划速度与物理极限速度的较小值，无缝咬合避障曲线
        optimized_tp[i].v = std::min(optimized_tp[i].v, v_acc_limit);
        
        // 重新积分更新 S 和 A，保证物理连贯性
        optimized_tp[i].s = optimized_tp[i-1].s + 0.5 * (optimized_tp[i-1].v + optimized_tp[i].v) * dt;
        optimized_tp[i].a = (optimized_tp[i].v - optimized_tp[i-1].v) / dt;
    }

    // 4. 执行最终的空间映射此时空融合
    SpatiotemporalFusion(path_sl, ref_line, optimized_tp);
    return true;
}

bool SpeedOptimizerQP::GenerateFallbackTrajectory(
    const std::array<double, 3>& init_state, 
    const std::vector<path_planner::PathPointSL>& path_sl,
    const std::vector<ref_line::ReferencePoint>& ref_line,
    std::vector<trajectory::TrajectoryPoint>& fallback_tp) const {
    
    fallback_tp.clear();
    fallback_tp.reserve(config_.n);

    double current_s = init_state[0];
    double current_v = init_state[1];
    const double max_deceleration = -4.0; // AEB 紧急制动强度

    for (int i = 0; i < config_.n; ++i) {
        trajectory::TrajectoryPoint tp;
        tp.t = i * config_.delta_t;
        
        current_v = std::max(0.0, current_v + max_deceleration * config_.delta_t);
        double delta_s = current_v * config_.delta_t + 0.5 * max_deceleration * std::pow(config_.delta_t, 2);
        
        if (current_v > 0.0) {
            current_s += std::max(0.0, delta_s);
            tp.a = max_deceleration;
        } else {
            tp.a = 0.0;
        }

        tp.s = current_s;
        tp.v = current_v;
        fallback_tp.push_back(tp);
    }

    // 保底轨迹同样进行时空融合，保证车辆刹停时方向盘姿态可控
    SpatiotemporalFusion(path_sl, ref_line, fallback_tp);
    return true;
}

void SpeedOptimizerQP::SpatiotemporalFusion(const std::vector<path_planner::PathPointSL>& path_sl,
                                            const std::vector<ref_line::ReferencePoint>& ref_line,
                                            std::vector<trajectory::TrajectoryPoint>& trajectory) const {
    if (path_sl.empty() || ref_line.empty()) return;

    for (auto& tp : trajectory) {
        // 1. S -> SL 映射
        auto p_sl = InterpolatePath(path_sl, tp.s);
        tp.l = p_sl.l;
        
        // 补齐参考线的真实物理曲率
        double ref_kappa = 0.0;
        auto it = std::lower_bound(ref_line.begin(), ref_line.end(), tp.s,
                                   [](const ref_line::ReferencePoint& p, double val) { return p.s < val; });
        if (it != ref_line.end()) {
            ref_kappa = it->kappa;
        }
        
        // 最终下发控制的复合曲率 = 参考线基础曲率 + 局部规划诱导的二阶导
        tp.kappa = ref_kappa + p_sl.ddl;
        tp.left_bound = p_sl.left_bound;
        tp.right_bound = p_sl.right_bound;

        // 2. SL -> Cartesian (XY) 投影
        frenet::CartesianFrenetConverter::frenet_to_cartesian(ref_line, {tp.s, tp.l}, tp.x, tp.y);
    }

    // 3. 计算航向角 Theta (基于空间前瞻基线，彻底消除极低速蠕行时的投影抖动)
    for (size_t i = 0; i < trajectory.size(); ++i) {
        size_t j = i + 1;
        bool found_far_point = false;
        
        // 向前寻找至少 0.3 米的空间距离作为角度计算的物理基线
        while (j < trajectory.size()) {
            double dist = std::hypot(trajectory[j].x - trajectory[i].x, 
                                     trajectory[j].y - trajectory[i].y);
            if (dist > 0.3) {
                found_far_point = true;
                break;
            }
            j++;
        }

        if (found_far_point) {
            double dx = trajectory[j].x - trajectory[i].x;
            double dy = trajectory[j].y - trajectory[i].y;
            trajectory[i].theta = std::atan2(dy, dx);
        } else if (i > 0) {
            // 前方已无位移量 (车辆基本刹停)，锁定方向盘姿态
            trajectory[i].theta = trajectory[i-1].theta;
        } else {
            // 极低速原地起步，启用数学解析解作为保底
            double ref_theta = ref_line.front().theta;
            auto p_sl = InterpolatePath(path_sl, trajectory[i].s);
            trajectory[i].theta = ref_theta + std::atan(p_sl.dl);
        }
    }
}

} // namespace speed_planner
} // namespace localplanner