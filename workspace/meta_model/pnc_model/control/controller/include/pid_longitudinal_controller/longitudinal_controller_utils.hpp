#ifndef LONGITUDINAL_CONTROLLER_UTILS_H
#define LONGITUDINAL_CONTROLLER_UTILS_H

#include "mpc_lateral_controller/mpc_trajectory.hpp"
#include "core/types.h"
#include "comm/comm_struct.hpp"

using namespace mpc_lateral_controller;

namespace pid_longitudinal_controller
{

namespace longitudinal_utils
{

double calcStopDistance(const Adsfi::HafPose &current_pose, const MPCTrajectory traj,
                        uint32_t nearestIndex);

uint32_t searchZeroVelocityIndex(MPCTrajectory traj);

double CalcTwoPointDis(const MPCTrajectory traj, uint32_t point1_index,
                       uint32_t point2_index);

Adsfi::HafPose calcPoseAfterTimeDelay(const Adsfi::HafPose &current_pose, const double delay_time,
                            const double current_vel, const double current_acc);

//Adsfi::HafQuaternion lerpOrientation(const Adsfi::HafQuaternion &o_from, const Adsfi::HafQuaternion &o_to,
 //                          const double ratio);

double applyDiffLimitFilter(const double input_val, const double prev_val,
                            const double dt, const double lim_val);

double applyDiffLimitFilter(const double input_val, const double prev_val,
                            const double dt, const double max_val,
                            const double min_val);

}    // namespace

}    // namespace

#endif  // LONGITUDINAL_CONTROLLER_UTILS_H
