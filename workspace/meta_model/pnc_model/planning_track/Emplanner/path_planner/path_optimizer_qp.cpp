#include "path_optimizer_qp.h"
#include <algorithm>
#include <iostream>

namespace localplanner {
namespace path_planner {

namespace { // 匿名命名空间，封装内部辅助函数防止符号泄漏

/**
 * @brief 通过线性插值获取 DP 路径在特定 S 处的 L 值
 */
double GetDPLatAtS(const std::vector<PathPointSL>& dp_path, double s) {
    if (dp_path.empty()) return 0.0;
    if (s <= dp_path.front().s) return dp_path.front().l;
    if (s >= dp_path.back().s) return dp_path.back().l;

    auto it = std::lower_bound(dp_path.begin(), dp_path.end(), s,
                               [](const PathPointSL& p, double s_val) {
                                   return p.s < s_val;
                               });
    
    auto prev_it = std::prev(it);
    double ratio = (s - prev_it->s) / (it->s - prev_it->s);
    return prev_it->l + ratio * (it->l - prev_it->l);
}

/**
 * @brief 根据 S 值在参考线上插值查找对应的基准曲率 Kappa
 */
double GetRefKappaAtS(const std::vector<ref_line::ReferencePoint>& ref_line, double s) {
    if (ref_line.empty()) return 0.0;
    if (s <= ref_line.front().s) return ref_line.front().kappa;
    if (s >= ref_line.back().s) return ref_line.back().kappa;

    auto it = std::lower_bound(ref_line.begin(), ref_line.end(), s,
                               [](const ref_line::ReferencePoint& p, double s_val) {
                                   return p.s < s_val;
                               });
    
    auto prev_it = std::prev(it);
    double ratio = (s - prev_it->s) / (it->s - prev_it->s);
    return prev_it->kappa + ratio * (it->kappa - prev_it->kappa);
}

} // namespace anonymous

PathOptimizerQP::PathOptimizerQP(const Config& config) : config_(config) {}

bool PathOptimizerQP::Optimize(const std::vector<PathPointSL>& dp_path,
                               const std::vector<ObstacleSL>& obstacles,
                               const std::array<double, 3>& init_state,
                               const double current_v, 
                               const VehicleConfig& veh_cfg,
                               const std::vector<ref_line::ReferencePoint>& ref_line,
                               std::vector<PathPointSL>& final_path,
                               const double obstacle_margin) const {
    if (dp_path.empty()) return false;

    double s_start = init_state[0]; 

    // 1. 动态计算 QP 优化节点数 (根据当前车速计算所需安全视距)
    // 假设最大舒适减速度 2.0m/s^2，附加 20 米安全余量，最低保障 30 米视距
    double lookahead_s = std::max(50.0, (current_v * current_v) / (2.0 * 2.0) + 20.0);
    int n = static_cast<int>(lookahead_s / config_.delta_s) + 1;
    n = std::min(n, 100); // 限制最大节点数，防止计算耗时过长
    
    // 初始化车辆中心点允许活动的物理走廊 (扣除半车身宽度和安全边距)
    double physical_left_bound = 6.0;   
    double physical_right_bound = -6.0; 
    double safe_margin = veh_cfg.width / 2.0 + 0.3; 
    
    double center_left_bound = physical_left_bound - safe_margin;   
    double center_right_bound = physical_right_bound + safe_margin; 
    
    // 初始化边界：{右边界(较小值), 左边界(较大值)}
    std::vector<std::pair<double, double>> l_bounds(n, {center_right_bound, center_left_bound});
    std::vector<double> ref_kappas(n, 0.0); 

    bool is_blocked = false;

    // 2. 生成基于障碍物的动态凸空间隧道 (Convex Space)
    for (int i = 0; i < n; ++i) {
        double curr_s = s_start + i * config_.delta_s;
        ref_kappas[i] = GetRefKappaAtS(ref_line, curr_s); 

        if (is_blocked) {
            l_bounds[i].first = l_bounds[i-1].first;
            l_bounds[i].second = l_bounds[i-1].second;
            continue;
        }

        double dp_l = GetDPLatAtS(dp_path, curr_s);

        // 处理避障几何倒角，防止走廊呈 90 度直角导致求解失败
        for (const auto& obs : obstacles) {
            double start_s = obs.start_s - (veh_cfg.front_hang + obstacle_margin);
            double end_s = obs.end_s + (veh_cfg.rear_hang + obstacle_margin);
            
            // 设定缓和斜率，模拟车辆换道时的平滑过渡
            const double slope = 0.6; 
            double funnel_buffer = 0.0;

            if (curr_s < start_s) {
                funnel_buffer = (start_s - curr_s) * slope; 
            } else if (curr_s > end_s) {
                funnel_buffer = (curr_s - end_s) * slope;
            }

            double half_width = veh_cfg.width / 2.0 + obstacle_margin;

            if (dp_l > (obs.left_l + obs.right_l) / 2.0) {
                // DP 决定从左侧绕行，收缩走廊右边界
                double bound = obs.left_l + half_width - funnel_buffer;
                l_bounds[i].first = std::max(l_bounds[i].first, bound);
            } else {
                // DP 决定从右侧绕行，收缩走廊左边界
                double bound = obs.right_l - half_width + funnel_buffer;
                l_bounds[i].second = std::min(l_bounds[i].second, bound);
            }
        }

        // 终极安全防线：碰撞与容错校验
        double overlap = l_bounds[i].first - l_bounds[i].second;
        if (overlap > -0.1) { 
            // 如果重叠量超过安全冗余，说明发生物理干涉
            if (overlap > 2.0 * obstacle_margin) {
                std::cerr << "[PathOptimizerQP] Physical blockage detected at S=" 
                          << curr_s << "m. Aborting optimization." << std::endl;
                return false; 
            } else {
                // 空间紧凑但物理可行，撕开数学缝隙防止矩阵死锁
                double mid = (l_bounds[i].first + l_bounds[i].second) / 2.0;
                l_bounds[i].first = mid - 0.05;
                l_bounds[i].second = mid + 0.05;
            }
        }

        if (l_bounds[i].second - l_bounds[i].first < 0.05) {
            return false; 
        }
    }

    // 放宽起点约束，允许起始状态的轻微抖动而不崩溃
    l_bounds[0].first = std::min(l_bounds[0].first, init_state[1] - 0.1);
    l_bounds[0].second = std::max(l_bounds[0].second, init_state[1] + 0.1);
    if (n > 1) { 
        l_bounds[1].first = std::min(l_bounds[1].first, init_state[1] - 0.2);
        l_bounds[1].second = std::max(l_bounds[1].second, init_state[1] + 0.2);
    }    

    // 提取阿克曼底盘的运动学极限
    double kappa_max = 1.0 / std::max(1.0, veh_cfg.min_turn_radius);
    double max_ddl_kinematic = kappa_max * 0.9; // 预留 10% 冗余
    double max_dl_kinematic = 1.0;      
    

    // // // 🚀 核心修复 6：彻底放飞 OSQP 求解器的纯数学物理极值！
    // double max_dl_kinematic = 2.0;    // 允许瞬间画出极大偏角      
    // double max_ddl_kinematic = 2.0;   // 允许瞬间爆发极大向心力

    // 3. 配置并执行 OSQP 求解器
    std::array<double, 3> qp_init_state = {init_state[1], init_state[2], 0.0};
    PiecewiseJerkProblem problem(n, config_.delta_s, qp_init_state);
    
    problem.set_weight_l(config_.weight_l);
    problem.set_weight_dl(config_.weight_dl);
    problem.set_weight_ddl(config_.weight_ddl);
    problem.set_weight_dddl(config_.weight_dddl);

    problem.set_x_bounds(l_bounds);
    problem.set_dx_bounds(max_dl_kinematic);       
    problem.set_ddx_bounds(max_ddl_kinematic);     
    problem.set_ref_kappa(ref_kappas);

    std::vector<double> opt_l, opt_dl, opt_ddl;
    if (!problem.Solve(&opt_l, &opt_dl, &opt_ddl)) {
        std::cerr << "[PathOptimizerQP] QP Solver failed to find a feasible path!" << std::endl;
        return false;
    }

    // 4. 封装结果输出
    final_path.clear();
    final_path.reserve(n);
    for (int i = 0; i < n; ++i) {
        PathPointSL p;
        p.s = s_start + i * config_.delta_s;
        p.l = opt_l[i];
        p.dl = opt_dl[i];
        p.ddl = opt_ddl[i];

        // 还原真实的左右边界用于可视化或后续校验
        p.right_bound = l_bounds[i].first - safe_margin;
        p.left_bound  = l_bounds[i].second + safe_margin;

        final_path.push_back(p);
    }

    return true;
}

} // namespace path_planner
} // namespace localplanner