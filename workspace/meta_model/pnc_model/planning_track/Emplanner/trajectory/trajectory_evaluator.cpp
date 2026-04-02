#include "trajectory_evaluator.h"
#include <algorithm>
#include <cmath>

namespace localplanner {
namespace trajectory {

TrajectoryEvaluator::TrajectoryEvaluator(const std::vector<TrajectoryPoint>& trajectory) 
    : trajectory_(trajectory) {}

TrajectoryPoint TrajectoryEvaluator::Evaluate(double t) const {
    // 异常与边界保护
    if (trajectory_.empty()) return {};
    if (t <= trajectory_.front().t) return trajectory_.front();
    if (t >= trajectory_.back().t) return trajectory_.back();

    // 1. 寻找覆盖目标时间 t 的时间窗口 [prev_it, next_it]
    auto next_it = std::lower_bound(trajectory_.begin(), trajectory_.end(), t,
        [](const TrajectoryPoint& tp, double time) {
            return tp.t < time;
        });
    
    // 由于前面已经处理了边界条件，这里的 prev 和 next 一定是合法的相邻节点
    auto prev_it = std::prev(next_it);
    const TrajectoryPoint& prev = *prev_it;
    const TrajectoryPoint& next = *next_it;

    double dt_total = next.t - prev.t;
    double dt_target = t - prev.t;
    
    // 防止极其微小的时间间隔导致除零异常
    if (dt_total < 1e-6) return prev;

    double ratio = dt_target / dt_total;

    // 2. 执行插值计算
    TrajectoryPoint target;
    target.t = t;

    // 运动学插值 (Kinematic Interpolation)
    // 采用线性插值保证加速度和速度变化的连续平顺性
    target.v = prev.v + (next.v - prev.v) * ratio;
    target.a = prev.a + (next.a - prev.a) * ratio; 
    
    // 使用梯形积分（平均速度）推导位置 S，严格保证空间推进与速度的物理一致性
    target.s = prev.s + 0.5 * (prev.v + target.v) * dt_target;
    
    // 空间几何与姿态插值 (Geometric Interpolation)
    target.x = prev.x + (next.x - prev.x) * ratio;
    target.y = prev.y + (next.y - prev.y) * ratio;
    
    // 注意：航向角理论上应使用 SLERP(球面线性插值) 以防止 +-PI 处的跳变
    // 但因轨迹点足够密集，且车辆无法瞬间完成大角度转向，此处直接线性插值满足工程精度
    target.theta = prev.theta + (next.theta - prev.theta) * ratio;
    target.kappa = prev.kappa + (next.kappa - prev.kappa) * ratio;
    
    // Frenet 横向偏移插值 (确保轨迹拼接时不会出现被强制拉回中心线的阶跃)
    target.l = prev.l + (next.l - prev.l) * ratio;

    // 边界透传
    target.left_bound = prev.left_bound + (next.left_bound - prev.left_bound) * ratio;
    target.right_bound = prev.right_bound + (next.right_bound - prev.right_bound) * ratio;

    return target;
}

} // namespace trajectory
} // namespace localplanner