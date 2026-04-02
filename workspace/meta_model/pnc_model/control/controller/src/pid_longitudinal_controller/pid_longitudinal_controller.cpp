#include "pid_longitudinal_controller/pid_longitudinal_controller.hpp"

namespace pid_longitudinal_controller
{

//
double getNowTime()
{
    double stamp;

    const auto now =
            std::chrono::high_resolution_clock::now().time_since_epoch();
    uint32_t sec =
            std::chrono::duration_cast<std::chrono::seconds>(now).count();
    uint32_t nsec =
            std::chrono::duration_cast<std::chrono::nanoseconds>(now).count()
                    % 1000000000UL;
    stamp = sec + nsec / 1e9;

    return stamp;
}

PidLongitudinalController::PidLongitudinalController(std::string config_path)
{
    YAML::Node config;

    try
    {
        config = YAML::LoadFile(config_path);
    }
    catch (...)
    {
        std::cout << "The format of config file " << config_path
                << " is wrong. Please check (e.g. indentation)." << std::endl;
    }

    YAML::Node common_params = yamlSubNodeAbort(config, "parameters");

    yamlRead<double>(common_params, "ctrl_period", m_longitudinal_ctrl_period,
            double(3.0));

    yamlRead<double>(common_params, "delay_compensation_time",
            m_delay_compensation_time, double(7.0));

    yamlRead<bool>(common_params, "enable_smooth_stop", m_enable_smooth_stop,
            bool(true));

    yamlRead<bool>(common_params, "enable_overshoot_emergency",
            m_enable_overshoot_emergency, bool(false));

    yamlRead<bool>(common_params, "enable_large_tracking_error_emergency",
            m_enable_large_tracking_error_emergency, bool(true));

    yamlRead<bool>(common_params, "enable_keep_stopped_until_steer_convergence",
            m_enable_keep_stopped_until_steer_convergence, bool(true));

    {
        auto &p = m_state_transition_params;

        yamlRead<double>(common_params, "drive_state_stop_dist",
                p.drive_state_stop_dist, double(3.0));

        yamlRead<double>(common_params, "drive_state_offset_stop_dist",
                p.drive_state_offset_stop_dist, double(7.0));

        yamlRead<double>(common_params, "stopping_state_stop_dist",
                p.stopping_state_stop_dist, double(0.4));

        yamlRead<double>(common_params, "stopped_state_entry_duration_time",
                p.stopped_state_entry_duration_time, double(0.4));

        yamlRead<double>(common_params, "stopped_state_entry_vel",
                p.stopped_state_entry_vel, double(0.4));

        yamlRead<double>(common_params, "stopped_state_entry_acc",
                p.stopped_state_entry_acc, double(0.4));

        yamlRead<double>(common_params, "emergency_state_overshoot_stop_dist",
                p.emergency_state_overshoot_stop_dist, double(0.4));

        yamlRead<double>(common_params, "emergency_state_traj_trans_dev",
                p.emergency_state_traj_trans_dev, double(0.4));

        yamlRead<double>(common_params, "emergency_state_traj_rot_dev",
                p.emergency_state_traj_rot_dev, double(0.4));
    }

    {
        double kp, ki, kd;

        yamlRead<double>(common_params, "kp", kp, double(0.4));
        yamlRead<double>(common_params, "ki", ki, double(0.4));
        yamlRead<double>(common_params, "kd", kd, double(0.4));

        m_pid_vel.setGains(kp, ki, kd);

        const double max_pid = common_params["max_out"].as<double>();    // [m/s^2]
        const double min_pid = common_params["min_out"].as<double>();    // [m/s^2]
        const double max_p = common_params["max_p_effort"].as<double>();    // [m/s^2]
        const double min_p = common_params["min_p_effort"].as<double>();    // [m/s^2]
        const double max_i = common_params["max_i_effort"].as<double>();    // [m/s^2]
        const double min_i = common_params["min_i_effort"].as<double>();    // [m/s^2]
        const double max_d = common_params["max_d_effort"].as<double>();    // [m/s^2]
        const double min_d = common_params["min_d_effort"].as<double>();    // [m/s^2]

        m_pid_vel.setLimits(max_pid, min_pid, max_p, min_p, max_i, min_i, max_d,
                min_d);

        const double lpf_vel_error_gain =
                common_params["lpf_vel_error_gain"].as<double>();

        m_lpf_vel_error = std::make_shared<LowpassFilter1d>(0.0,
                lpf_vel_error_gain);

        m_current_vel_threshold_pid_integrate =
                common_params["current_vel_threshold_pid_integration"].as<double>();

        m_enable_brake_keeping_before_stop =
                common_params["enable_brake_keeping_before_stop"].as<bool>();

        m_brake_keeping_acc = common_params["brake_keeping_acc"].as<double>();
    }

    {

        const double max_strong_acc =
                common_params["smooth_stop_max_strong_acc"].as<double>();
        const double min_strong_acc =
                common_params["smooth_stop_min_strong_acc"].as<double>();
        const double weak_acc =
                common_params["smooth_stop_weak_acc"].as<double>();
        const double weak_stop_acc =
                common_params["smooth_stop_weak_stop_acc"].as<double>();
        const double strong_stop_acc =
                common_params["smooth_stop_strong_stop_acc"].as<double>();
        const double max_fast_vel =
                common_params["smooth_stop_max_fast_vel"].as<double>();
        const double min_running_vel =
                common_params["smooth_stop_min_running_vel"].as<double>();
        const double min_running_acc =
                common_params["smooth_stop_min_running_acc"].as<double>();
        const double weak_stop_time =
                common_params["smooth_stop_weak_stop_time"].as<double>();
        const double weak_stop_dist =
                common_params["smooth_stop_weak_stop_dist"].as<double>();
        const double strong_stop_dist =
                common_params["smooth_stop_strong_stop_dist"].as<double>();

        const int road_type =
        common_params["smooth_stop_road_type"].as<int>();
        m_smooth_stop.setParams(max_strong_acc, min_strong_acc, weak_acc,
                weak_stop_acc, strong_stop_acc, max_fast_vel, min_running_vel,
                min_running_acc, weak_stop_time, weak_stop_dist,
                strong_stop_dist,road_type);
    }

    {
        auto &p = m_stopped_state_params;
        p.vel = common_params["stopped_vel"].as<double>();
        p.acc = common_params["stopped_acc"].as<double>();
        p.jerk = common_params["stopped_jerk"].as<double>();
    }

    {
        auto &p = m_emergency_state_params;
        p.vel = common_params["emergency_vel"].as<double>();
        p.acc = common_params["emergency_acc"].as<double>();
        p.jerk = common_params["emergency_jerk"].as<double>();
    }

    m_max_acc = common_params["max_acc"].as<double>();
    m_min_acc = common_params["min_acc"].as<double>();

    m_max_jerk = common_params["max_jerk"].as<double>();
    m_min_jerk = common_params["min_jerk"].as<double>();
}

bool PidLongitudinalController::isReady(
        const trajectory_follower::InputData &input_data)
{
    return true;
}

void PidLongitudinalController::setKinematicState(const Adsfi::HafPose current_pose,
   const Adsfi::HafTwist current_twist,double current_vel)
{
    m_current_pose = current_pose;
    m_current_twist = current_twist;
    m_current_vel = current_vel;
}

void PidLongitudinalController::setCurrentOperationMode(const uint8_t &msg)
{
    m_current_operation_mode = msg;
}

void PidLongitudinalController::setTrajectory(
        const Adsfi::HafEgoTrajectory &trajectory)
{
    m_hafTrajectory = trajectory;

    m_trajectory = commStruct::convertToMPCTrajectory(m_hafTrajectory);

    m_autoTrajectory = commStruct::convertToAutowareTrajectory(m_trajectory);

}

void PidLongitudinalController::setCurrentAcceleration(const double &msg)
{
    m_current_accel = msg;
}

//输出速度和加速度
trajectory_follower::LongitudinalOutput PidLongitudinalController::run(
        trajectory_follower::InputData const &input_data)
{
    setTrajectory(input_data.current_trajectory);

    setKinematicState(input_data.current_pose,input_data.current_twist,input_data.current_chassis_report.velocity);

    setCurrentAcceleration(input_data.current_accel);

    Adsfi::HafPose current_pose = m_current_pose;

    const auto control_data = getControlData(current_pose);

    updateControlState(control_data);

    const Motion ctrl_cmd = calcCtrlCmd(current_pose, control_data);

    const auto cmd_msg = createCtrlCmdMsg(ctrl_cmd,
            control_data.current_motion.vel);

    trajectory_follower::LongitudinalOutput output;
    output.control_cmd = cmd_msg;

    // 根据实车速度和期望速度, 算出加速度/减速度. 然后根据刹车和油门标定表, 计算对应的油门值和刹车值.
    if (output.control_cmd.acceleration > 0.0)
    {
        output.control_cmd.throttle = output.control_cmd.acceleration
                / m_max_acc;
        output.control_cmd.brake = 0.0;
    }
    else
    {
        output.control_cmd.throttle = 0.0;
        output.control_cmd.brake = output.control_cmd.acceleration / m_min_acc;
    }

    return output;

}

PidLongitudinalController::ControlData PidLongitudinalController::getControlData(
        const Adsfi::HafPose &current_pose)
{
    ControlData control_data {};

    control_data.dt = getDt();

//     control_data.current_motion.vel = std::sqrt(
//             pow(m_current_twist.linear.x, 2)
//                     + pow(m_current_twist.linear.y, 2)
//                     + pow(m_current_twist.linear.z, 2));
    control_data.current_motion.vel = m_current_vel;

    control_data.current_motion.acc = m_current_accel;
    control_data.nearest_idx =
            commStruct::findFirstNearestIndexWithSoftConstraints(
                    m_autoTrajectory, current_pose);

    control_data.is_far_from_trajectory = false;

    control_data.shift = getCurrentShift(control_data.nearest_idx);    //方向改变
    if (control_data.shift != m_prev_shift)
    {
        m_pid_vel.reset();
    }

    m_prev_shift = control_data.shift;

    control_data.stop_dist = longitudinal_utils::calcStopDistance(current_pose,
            m_trajectory, control_data.nearest_idx);

    return control_data;
}

PidLongitudinalController::Motion PidLongitudinalController::calcEmergencyCtrlCmd(
        const double dt) const
{
    const auto &p = m_emergency_state_params;

    const double vel = longitudinal_utils::applyDiffLimitFilter(p.vel,
            m_prev_raw_ctrl_cmd.vel, dt, p.acc);

    const double acc = longitudinal_utils::applyDiffLimitFilter(p.acc,
            m_prev_raw_ctrl_cmd.acc, dt, p.jerk);

    return Motion
    { vel, acc };
}

void PidLongitudinalController::updateControlState(
        const ControlData &control_data)
{
    const double current_vel = control_data.current_motion.vel;
    const double current_acc = control_data.current_motion.acc;
    const double stop_dist = control_data.stop_dist;
    const auto &p = m_state_transition_params;

    const bool departure_condition_from_stopping = false;
    const bool departure_condition_from_stopped = true;/*后续这个标志从其它地方获取，首先设定为true*/


    const bool keep_stopped_condition =
            m_enable_keep_stopped_until_steer_convergence
                    && (!lateral_sync_data_.is_steer_converged);    //!lateral_sync_data_.is_steer_converged
   // std::cout <<"current_vel::"<<current_vel<<std::endl;

    double current_stop_dist = m_smooth_stop.calculateBrakingDistance(current_vel*3.6);

    /*speed delay 1s */

    current_stop_dist = std::max((current_stop_dist + 1.0 * current_vel),2.0);

    //std::cout <<"stop_dist:"<<stop_dist<<std::endl;
    const bool stopping_condition = stop_dist < current_stop_dist;

    //std::cout <<"current_stop_dist:"<<current_stop_dist<<std::endl;


    ///std::cout <<"p.stopped_state_entry_vel:"<<p.stopped_state_entry_vel<<"current_vel"<<current_vel<<std::endl;
    //std::cout <<" p.stopped_state_entry_acc:"<<p.stopped_state_entry_vel<<"current_acc"<<current_acc<<std::endl;
    //std::cout <<"lateral_sync_data_.is_steer_converged:"<<lateral_sync_data_.is_steer_converged<<std::endl;
    if (std::fabs(current_vel) > p.stopped_state_entry_vel
            || std::fabs(current_acc) > p.stopped_state_entry_acc)
    {
        m_last_running_time = getNowTime();
    }

   bool stopped_condition = false;

   if((getNowTime() - m_last_running_time)> p.stopped_state_entry_duration_time || 
   (!lateral_sync_data_.is_steer_converged))
   {
        stopped_condition = true;
   } 

   //std::cout <<"stopped_condition:"<<stopped_condition<<std::endl;

    static constexpr double vel_epsilon = 1e-3;

    // const double current_vel_cmd = std::fabs(
    //         m_trajectory.vx.at(control_data.nearest_idx));

    /* 目标速度取最近点 2 米范围内的最小速度 */
    
    const double SEARCH_RANGE = 2.0;  // 2米范围

    int nearest_idx = static_cast<int>(control_data.nearest_idx);
    int vx_size = static_cast<int>(m_trajectory.vx.size());
    //std::cout <<"control_data.nearest_idx:"<<control_data.nearest_idx<<"vx_size:"<<vx_size<<std::endl;
    // 1. 向前找 2米 内的起始索引
    int start_idx = nearest_idx;
    double forward_dist = 0.0;
    while (start_idx > 0) {
        // 计算相邻点距离（假设轨迹点是等距/连续路径，用坐标差计算）
        double dx = m_trajectory.x.at(start_idx) - m_trajectory.x.at(start_idx - 1);
        double dy = m_trajectory.y.at(start_idx) - m_trajectory.y.at(start_idx - 1);
        double dist = std::hypot(dx, dy);

        if (forward_dist + dist > SEARCH_RANGE) break;

        forward_dist += dist;
        start_idx--;
    }

    // 2. 向后找 2米 内的结束索引
    int end_idx = nearest_idx;
    double backward_dist = 0.0;
    while (end_idx < vx_size - 1) {
        double dx = m_trajectory.x.at(end_idx + 1) - m_trajectory.x.at(end_idx);
        double dy = m_trajectory.y.at(end_idx + 1) - m_trajectory.y.at(end_idx);
        double dist = std::hypot(dx, dy);

        if (backward_dist + dist > SEARCH_RANGE) break;

        backward_dist += dist;
        end_idx++;
    }
    std::cout <<"start_idx:"<<start_idx<<"end_idx:"<<end_idx<<std::endl;

    // 3. 在 [start_idx, end_idx] 范围内找最小速度（绝对值）
    auto min_it = std::min_element(
        m_trajectory.vx.begin() + start_idx,
        m_trajectory.vx.begin() + end_idx + 1,
        [](double a, double b) {
            return std::fabs(a) < std::fabs(b);
        }
    );

    const double current_vel_cmd = std::fabs(*min_it);
    

   
    const bool emergency_condition = m_enable_overshoot_emergency
            && stop_dist < p.emergency_state_overshoot_stop_dist
            && current_vel_cmd < vel_epsilon;

    const bool has_nonzero_target_vel = std::abs(current_vel_cmd) > 0.1;

   // std::cout <<"current_vel_cmd:"<<current_vel_cmd<<"has_nonzero_target_vel:"<<has_nonzero_target_vel<<std::endl;

    const auto changeState = [this](const auto s)
    {
        if (s != m_control_state)
        {
            ;
        }

        m_control_state = s;

        return;
    };

    if (m_control_state == ControlState::DRIVE)
    {

        if (m_enable_smooth_stop)
        {
            if (stopping_condition)
            {
                const double pred_vel_in_target =
                        predictedVelocityInTargetPoint(
                                control_data.current_motion,
                                m_delay_compensation_time);

                const double pred_stop_dist = control_data.stop_dist
                        - 0.5 * (pred_vel_in_target + current_vel)
                                * m_delay_compensation_time;

                m_smooth_stop.init(pred_vel_in_target, pred_stop_dist);

                return changeState(ControlState::STOPPING);
            }
        }
       
        if (stopped_condition)
        {
        return changeState(ControlState::STOPPED);
        }
        

        return;
    }

    if (m_control_state == ControlState::STOPPING)
    {
        if (emergency_condition)
        {
            return changeState(ControlState::EMERGENCY);
        }

        if (stopped_condition)
        {
            return changeState(ControlState::STOPPED);
        }

        if (departure_condition_from_stopping)
        {
            m_pid_vel.reset();
            m_lpf_vel_error->reset(0.0);
            m_prev_ctrl_cmd.acc = std::max(0.0, m_prev_ctrl_cmd.acc);

            return changeState(ControlState::DRIVE);
        }

        return;
    }

    if (m_control_state == ControlState::STOPPED)
    {
        if (has_nonzero_target_vel && !departure_condition_from_stopped)
        {
            std::cout
                    << "target speed > 0, but departure condition is not met. Keep STOPPED."<<std::endl;
            return changeState(ControlState::STOPPED);
        }

        // if (has_nonzero_target_vel && keep_stopped_condition)
        // {
        //     HAF_LOG_INFO
        //             << "target speed > 0, but keep stop condition is met. Keep STOPPED.";
        //     return changeState(ControlState::STOPPED);
        // }

        if (keep_stopped_condition)
        {
                 std::cout
                    << "keep_stopped_condition true." <<std::endl;
            return changeState(ControlState::STOPPED);
        }

        if (departure_condition_from_stopped && has_nonzero_target_vel)
        {
            m_pid_vel.reset();
            m_lpf_vel_error->reset(0.0);
            m_prev_ctrl_cmd.acc = std::max(0.0, m_prev_ctrl_cmd.acc);
            return changeState(ControlState::DRIVE);
        }
        return changeState(ControlState::STOPPED);

        return;
    }

    if (m_control_state == ControlState::EMERGENCY)
    {
        if (stopped_condition && !emergency_condition)
        {
            return changeState(ControlState::STOPPED);
        }

        return;
    }

    return;
}

PidLongitudinalController::Motion PidLongitudinalController::calcCtrlCmd(
        const Adsfi::HafPose &current_pose,
        const ControlData &control_data)
{

    const size_t nearest_idx = control_data.nearest_idx;
    const double current_vel = control_data.current_motion.vel;
    const double current_acc = control_data.current_motion.acc;

    // 速度和加速度命令
    Motion raw_ctrl_cmd {};
    Motion target_motion {};

    if (m_control_state == ControlState::DRIVE)
    {
        //
        target_motion = Motion
        { m_trajectory.vx.at(nearest_idx), 0 };

        const double pred_vel_in_target = predictedVelocityInTargetPoint(
                control_data.current_motion, m_delay_compensation_time);

        raw_ctrl_cmd.acc = applyVelocityFeedback(target_motion, control_data.dt,
                pred_vel_in_target);

        raw_ctrl_cmd.vel = raw_ctrl_cmd.acc * control_data.dt
                + std::abs(control_data.current_motion.vel);

        raw_ctrl_cmd.vel = std::max(raw_ctrl_cmd.vel,0.2); /*最小速度0.2*/
        std::cout << " PID STATE: " << " DRIVE " << " [ vel: "
                << raw_ctrl_cmd.vel << ", acc: " << raw_ctrl_cmd.acc << ", dt: "
                << control_data.dt << ", v_curr: " << current_vel << ", v_ref: "
                << target_motion.vel << ", stop_dist: "
                << control_data.stop_dist << " ]"<<std::endl;

    }

    else if (m_control_state == ControlState::STOPPING)
    {
        raw_ctrl_cmd.acc = m_smooth_stop.calculate(control_data.stop_dist,
                current_vel, current_acc, m_vel_hist,
                m_delay_compensation_time);

        if (raw_ctrl_cmd.acc < 0.0)
        {
            raw_ctrl_cmd.vel = std::max(
                    raw_ctrl_cmd.acc * control_data.dt*50.0
                            + std::abs(control_data.current_motion.vel), 0.0);
        }
        else
        {
            raw_ctrl_cmd.vel = std::min(
                    raw_ctrl_cmd.acc * control_data.dt
                            + std::abs(control_data.current_motion.vel), 1.4);
        }

        std::cout  << " PID STATE: " << " STOPPING " << "| vel: "
                << raw_ctrl_cmd.vel << ", acc: " << raw_ctrl_cmd.acc << ", dt: "
                << control_data.dt << ", v_curr: " << current_vel << ", v_ref: "
                << target_motion.vel << ", stop_dist: "
                << control_data.stop_dist<<std::endl;

    }

    //
    else if (m_control_state == ControlState::STOPPED)
    {
        const auto &p = m_stopped_state_params;

        raw_ctrl_cmd.acc = longitudinal_utils::applyDiffLimitFilter(p.acc,
                m_prev_raw_ctrl_cmd.acc, control_data.dt, p.jerk);

        raw_ctrl_cmd.vel = raw_ctrl_cmd.acc * control_data.dt
                + control_data.current_motion.vel;

        raw_ctrl_cmd.vel = std::max(
                    raw_ctrl_cmd.vel ,0.0);

        std::cout << " PID STATE: " << " STOPPED " << " vel: "
                << raw_ctrl_cmd.vel << ", acc: " << raw_ctrl_cmd.acc << ", dt: "
                << control_data.dt << ", v_curr: " << current_vel << ", v_ref: "
                << target_motion.vel << ", stop_dist: "
                << control_data.stop_dist <<std::endl;
    }

    else if (m_control_state == ControlState::EMERGENCY)
    {
        raw_ctrl_cmd = calcEmergencyCtrlCmd(control_data.dt);
    }

    if (m_prev_shift == Shift::Reverse)
    {
        raw_ctrl_cmd.vel = -raw_ctrl_cmd.vel;
    }

    m_prev_raw_ctrl_cmd = raw_ctrl_cmd;

    const double filtered_acc_cmd = calcFilteredAcc(raw_ctrl_cmd.acc,
            control_data);

    const Motion filtered_ctrl_cmd
    { raw_ctrl_cmd.vel, filtered_acc_cmd };

    return filtered_ctrl_cmd;
}

commStruct::LongitudinalCommand PidLongitudinalController::createCtrlCmdMsg(
        const Motion &ctrl_cmd, const double &current_vel)
{
    commStruct::LongitudinalCommand cmd;

    const auto now =
            std::chrono::high_resolution_clock::now().time_since_epoch();
    uint32_t sec =
            std::chrono::duration_cast<std::chrono::seconds>(now).count();
    uint32_t nsec =
            std::chrono::duration_cast<std::chrono::nanoseconds>(now).count()
                    % 1000000000UL;
    cmd.stamp.sec = sec;
    cmd.stamp.nsec = nsec;

    cmd.speed = ctrl_cmd.vel;
    cmd.acceleration = ctrl_cmd.acc;

    m_vel_hist.push_back(
    { getNowTime(), current_vel });

    std::cout  << "[target_speed, current_speed]: (" << cmd.speed << ", "
            << current_vel << ")" << std::endl;

    while (m_vel_hist.size()
            > static_cast<size_t>(0.5 / m_longitudinal_ctrl_period))
    {
        m_vel_hist.erase(m_vel_hist.begin());
    }

    m_prev_ctrl_cmd = ctrl_cmd;

    return cmd;
}

//
double PidLongitudinalController::getDt()
{
    double dt;

    if (!m_prev_control_time)
    {
        dt = m_longitudinal_ctrl_period;
        m_prev_control_time = getNowTime();
    }
    else
    {
        dt = (getNowTime() - m_prev_control_time);
        m_prev_control_time = getNowTime();
    }

    const double max_dt = m_longitudinal_ctrl_period * 2.0;

    const double min_dt = m_longitudinal_ctrl_period * 0.5;

    return std::max(std::min(dt, max_dt), min_dt);
}

//
enum PidLongitudinalController::Shift PidLongitudinalController::getCurrentShift(
        const size_t nearest_idx) const
{
    constexpr double epsilon = 0.001;

    const double target_vel = m_trajectory.vx.at(nearest_idx);

    if (target_vel > epsilon)
    {
        return Shift::Forward;
    }
    else if (target_vel < -epsilon)
    {
        return Shift::Reverse;
    }

    return m_prev_shift;
}

//
double PidLongitudinalController::calcFilteredAcc(
        const double raw_acc, const ControlData &control_data)
{

    const double acc_max_filtered = std::clamp(raw_acc, m_min_acc, m_max_acc);

    storeAccelCmd(acc_max_filtered);

    const double acc_slope_filtered = applySlopeCompensation(acc_max_filtered,
            control_data.slope_angle, control_data.shift);

    const double acc_jerk_filtered = longitudinal_utils::applyDiffLimitFilter(
            acc_slope_filtered, m_prev_ctrl_cmd.acc, control_data.dt,
            m_max_jerk, m_min_jerk);

    return acc_jerk_filtered;
}

//
void PidLongitudinalController::storeAccelCmd(const double accel)
{
    if (m_control_state == ControlState::DRIVE)
    {
        commStruct::LongitudinalCommand cmd;
        const auto now =
                std::chrono::high_resolution_clock::now().time_since_epoch();
        uint32_t sec =
                std::chrono::duration_cast<std::chrono::seconds>(now).count();
        uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(
                now).count() % 1000000000UL;
        cmd.stamp.sec = sec;
        cmd.stamp.nsec = nsec;
        cmd.acceleration = static_cast<decltype(cmd.acceleration)>(accel);

        m_ctrl_cmd_vec.emplace_back(cmd);
    }
    else
    {
        // reset command
        m_ctrl_cmd_vec.clear();
    }

    // remove unused ctrl cmd
    if (m_ctrl_cmd_vec.size() <= 2)
    {
        return;
    }

    if ((getNowTime()
            - (m_ctrl_cmd_vec.at(1).stamp.sec + m_ctrl_cmd_vec.at(1).stamp.nsec))
            > m_delay_compensation_time)
    {
        m_ctrl_cmd_vec.erase(m_ctrl_cmd_vec.begin());
    }

}

//
double PidLongitudinalController::applySlopeCompensation(
        const double input_acc, const double pitch, const Shift shift) const
{
    if (!m_enable_slope_compensation)
    {
        return input_acc;
    }

    const double pitch_limited = std::min(std::max(pitch, m_min_pitch_rad),
            m_max_pitch_rad);

    double sign =
            (shift == Shift::Forward) ? -1 : (shift == Shift::Reverse ? 1 : 0);

    double compensated_acc = input_acc + sign * 9.81 * std::sin(pitch_limited);

    return compensated_acc;
}

//
double PidLongitudinalController::predictedVelocityInTargetPoint(
        const Motion current_motion, const double delay_compensation_time) const
{
    const double current_vel = current_motion.vel;
    const double current_acc = current_motion.acc;

    if (std::fabs(current_vel) < 1e-01)
    {
        return current_vel;
    }

    const double current_vel_abs = std::fabs(current_vel);

    if (m_ctrl_cmd_vec.size() == 0)
    {
        const double pred_vel = current_vel
                + current_acc * delay_compensation_time;
        return pred_vel > 0 ? std::copysign(pred_vel, current_vel) : 0.0;
    }

    double pred_vel = current_vel_abs;
    const auto past_delay_time = getNowTime() - delay_compensation_time;
    for (std::size_t i = 0; i < m_ctrl_cmd_vec.size(); ++i)
    {
        if ((getNowTime()
                - (m_ctrl_cmd_vec.at(i).stamp.sec
                        + m_ctrl_cmd_vec.at(i).stamp.nsec / 1e9)
                < m_delay_compensation_time))
        {
            if (i == 0)
            {
                pred_vel = current_vel_abs
                        + static_cast<double>(m_ctrl_cmd_vec.at(i).acceleration)
                                * delay_compensation_time;

                return pred_vel > 0 ? std::copysign(pred_vel, current_vel) : 0.0;
            }

            const double acc = m_ctrl_cmd_vec.at(i - 1).acceleration;
            const auto curr_time_i = getNowTime();

            const double time_to_next_acc =
                    std::min(
                            (curr_time_i
                                    - (m_ctrl_cmd_vec.at(i - 1).stamp.sec
                                            + m_ctrl_cmd_vec.at(i - 1).stamp.nsec
                                                    / 1e9)),
                            (curr_time_i - past_delay_time));
            pred_vel += acc * time_to_next_acc;
        }
    }

    const double last_acc =
            m_ctrl_cmd_vec.at(m_ctrl_cmd_vec.size() - 1).acceleration;

    const double time_to_current = (getNowTime()
            - (m_ctrl_cmd_vec.at(m_ctrl_cmd_vec.size() - 1).stamp.sec
                    + m_ctrl_cmd_vec.at(m_ctrl_cmd_vec.size() - 1).stamp.nsec
                            / 1e9));

    pred_vel += last_acc * time_to_current;

    return pred_vel > 0 ? std::copysign(pred_vel, current_vel) : 0.0;
}

//
double PidLongitudinalController::applyVelocityFeedback(
        const Motion target_motion, const double dt, const double current_vel)
{
    const double current_vel_abs = std::fabs(current_vel);
    const double target_vel_abs = std::fabs(target_motion.vel);
    const bool is_under_control = (m_current_operation_mode == 1);

    const bool enable_integration = (current_vel_abs
            > m_current_vel_threshold_pid_integrate) && is_under_control;

    const double error_vel_filtered = m_lpf_vel_error->filter(
            target_vel_abs - current_vel_abs);

    std::vector<double> pid_contributions(3);
    const double pid_acc = m_pid_vel.calculate(error_vel_filtered, dt,
            enable_integration, pid_contributions);

    constexpr double ff_scale_max = 2.0;    // for safety
    constexpr double ff_scale_min = 0.5;    // for safety

    const double ff_scale = std::clamp(
            current_vel_abs / std::max(target_vel_abs, 0.1), ff_scale_min,
            ff_scale_max);

    const double ff_acc = target_motion.acc * ff_scale;

    const double feedback_acc = ff_acc + pid_acc;

    return feedback_acc;
}

}    // namespace pid_longitudinal_controller
