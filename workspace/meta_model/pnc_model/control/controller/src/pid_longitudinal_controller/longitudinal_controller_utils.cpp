#include "pid_longitudinal_controller/longitudinal_controller_utils.hpp"

namespace pid_longitudinal_controller
{

namespace longitudinal_utils
{

double calcStopDistance(const Adsfi::HafPose &current_pose, const MPCTrajectory traj,
                        uint32_t nearestIndex)
{

    double stop_distance = 0.0;

    const auto stop_idx_opt = searchZeroVelocityIndex(traj);

    if ((nearestIndex > stop_idx_opt) && (stop_idx_opt > 1))
    {
        for (uint32_t i = stop_idx_opt; i < nearestIndex - 1; i++)
        {
            stop_distance += CalcTwoPointDis(traj, i, i + 1);
        }

        stop_distance = -stop_distance;
    }

    else if ((nearestIndex < stop_idx_opt) && (nearestIndex >= 0))
    {
        for (uint32_t i = nearestIndex; i < stop_idx_opt - 1; i++)
        {
            stop_distance += CalcTwoPointDis(traj, i, i + 1);
        }
    }
    else
    {
        ;
    }

    double nearestDis = sqrt(
            pow((current_pose.position.x - traj.x.at(nearestIndex)), 2)
                    + pow((current_pose.position.y - traj.y.at(nearestIndex)),
                            2));

    stop_distance += nearestDis;

    return stop_distance;
}

uint32_t searchZeroVelocityIndex(MPCTrajectory traj)
{
    uint16_t trajSize = traj.vx.size();

    for (uint16_t i = 0; i < trajSize; i++)
    {
        if (std::abs(traj.vx.at(i)) < 1e-5)
        {
            return i;
        }
    }

    return trajSize;
}

double CalcTwoPointDis(const MPCTrajectory traj, uint32_t point1_index,
                       uint32_t point2_index)
{

    if (point1_index < traj.x.size() && point2_index < traj.x.size()
            && traj.x.size() > 0)
    {

        return sqrt(
                (traj.x.at(point2_index) - traj.x.at(point1_index))
                        * (traj.x.at(point2_index) - traj.x.at(point1_index))
                        + (traj.y.at(point2_index) - traj.y.at(point1_index))
                                * (traj.y.at(point2_index)
                                        - traj.y.at(point1_index)));
    }
    else
    {
        return 0;
    }
}

Adsfi::HafPose calcPoseAfterTimeDelay(const Adsfi::HafPose &current_pose, const double delay_time,
                            const double current_vel, const double current_acc)
{
    if (delay_time <= 0.0)
    {
        return current_pose;
    }

    const double time_to_stop = -current_vel / current_acc;

    const double delay_time_calculation =
            time_to_stop > 0.0 && time_to_stop < delay_time ?
                    time_to_stop : delay_time;

    const double yaw = commStruct::getYaw(current_pose.orientation);

    const double running_distance = delay_time_calculation * current_vel
            + 0.5 * current_acc * delay_time_calculation
                    * delay_time_calculation;

    const double dx = running_distance * std::cos(yaw);
    const double dy = running_distance * std::sin(yaw);

    auto pred_pose = current_pose;

    pred_pose.position.x += dx;
    pred_pose.position.y += dy;

    return pred_pose;
}

double lerp(const double v_from, const double v_to, const double ratio)
{
    return v_from + (v_to - v_from) * ratio;
}

// Quaternion lerpOrientation(const Adsfi::HafQuaternion &o_from, const Adsfi::HafQuaternion &o_to,
//                            const double ratio)
// {
//     tf2::Quaternion q_from, q_to;
//     tf2::fromMsg(o_from, q_from);
//     tf2::fromMsg(o_to, q_to);

//     const auto q_interpolated = q_from.slerp(q_to, ratio);

//     return tf2::toMsg(q_interpolated);
// }

double applyDiffLimitFilter(const double input_val, const double prev_val,
                            const double dt, const double max_val,
                            const double min_val)
{
    const double diff_raw = (input_val - prev_val) / dt;
    const double diff = std::min(std::max(diff_raw, min_val), max_val);
    const double filtered_val = prev_val + diff * dt;

    return filtered_val;
}

double applyDiffLimitFilter(const double input_val, const double prev_val,
                            const double dt, const double lim_val)
{
    const double max_val = std::fabs(lim_val);
    const double min_val = -max_val;

    return applyDiffLimitFilter(input_val, prev_val, dt, max_val, min_val);
}

}    // namespace longitudinal_utils
}    // namespace pid_longitudinal_controller
