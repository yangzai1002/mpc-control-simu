#ifndef INPUT_DATA_HPP_
#define INPUT_DATA_HPP_

#include "comm/comm_struct.hpp"
#include "core/types.h"
#include "planning/ego_trajectory.h"
#include "control/chassis_report.h"

namespace trajectory_follower
{

struct LateralSyncData
{
    bool is_steer_converged
    { false };
};

struct LongitudinalSyncData
{
    bool is_velocity_converged
    { false };
};

struct InputData
{
    Adsfi::HafEgoTrajectory current_trajectory;
    //adsfi::PlanningResult current_trajectory;
    //nav_msgs::Odometry current_odometry;
    Adsfi::HafPose current_pose;
    Adsfi::HafTwist current_twist;

    commStruct::SteeringReport current_steering;

    // 底盘报告 [vel(m/s), steerAngle(rad), actualGear, ]
    HafChassisReport current_chassis_report;

    // 当前加速度
    double current_accel;

    double first_point_x = 0.0;
    double first_point_y = 0.0;

};

}    // namespace

#endif  // INPUT_DATA_HPP_
