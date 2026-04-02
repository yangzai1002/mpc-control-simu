#include "pid_longitudinal_controller/smooth_stop.hpp"

namespace pid_longitudinal_controller
{

// 根据路面类型获取附着系数μ
double getFrictionCoeff(RoadType road_type) {
    switch (road_type) {
        case RoadType::DRY: return 0.8;
        case RoadType::WET: return 0.4;
        case RoadType::ICE_SNOW: return 0.2;
        case RoadType::GRASS: return 0.6;
        default: throw std::invalid_argument("未知路面类型");
    }
}


double getNowTime1()
{
    const auto now =
            std::chrono::high_resolution_clock::now().time_since_epoch();
    uint32_t sec =
            std::chrono::duration_cast<std::chrono::seconds>(now).count();
    uint32_t nsec =
            std::chrono::duration_cast<std::chrono::nanoseconds>(now).count()
                    % 1000000000UL;
    double stamp = sec * 1.0 + (nsec / 1000000000.0);
    return stamp;
}


// 核心算法：计算总制动距离
// 参数：车速（km/h）、路面类型、反应时间（可选）
// 返回：总制动距离（m）
double SmoothStop::calculateBrakingDistance(double speed_kmh, double react_time) 
{
    // 1. 异常处理：车速不能为负
    if (speed_kmh < 0) {
        //throw std::invalid_argument("车速不能为负数");
        speed_kmh = 0;
    }
    // 2. 单位转换：km/h → m/s
    double speed_mps = speed_kmh / 3.6;
    // 3. 计算反应距离
    double s_react = speed_mps * react_time;
    // 4. 获取路面附着系数
    double mu = getFrictionCoeff(RoadType(m_params.road_type));
    // 5. 计算制动有效距离
    double s_brake = (speed_mps * speed_mps) / (2 * mu * GRAVITY);
    // 6. 总制动距离
    double s_total = s_react + s_brake;
    // 保留2位小数，返回结果
    return std::round(s_total * 100) / 100;
}

//
void SmoothStop::init(const double pred_vel_in_target,
                      const double pred_stop_dist)
{
    m_weak_acc_time = getNowTime1();

    if (pred_stop_dist < std::numeric_limits<double>::epsilon())
    {
        m_strong_acc = m_params.min_strong_acc;
        return;
    }

    m_strong_acc = -std::pow(pred_vel_in_target, 2) / (2 * pred_stop_dist);
    m_strong_acc = std::max(std::min(m_strong_acc, m_params.max_strong_acc),
            m_params.min_strong_acc);
}

void SmoothStop::setParams(double max_strong_acc, double min_strong_acc,
                           double weak_acc, double weak_stop_acc,
                           double strong_stop_acc, double min_fast_vel,
                           double min_running_vel, double min_running_acc,
                           double weak_stop_time, double weak_stop_dist,
                           double strong_stop_dist,int road_type)
{
    m_params.max_strong_acc = max_strong_acc;
    m_params.min_strong_acc = min_strong_acc;
    m_params.weak_acc = weak_acc;
    m_params.weak_stop_acc = weak_stop_acc;
    m_params.strong_stop_acc = strong_stop_acc;

    m_params.min_fast_vel = min_fast_vel;
    m_params.min_running_vel = min_running_vel;
    m_params.min_running_acc = min_running_acc;
    m_params.weak_stop_time = weak_stop_time;

    m_params.weak_stop_dist = weak_stop_dist;
    m_params.strong_stop_dist = strong_stop_dist;
    m_params.road_type = road_type;
    m_is_set_params = true;
    
}

//
double SmoothStop::calcTimeToStop(
        const std::vector<std::pair<double, double>> &vel_hist) const
{
    if (!m_is_set_params)
    {
        throw std::runtime_error(
                "Trying to calculate uninitialized SmoothStop");
    }

    const double vel_hist_size = static_cast<double>(vel_hist.size());
    if (vel_hist_size == 0.0)
    {
        return
        {};
    }

    const double current_ros_time = getNowTime1();
    double mean_t = 0.0;
    double mean_v = 0.0;
    double sum_tv = 0.0;
    double sum_tt = 0.0;

    for (const auto &vel : vel_hist)
    {
        const double t = (vel.first - current_ros_time);
        const double v = vel.second;

        mean_t += t / vel_hist_size;
        mean_v += v / vel_hist_size;
        sum_tv += t * v;
        sum_tt += t * t;
    }

    if (std::abs(vel_hist_size * mean_t * mean_t - sum_tt)
            < std::numeric_limits<double>::epsilon())
    {
        return
        {};
    }

    const double a = (vel_hist_size * mean_t * mean_v - sum_tv)
            / (vel_hist_size * mean_t * mean_t - sum_tt);
    const double b = mean_v - a * mean_t;

    if (std::abs(a) < std::numeric_limits<double>::epsilon())
    {
        return
        {};
    }

    const double time_to_stop = -b / a;
    if (time_to_stop > 0)
    {
        return time_to_stop;
    }

    return
    {};
}

//
double SmoothStop::calculate(
        const double stop_dist, const double current_vel,
        const double current_acc,
        const std::vector<std::pair<double, double>> &vel_hist,
        const double delay_time)
{
    if (!m_is_set_params)
    {
        throw std::runtime_error(
                "Trying to calculate uninitialized SmoothStop");
    }

    double deceleration;

    if (stop_dist < m_params.strong_stop_dist)
    {
        return m_params.strong_stop_acc;
    }
    else
    {
        deceleration = -(current_vel * current_vel / 2 * stop_dist);
        return std::max(deceleration, m_params.strong_stop_acc);
    }

}
}
