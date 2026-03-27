#ifndef PID_LONGITUDINAL_CONTROLLER_H
#define PID_LONGITUDINAL_CONTROLLER_H


#include "pid_longitudinal_controller/longitudinal_controller_utils.hpp"
#include "pid_longitudinal_controller/lowpass_filter.hpp"
#include "pid_longitudinal_controller/pid.hpp"
#include "pid_longitudinal_controller/smooth_stop.hpp"
#include "trajectory_follower_base/longitudinal_controller_base.hpp"

#include "comm/yaml_reader.hpp"
#include "comm/comm_struct.hpp"
#include "core/logger.h"

using namespace mpc_lateral_controller;
using namespace Adsfi;
using namespace std;

namespace pid_longitudinal_controller
{

class PidLongitudinalController: public trajectory_follower::LongitudinalControllerBase
{

public:
    PidLongitudinalController(std::string config_path);

private:
    struct Motion
    {
        double vel
        { 0.0 };

        double acc
        { 0.0 };
    };

    enum class Shift
    {
        Forward = 0, Reverse
    };

    struct ControlData
    {
        bool is_far_from_trajectory
        { true };

        size_t nearest_idx
        { 0 };

        Motion current_motion {};

        Shift shift
        { Shift::Forward };

        double stop_dist
        { 0.0 };

        double slope_angle
        { 0.0 };

        double dt
        { 0.0 };

    };

    //
    //nav_msgs::Odometry m_current_kinematic_state;
    Adsfi::HafPose m_current_pose;
    Adsfi::HafTwist m_current_twist;
    double m_current_vel;

    double m_current_accel;

    MPCTrajectory m_trajectory;
    //
    Adsfi::HafEgoTrajectory m_hafTrajectory;
    //
    commStruct::Trajectory m_autoTrajectory;

    uint8_t m_current_operation_mode;

    double m_wheel_base;

    enum class ControlState
    {
        DRIVE = 0,    //
        STOPPING,    //
        STOPPED,    //
        EMERGENCY    //
    };

    ControlState m_control_state
    { ControlState::STOPPED };

    std::string toStr(const ControlState s)
    {
        if (s == ControlState::DRIVE)
            return "DRIVE";

        if (s == ControlState::STOPPING)
            return "STOPPING";

        if (s == ControlState::STOPPED)
            return "STOPPED";

        if (s == ControlState::EMERGENCY)
            return "EMERGENCY";

        return "UNDEFINED";
    }
    ;

    double m_longitudinal_ctrl_period;

    double m_delay_compensation_time;

    bool m_enable_smooth_stop;
    bool m_enable_overshoot_emergency;
    bool m_enable_slope_compensation;
    bool m_enable_large_tracking_error_emergency;
    bool m_enable_keep_stopped_until_steer_convergence;
    
    struct StateTransitionParams
    {
        // drive
        double drive_state_stop_dist;
        double drive_state_offset_stop_dist;

        // stopping
        double stopping_state_stop_dist;

        // stop
        double stopped_state_entry_duration_time;
        double stopped_state_entry_vel;
        double stopped_state_entry_acc;

        // emergency
        double emergency_state_overshoot_stop_dist;
        double emergency_state_traj_trans_dev;
        double emergency_state_traj_rot_dev;
    };

    double calc_stopping_state_stop_dist = 0.0;/*according to current vel calc current stop distance*/

    StateTransitionParams m_state_transition_params;

    PIDController m_pid_vel;

    std::shared_ptr<LowpassFilter1d> m_lpf_vel_error
    { nullptr };

    double m_current_vel_threshold_pid_integrate;
    bool m_enable_brake_keeping_before_stop;
    double m_brake_keeping_acc;

    SmoothStop m_smooth_stop;

    struct StoppedStateParams
    {
        double vel;
        double acc;
        double jerk;
    };

    //
    StoppedStateParams m_stopped_state_params;

    struct EmergencyStateParams
    {
        double vel;
        double acc;
        double jerk;
    };

    EmergencyStateParams m_emergency_state_params;

    double m_max_acc;
    double m_min_acc;

    double m_max_jerk;
    double m_min_jerk;

    bool m_use_traj_for_pitch;

    std::shared_ptr<LowpassFilter1d> m_lpf_pitch
    { nullptr };

    double m_max_pitch_rad;
    double m_min_pitch_rad;

    double m_ego_nearest_dist_threshold;
    double m_ego_nearest_yaw_threshold;

    std::vector<commStruct::LongitudinalCommand> m_ctrl_cmd_vec;

    double m_prev_control_time;

    Shift m_prev_shift
    { Shift::Forward };

    Motion m_prev_ctrl_cmd {};

    Motion m_prev_raw_ctrl_cmd {};

    std::vector<std::pair<double, double>> m_vel_hist;

    double m_last_running_time;

    struct DiagnosticData
    {
        double trans_deviation
        { 0.0 };

        double rot_deviation
        { 0.0 };
    };

    DiagnosticData m_diagnostic_data;

public:

    void setKinematicState(const Adsfi::HafPose current_pose,
        const Adsfi::HafTwist current_twist,double current_vel);

    void setCurrentAcceleration(const double &msg);

    void setCurrentOperationMode(const uint8_t &msg);

    void setTrajectory(const Adsfi::HafEgoTrajectory &msg);

    trajectory_follower::LongitudinalOutput run(
            trajectory_follower::InputData const &input_data);

    PidLongitudinalController::ControlData getControlData(
            const Adsfi::HafPose &current_pose);

    Motion calcEmergencyCtrlCmd(const double dt) const;

    void updateControlState(const ControlData &control_data);

    Motion calcCtrlCmd(const Adsfi::HafPose &current_pose,
                       const ControlData &control_data);

    commStruct::LongitudinalCommand createCtrlCmdMsg(const Motion &ctrl_cmd,
                                                     const double &current_vel);

    double getDt();

    Motion getCurrentMotion() const;

    enum Shift getCurrentShift(const size_t nearest_idx) const;

    double calcFilteredAcc(const double raw_acc,
                           const ControlData &control_data);

    void storeAccelCmd(const double accel);

    double applySlopeCompensation(const double acc, const double pitch,
                                  const Shift shift) const;

    double predictedVelocityInTargetPoint(
            const Motion current_motion,
            const double delay_compensation_time) const;

    double applyVelocityFeedback(const Motion target_motion, const double dt,
                                 const double current_vel);

    void updateDebugVelAcc(const Motion &ctrl_cmd,
                           const Adsfi::HafPose &current_pose,
                           const ControlData &control_data);

    bool isReady(const trajectory_follower::InputData &input_data) override;

};
}    // namespace

#endif  // PID_LONGITUDINAL_CONTROLLER_H
