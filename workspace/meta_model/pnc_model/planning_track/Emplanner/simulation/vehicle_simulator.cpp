#include "vehicle_simulator.h"
#include <algorithm>

namespace localplanner {
namespace simulation {

VehicleSimulator::VehicleSimulator(const Config& config, const State& init_state)
    : config_(config), state_(init_state) {}

void VehicleSimulator::Update(double target_a, double target_kappa, double dt) {
    // 1. 控制指令限幅保护 (模拟执行器物理极限)
    double cmd_a = std::max(config_.max_dec, std::min(config_.max_acc, target_a));
    
    // 将规划器下发的曲率转化为前轮偏角指令: delta = arctan(kappa * L)
    double cmd_steer = std::atan(target_kappa * config_.wheel_base);
    cmd_steer = std::max(-config_.max_steer_angle, std::min(config_.max_steer_angle, cmd_steer));

    // 假设理想执行器无延迟 (若需更真实，可在此处加入一阶低通滤波模拟方向盘延迟)
    state_.a = cmd_a;
    state_.steer = cmd_steer;

    // 2. 运动学自行车模型进行物理状态积分 (使用前向欧拉法)
    double v_new = state_.v + state_.a * dt;
    v_new = std::max(0.0, v_new); // 简单起见，禁止倒车

    // 航向角变化率: \dot{theta} = (v / L) * tan(delta)
    double omega = (state_.v / config_.wheel_base) * std::tan(state_.steer);
    
    // 位置积分: \dot{x} = v * cos(theta), \dot{y} = v * sin(theta)
    state_.x += state_.v * std::cos(state_.theta) * dt;
    state_.y += state_.v * std::sin(state_.theta) * dt;
    state_.theta += omega * dt;

    // 角度规范化到 [-pi, pi]
    while (state_.theta > M_PI) state_.theta -= 2.0 * M_PI;
    while (state_.theta < -M_PI) state_.theta += 2.0 * M_PI;

    state_.v = v_new;
}

VehicleSimulator::State VehicleSimulator::GetState() const {
    return state_;
}

} // namespace simulation
} // namespace localplanner