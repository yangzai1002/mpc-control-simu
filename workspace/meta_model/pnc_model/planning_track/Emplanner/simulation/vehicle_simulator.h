#pragma once
#include <cmath>
#include <iostream>

namespace localplanner {
namespace simulation {

/**
 * @brief 独立的车辆运动学仿真器 (被控对象 Plant)
 * 接收规划/控制下发的加速度和前轮转角，模拟真实物理世界的车辆运动
 */
class VehicleSimulator {
public:
    // 真实的物理世界状态
    struct State {
        double x = 0.0;
        double y = 0.0;
        double theta = 0.0; // 航向角 (rad)
        double v = 0.0;     // 纵向速度 (m/s)
        double a = 0.0;     // 真实加速度
        double steer = 0.0; // 真实前轮转角 (rad)
    };

    struct Config {
        double wheel_base = 2.8;      // 轴距 (需与车辆配置保持一致)
        double max_steer_angle = 0.6; // 最大前轮转角限制 (rad)
        double max_acc = 1.0;         // 物理最大加速度
        double max_dec = -2.0;        // 物理最大减速度
    };

    VehicleSimulator(const Config& config, const State& init_state);

    /**
     * @brief 物理世界时间步进
     * @param target_a     期望加速度指令 (m/s^2)
     * @param target_kappa 期望曲率指令 (1/m) - 规划器通常输出曲率，在此转化为转角
     * @param dt           仿真步长 (s)
     */
    void Update(double target_a, double target_kappa, double dt);

    // 获取当前真实物理状态
    State GetState() const;

private:
    Config config_;
    State state_;
};

} // namespace simulation
} // namespace localplanner