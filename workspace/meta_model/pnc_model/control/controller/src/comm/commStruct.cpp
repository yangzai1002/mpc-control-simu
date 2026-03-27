#include "comm/comm_struct.hpp"

namespace commStruct
{

inline double normalizeRadian(const double rad, const double min_rad)
{
    const auto max_rad = min_rad + 2 * M_PI;

    const auto value = std::fmod(rad, 2 * M_PI);

    if (min_rad <= value && value < max_rad)
    {
        return value;
    }

    return value - std::copysign(2 * M_PI, value);
}

template<class Point1, class Point2>
double calcDistance2d(const Point1 &point1, const Point2 &point2)
{
    return std::hypot(point1.x - point2.x, point1.y - point2.y);
}

template<class Point1, class Point2>
double calcSquaredDistance2d(const Point1 &point1, const Point2 &point2)
{
    return std::pow(point1.x - point2.x, 2) + std::pow(point1.y - point2.y, 2);
}


double calcSquaredDistance2dHaf(const HafWayPoint &point1, const HafWayPoint &point2)
{
    return std::pow(point1.x - point2.x, 2) + std::pow(point1.y - point2.y, 2);
}

double calcDistance2dHaf(const HafWayPoint &point1, const HafWayPoint &point2)
{
    return std::hypot(point1.x - point2.x, point1.y - point2.y);
}

double calcDistance3d(const MPCTrajectory &trajectory,
                      const size_t idx1, const size_t idx2)
{
    const double dx = trajectory.x.at(idx1) - trajectory.x.at(idx2);
    const double dy = trajectory.y.at(idx1) - trajectory.y.at(idx2);
    const double dz = trajectory.z.at(idx1) - trajectory.z.at(idx2);
    return std::hypot(dx, dy, dz);
}

double calcCurvature(const Adsfi::Point3d &p1,
                     const Adsfi::Point3d &p2,
                     const Adsfi::Point3d &p3)
{
    const double denominator = calcDistance2d(p1, p2) * calcDistance2d(p2, p3)
            * calcDistance2d(p3, p1);

    if (std::fabs(denominator) < 1e-10)
    {
        throw std::runtime_error(
                "points are too close for curvature calculation.");
    }

    return 2.0 * ((p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x))
            / denominator;
}

Adsfi::HafQuaternion createQuaternionFromYaw(const double yaw)
{
    Adsfi::HafQuaternion q;
   
    double roll = 0.0,pitch = 0.0;
    double yaw_in = fmod(yaw, 2 * M_PI);

    // 计算半角的正弦和余弦
    double cr = cos(roll / 2.0);
    double sr = sin(roll / 2.0);
    double cp = cos(pitch / 2.0);
    double sp = sin(pitch / 2.0);
    double cy = cos(yaw_in / 2.0);
    double sy = sin(yaw_in / 2.0);

    // 代入公式计算四元数分量
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    q.w = cr * cp * cy + sr * sp * sy;

    return q;
}

double getYaw(const Adsfi::HafQuaternion& orientation) {

    double x = orientation.x;
    double y = orientation.y;
    double z = orientation.z;
    double w = orientation.w;

    // 核心公式计算偏航角
    double sin_yaw = 2.0 * (w * y - x * z);
    double cos_yaw = 1.0 - 2.0 * (y * y + z * z);
    
    // atan2 返回范围[-π, π]，符合航向角需求
    double yaw = std::atan2(sin_yaw, cos_yaw);

    return yaw;
}

// geometry_msgs::Vector3 createTranslation(const double x, const double y,
//                                          const double z)
// {
//     geometry_msgs::Vector3 v;
//     v.x = x;
//     v.y = y;
//     v.z = z;

//     return v;
// }

// Adsfi::HafPose calcOffsetPose(const Adsfi::HafPose &p, const double x,
//                                    const double y, const double z,
//                                    const double yaw)
// {
//     Adsfi::HafPose pose;
//     geometry_msgs::Transform transform;

//     transform.translation = createTranslation(x, y, z);
//     transform.rotation = createQuaternionFromYaw(yaw);

//     tf2::Transform tf_pose;
//     tf2::Transform tf_offset;
//     tf2::fromMsg(transform, tf_offset);
//     tf2::fromMsg(p, tf_pose);
//     tf2::toMsg(tf_pose * tf_offset, pose);

//     return pose;
// }

double calcYawDeviation(const Adsfi::HafPose &base_pose,
                        const Adsfi::HafPose &target_pose)
{

    const auto base_yaw = getYaw(base_pose.orientation);
    const auto target_yaw = getYaw(target_pose.orientation);

    return normalizeRadian(target_yaw - base_yaw);
}

bool calcMPCTrajectoryTime(MPCTrajectory &traj)
{
    constexpr auto min_dt = 1.0e-4;
    double t = 0.0;

    traj.relative_time.clear();
    traj.relative_time.push_back(t);

    for (size_t i = 0; i < traj.x.size() - 1; ++i)
    {
        const double dist = calcDistance3d(traj, i, i + 1);
        const double v = std::max(std::fabs(traj.vx.at(i)), 0.1);
        t += std::max(dist / v, min_dt);
        traj.relative_time.push_back(t);
    }

    return true;
}

MPCTrajectory convertToMPCTrajectory(
        const HafEgoTrajectory &input)
{

    MPCTrajectory output;

    for (const HafTrajectoryPoint &p : input.trajectoryPoints)
    {
        const double x = p.wayPoint.x;
        const double y = p.wayPoint.y;
        const double z = p.wayPoint.z;

        const double yaw = p.steerAngle;
        const double vx = p.speed;
        const double k = 0.0;
        const double t = 0.0;

        output.push_back(x, y, z, yaw, vx, k, k,t);
    }

    calcMPCTrajectoryTime(output);

    return output;
}

Trajectory convertToAutowareTrajectory(
        const MPCTrajectory &traj)
{

    Trajectory output;

    for (size_t i = 0; i < traj.size(); ++i)
    {
        TrajectoryPoint p;

        p.point.position.x = traj.x.at(i);
        p.point.position.y = traj.y.at(i);
        p.point.position.z = traj.z.at(i);

        p.point.orientation = commStruct::createQuaternionFromYaw(
                traj.yaw.at(i));

        p.speed = traj.vx.at(i);

        output.trajectory.push_back(p);

        if (output.trajectory.size() == output.trajectory.max_size())
        {
            break;
        }

    }

    return output;
}

size_t findNearestIndex(const Trajectory &points,
                        const Adsfi::HafPose &pose, const double max_dist,
                        const double max_yaw)
{
    const double max_squared_dist = max_dist * max_dist;

    double min_squared_dist = std::numeric_limits<double>::max();
    bool is_nearest_found = false;
    size_t min_idx = 0;

    for (size_t i = 0; i < points.trajectory.size(); ++i)
    {
        const auto squared_dist = calcSquaredDistance2d(
                points.trajectory.at(i).point.position, pose.position);

        if (squared_dist > max_squared_dist || squared_dist >= min_squared_dist)
        {
            continue;
        }

        const auto yaw = calcYawDeviation(points.trajectory.at(i).point, pose);

        if (std::fabs(yaw) > max_yaw)
        {
            continue;
        }

        min_squared_dist = squared_dist;
        min_idx = i;
        is_nearest_found = true;
    }

    if (is_nearest_found)
    {
        return min_idx;
    }

    return -1;

}

size_t findFirstNearestIndexWithSoftConstraints(const Trajectory &points,
                                                const Adsfi::HafPose &pose,
                                                const double dist_threshold,
                                                const double yaw_threshold)
{

    {
        const double squared_dist_threshold = dist_threshold * dist_threshold;
        double min_squared_dist = std::numeric_limits<double>::max();
        size_t min_idx = 0;

        bool is_within_constraints = false;

        for (size_t i = 0; i < points.trajectory.size(); ++i)
        {
            const auto squared_dist = calcSquaredDistance2d(
                    points.trajectory.at(i).point.position, pose.position);

            const auto yaw = calcYawDeviation(points.trajectory.at(i).point,
                    pose);

            if (squared_dist_threshold < squared_dist
                    || yaw_threshold < std::abs(yaw))
            {
                if (is_within_constraints)
                {
                    break;
                }

                continue;
            }

            if (min_squared_dist <= squared_dist)
            {
                continue;
            }

            min_squared_dist = squared_dist;
            min_idx = i;
            is_within_constraints = true;
        }

        if (is_within_constraints)
        {
            return min_idx;
        }
    }

    {
        const double squared_dist_threshold = dist_threshold * dist_threshold;
        double min_squared_dist = std::numeric_limits<double>::max();
        size_t min_idx = 0;
        bool is_within_constraints = false;

        for (size_t i = 0; i < points.trajectory.size(); ++i)
        {
            const auto squared_dist = calcSquaredDistance2d(
                    points.trajectory.at(i).point.position, pose.position);

            if (squared_dist_threshold < squared_dist)
            {
                if (is_within_constraints)
                {
                    break;
                }

                continue;
            }

            if (min_squared_dist <= squared_dist)
            {
                continue;
            }

            min_squared_dist = squared_dist;
            min_idx = i;
            is_within_constraints = true;
        }

        if (is_within_constraints)
        {
            return min_idx;
        }

    }

    return findNearestIndex(points, pose);
}

template<class Pose1, class Pose2>
bool isDrivingForward(const Pose1 &src_pose, const Pose2 &dst_pose)
{
    const double src_yaw = getYaw(src_pose.orientation);
    const double pose_direction_yaw = calcAzimuthAngle(src_pose.position,
            dst_pose.position);

    return std::fabs(normalizeRadian(src_yaw - pose_direction_yaw)) < M_PI / 2.0;
}

std::optional<bool> isDrivingForward(const commStruct::Trajectory &points)
{
    if (points.trajectory.size() < 2)
    {
        return std::nullopt;
    }

    const auto &first_pose = points.trajectory.at(0).point;
    const auto &second_pose = points.trajectory.at(1).point;

    return isDrivingForward(first_pose, second_pose);
}

double calcAzimuthAngle(const Adsfi::Point3d &p_from,
                        const Adsfi::Point3d &p_to)
{
    const double dx = p_to.x - p_from.x;
    const double dy = p_to.y - p_from.y;

    return std::atan2(dy, dx);
}

}
