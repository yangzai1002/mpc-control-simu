#ifndef SMOOTH_STOP_H
#define SMOOTH_STOP_H

#include <optional>
#include <cmath>
#include <vector>
#include <chrono>

using namespace std;

namespace pid_longitudinal_controller
{

    // 重力加速度常量（m/s²）
const double GRAVITY = 9.81;

// 默认反应时间（s）：自动驾驶系统典型值
const double DEFAULT_REACT_TIME = 0.5;

// 路面类型枚举，对应不同附着系数
enum class RoadType {
    DRY,        // 干燥路面
    WET,        // 湿滑路面
    ICE_SNOW,   // 冰雪路面
    GRASS       // 草地/砂石
};


class SmoothStop
{

public:

    void init(const double pred_vel_in_target, const double pred_stop_dist);

    void setParams(double max_strong_acc, double min_strong_acc,
                   double weak_acc, double weak_stop_acc,
                   double strong_stop_acc, double min_fast_vel,
                   double min_running_vel, double min_running_acc,
                   double weak_stop_time, double weak_stop_dist,
                   double strong_stop_dist,int road_type);

    double calcTimeToStop(
            const std::vector<std::pair<double, double>> &vel_hist) const;

    double calculate(const double stop_dist, const double current_vel,
                     const double current_acc,
                     const std::vector<std::pair<double, double>> &vel_hist,
                     const double delay_time);
    double calculateBrakingDistance(double speed_kmh, 
        double react_time = DEFAULT_REACT_TIME);

private:
    struct Params
    {
        double max_strong_acc;
        double min_strong_acc;
        double weak_acc;
        double weak_stop_acc;
        double strong_stop_acc;

        double min_fast_vel;
        double min_running_vel;
        double min_running_acc;
        double weak_stop_time;

        double weak_stop_dist;
        double strong_stop_dist;
        int road_type;
    };

    Params m_params;

    double m_strong_acc;
    double m_weak_acc_time;
    bool m_is_set_params = false;

};

}    // namespace

#endif  // SMOOTH_STOP_H
