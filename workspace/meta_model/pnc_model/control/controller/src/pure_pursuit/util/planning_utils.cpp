#include "pure_pursuit/util/planning_utils.hpp"
#include "comm/comm_struct.hpp"
#include "interpolation/linear_interpolation.hpp"
#include "interpolation/spline_interpolation.hpp"
#include "interpolation/zero_order_hold.hpp"
#include "amathutils_lib/amathutils.hpp"

namespace pure_pursuit
{

constexpr double close_s_threshold = 1e-6;

namespace planning_utils
{

double calcArcLengthFromWayPoint(const Trajectory &input_path,
        const size_t src_idx, const size_t dst_idx)
{
    double length = 0;

    for (size_t i = src_idx; i < dst_idx; ++i)
    {
        const double dx_wp = input_path.points.at(i + 1).pose.position.x
                - input_path.points.at(i).pose.position.x;

        const double dy_wp = input_path.points.at(i + 1).pose.position.y
                - input_path.points.at(i).pose.position.y;

        length += std::hypot(dx_wp, dy_wp);
    }

    return length;
}



double calcCurvature(const Adsfi::Point3d &target,
        const Adsfi::HafPose &current_pose)
{
    constexpr double KAPPA_MAX = 1e9;

    const double radius = calcRadius(target, current_pose);

    if (fabs(radius) > 0)
    {
        return 1 / radius;
    }
    else
    {
        return KAPPA_MAX;
    }
}



double calcDistance2D(const Adsfi::Point3d &p,
        const Adsfi::Point3d &q)
{
    const double dx = p.x - q.x;
    const double dy = p.y - q.y;
    return sqrt(dx * dx + dy * dy);
}



//
double calcDistSquared2D(const Adsfi::Point3d &p,
        const Adsfi::Point3d &q)
{
    const double dx = p.x - q.x;
    const double dy = p.y - q.y;
    return dx * dx + dy * dy;
}



/* a_vec = line_e - line_s, b_vec = point - line_s
 * a_vec x b_vec = |a_vec| * |b_vec| * sin(theta)
 *               = |a_vec| * lateral_error ( because, lateral_error = |b_vec| * sin(theta) )
 *
 * lateral_error = a_vec x b_vec / |a_vec|
 *        = (a_x * b_y - a_y * b_x) / |a_vec| */
// double calcLateralError2D(const Adsfi::Point3d &line_s,
//         const Adsfi::Point3d &line_e, const Adsfi::Point3d &point)
// {
//     tf2::Vector3 a_vec((line_e.x - line_s.x), (line_e.y - line_s.y), 0.0);
//     tf2::Vector3 b_vec((point.x - line_s.x), (point.y - line_s.y), 0.0);

//     double lat_err =
//             (a_vec.length() > 0) ?
//                     a_vec.cross(b_vec).z() / a_vec.length() : 0.0;
//     return lat_err;
// }

double calcLateralError2D(const Adsfi::Point3d &line_s,
                          const Adsfi::Point3d &line_e, 
                          const Adsfi::Point3d &point)
{
    double ax = line_e.x - line_s.x;
    double ay = line_e.y - line_s.y;
    double bx = point.x - line_s.x;
    double by = point.y - line_s.y;

    // 二维向量叉乘的z分量 = ax*by - ay*bx （数学等价，省去Vector3d构造开销）
    double cross_z = ax * by - ay * bx;
    double len = sqrt(ax*ax + ay*ay);

    return len > 0 ? cross_z / len : 0.0;
}



//计算半径
double calcRadius(const Adsfi::Point3d &target,
        const Adsfi::HafPose &current_pose)
{
    constexpr double RADIUS_MAX = 1e9;

    const double denominator = 2
            * transformToRelativeCoordinate2D(target, current_pose).y;

    const double numerator = calcDistSquared2D(target, current_pose.position);

    if (fabs(denominator) > 0)
    {
        return numerator / denominator;
    }
    else
    {
        return RADIUS_MAX;
    }
}


double convertCurvatureToSteeringAngle(double wheel_base, double kappa)
{
    return atan(wheel_base * kappa);
}


std::vector<Adsfi::HafPose> extractPoses(const Trajectory &trajectory)
{
    std::vector<Adsfi::HafPose> poses;

    for (const auto &p : trajectory.points)
    {
        poses.push_back(p.pose);
    }

    return poses;
}



std::pair<bool, int32_t> findClosestIdxWithDistAngThr(
        const std::vector<Adsfi::HafPose> &poses,  //
        const Adsfi::HafPose &current_pose,  //
        double th_dist,  // 2.0
        double th_yaw)   // 0.785398
{

    double dist_squared_min = std::numeric_limits<double>::max();

    int32_t idx_min = -1;

    for (size_t i = 0; i < poses.size(); ++i)
    {
        //
        const double ds = calcDistSquared2D(poses.at(i).position,
                current_pose.position);

        if (ds > th_dist * th_dist)
        {
            continue;
        }

        const double yaw_pose = MPCUtils::getYaw(current_pose.orientation);
        const double yaw_ps = MPCUtils::getYaw(poses.at(i).orientation);
        const double yaw_diff = normalizeEulerAngle(yaw_pose - yaw_ps);

        if (fabs(yaw_diff) > th_yaw)
        {
            continue;
        }

        if (ds < dist_squared_min)
        {
            dist_squared_min = ds;
            idx_min = i;
        }
    }

    return (idx_min >= 0) ?
            std::make_pair(true, idx_min) : std::make_pair(false, idx_min);
}


int8_t getLaneDirection(const std::vector<Adsfi::HafPose> &poses,
        double th_dist)
{
    if (poses.size() < 2)
    {
        printf("size of waypoints is smaller than 2\n");
        return 2;
    }

    for (uint32_t i = 0; i < poses.size(); i++)
    {
        Adsfi::HafPose prev;
        Adsfi::HafPose next;

        if (i == (poses.size() - 1))
        {
            prev = poses.at(i - 1);
            next = poses.at(i);
        }
        else
        {
            prev = poses.at(i);
            next = poses.at(i + 1);
        }

        if (planning_utils::calcDistSquared2D(prev.position, next.position)
                > th_dist * th_dist)
        {
            const auto rel_p = transformToRelativeCoordinate2D(next.position,
                    prev);

            return (rel_p.x > 0.0) ? 0 : 1;
        }
    }

    std::cout << " Lane is something wrong" << std::endl;

    return 2;
}


bool isDirectionForward(const Adsfi::HafPose &prev,
        const Adsfi::HafPose &next)
{
    return (transformToRelativeCoordinate2D(next.position, prev).x > 0.0) ?
            true : false;
}



bool isDirectionForward(const std::vector<Adsfi::HafPose> &poses)
{
    Adsfi::Point3d rel_p = transformToRelativeCoordinate2D(
            poses.at(2).position, poses.at(1));

    bool is_forward = (rel_p.x > 0.0) ? true : false;

    return is_forward;
}



//
// template<>
// bool isInPolygon(const std::vector<Adsfi::Point3d> &polygon,
//         const Adsfi::Point3d &point)
// {
//     std::vector<tf2::Vector3> polygon_conv;

//     for (const auto &el : polygon)
//     {
//         polygon_conv.emplace_back(el.x, el.y, el.z);
//     }

//     tf2::Vector3 point_conv = tf2::Vector3(point.x, point.y, point.z);

//     return isInPolygon<tf2::Vector3>(polygon_conv, point_conv);
// }



// km/h 转 m/s
double kmph2mps(const double velocity_kmph)
{
    return velocity_kmph / 3.6;
}

// m/s 转 km/h
double mps2kmph(const double velocity_mps)
{
    return velocity_mps * 3.6;
}


double normalizeEulerAngle(const double euler)
{
    double res = euler;

    while (res > M_PI)
    {
        res -= (2 * M_PI);
    }

    while (res < -M_PI)
    {
        res += 2 * M_PI;
    }

    return res;
}




// 绝对坐标
Adsfi::Point3d transformToAbsoluteCoordinate2D(
        const Adsfi::Point3d &point, const Adsfi::HafPose &origin)
{
    // 旋转
    Adsfi::Point3d rot_p;
    double yaw = MPCUtils::getYaw(origin.orientation);
    rot_p.x = (cos(yaw) * point.x) + ((-1) * sin(yaw) * point.y);
    rot_p.y = (sin(yaw) * point.x) + (cos(yaw) * point.y);

    // 平移
    Adsfi::Point3d res;
    res.x = rot_p.x + origin.position.x;
    res.y = rot_p.y + origin.position.y;
    res.z = origin.position.z;

    return res;
}


// 相对坐标
Adsfi::Point3d transformToRelativeCoordinate2D(
        const Adsfi::Point3d &point, const Adsfi::HafPose &origin)
{
    // 平移
    Adsfi::Point3d trans_p;
    trans_p.x = point.x - origin.position.x;
    trans_p.y = point.y - origin.position.y;

    // 旋转
    double yaw = MPCUtils::getYaw(origin.orientation);

    Adsfi::Point3d res;
    res.x = (cos(yaw) * trans_p.x) + (sin(yaw) * trans_p.y);
    res.y = ((-1) * sin(yaw) * trans_p.x) + (cos(yaw) * trans_p.y);
    res.z = origin.position.z;

    return res;
}



//利用yaw计算四元数
Adsfi::HafQuaternion getQuaternionFromYaw(const double _yaw)
{
    // tf2::Quaternion q;
    // q.setRPY(0, 0, _yaw);

    // return tf2::toMsg(q);

    return amathutils::getQuaternionFromYaw(_yaw);
}



//
std::vector<TrajectoryPoint> convertToTrajectoryPointArray(
        const Trajectory &trajectory)
{
    std::vector<TrajectoryPoint> output(trajectory.points.size());

    std::copy(trajectory.points.begin(), trajectory.points.end(),
            output.begin());

    return output;
}



//
bool validate_points_duplication(const std::vector<TrajectoryPoint> &points)
{
    for (size_t i = 0; i < points.size() - 1; ++i)
    {
        const auto &curr_pt = points.at(i);

        const auto &next_pt = points.at(i + 1);

        const double ds = calcDistance2D(curr_pt.pose.position,
                next_pt.pose.position);

        if (ds < close_s_threshold)
        {
            return false;
        }
    }

    return true;
}



//
bool validate_arguments(const std::vector<TrajectoryPoint> &input_points,
        const std::vector<double> &resampling_intervals)
{

    if (!(input_points.size() >= 2) && (resampling_intervals.size() >= 2))
    {
        printf("invalid argument: The number of input points is less than 2");

        return false;
    }


    if (!validate_resampling_range(input_points, resampling_intervals))
    {
        printf(
                "invalid argument: resampling interval is longer than input points");

        return false;
    }


    if (!validate_points_duplication(input_points))
    {
        printf("invalid argument: input points has some duplicated points");
        return false;
    }

    return true;
}



Trajectory resampleTrajectory(const Trajectory &input_trajectory,
        const std::vector<double> &resampled_arclength,
        const bool use_akima_spline_for_xy, const bool use_lerp_for_z,
        const bool use_zero_order_hold_for_twist)
{
    if (!validate_arguments(input_trajectory.points, resampled_arclength))
    {
        return input_trajectory;
    }

    std::vector<double> input_arclength;
    std::vector<Adsfi::HafPose> input_pose;
    std::vector<double> v_lon;
    std::vector<double> v_lat;
    std::vector<double> heading_rate;
    std::vector<double> acceleration;
    std::vector<double> front_wheel_angle;
    std::vector<double> rear_wheel_angle;
    std::vector<double> time_from_start;

    input_arclength.reserve(input_trajectory.points.size());
    input_pose.reserve(input_trajectory.points.size());
    v_lon.reserve(input_trajectory.points.size());
    v_lat.reserve(input_trajectory.points.size());
    heading_rate.reserve(input_trajectory.points.size());
    acceleration.reserve(input_trajectory.points.size());
    front_wheel_angle.reserve(input_trajectory.points.size());
    rear_wheel_angle.reserve(input_trajectory.points.size());
    time_from_start.reserve(input_trajectory.points.size());

    input_arclength.push_back(0.0);
    input_pose.push_back(input_trajectory.points.front().pose);
    v_lon.push_back(input_trajectory.points.front().longitudinal_velocity_mps);
    v_lat.push_back(input_trajectory.points.front().lateral_velocity_mps);
    heading_rate.push_back(input_trajectory.points.front().heading_rate_rps);
    acceleration.push_back(input_trajectory.points.front().acceleration_mps2);

    front_wheel_angle.push_back(
            input_trajectory.points.front().front_wheel_angle_rad);

    rear_wheel_angle.push_back(
            input_trajectory.points.front().rear_wheel_angle_rad);

    time_from_start.push_back(input_trajectory.points.front().time_from_start);

    for (size_t i = 1; i < input_trajectory.points.size(); ++i)
    {
        const auto &prev_pt = input_trajectory.points.at(i - 1);
        const auto &curr_pt = input_trajectory.points.at(i);

        const double ds = calcDistance2D(prev_pt.pose.position,
                curr_pt.pose.position);

        input_arclength.push_back(ds + input_arclength.back());
        input_pose.push_back(curr_pt.pose);

        v_lon.push_back(curr_pt.longitudinal_velocity_mps);
        v_lat.push_back(curr_pt.lateral_velocity_mps);

        heading_rate.push_back(curr_pt.heading_rate_rps);

        acceleration.push_back(curr_pt.acceleration_mps2);

        front_wheel_angle.push_back(curr_pt.front_wheel_angle_rad);

        rear_wheel_angle.push_back(curr_pt.rear_wheel_angle_rad);

        time_from_start.push_back(curr_pt.time_from_start);
    }

    // Interpolate
    const auto lerp = [&](const auto &input)
    {
        return interpolation::lerp(input_arclength, input, resampled_arclength);
    };

    std::vector<size_t> closest_segment_indices;

    if (use_zero_order_hold_for_twist)
    {
        closest_segment_indices = interpolation::calc_closest_segment_indices(
                input_arclength, resampled_arclength);
    }

    const auto zoh =
            [&](
                    const auto &input)
                    {
                        return interpolation::zero_order_hold(input_arclength, input, closest_segment_indices);
                    };

    // 重新采样位姿向量
    const auto interpolated_pose = resamplePoseVector(input_pose,
            resampled_arclength, use_akima_spline_for_xy, use_lerp_for_z);

    const auto interpolated_v_lon =
            use_zero_order_hold_for_twist ? zoh(v_lon) : lerp(v_lon);

    const auto interpolated_v_lat =
            use_zero_order_hold_for_twist ? zoh(v_lat) : lerp(v_lat);

    const auto interpolated_heading_rate = lerp(heading_rate);

    const auto interpolated_acceleration =
            use_zero_order_hold_for_twist ?
                    zoh(acceleration) : lerp(acceleration);

    const auto interpolated_front_wheel_angle = lerp(front_wheel_angle);
    const auto interpolated_rear_wheel_angle = lerp(rear_wheel_angle);
    const auto interpolated_time_from_start = lerp(time_from_start);

    if (interpolated_pose.size() != resampled_arclength.size())
    {
        std::cerr
                << "[motion_utils]: Resampled pose size is different from resampled arclength"
                << std::endl;

        return input_trajectory;
    }

    Trajectory resampled_trajectory;
    resampled_trajectory.header = input_trajectory.header;
    resampled_trajectory.points.resize(interpolated_pose.size());

    for (size_t i = 0; i < resampled_trajectory.points.size(); ++i)
    {
        TrajectoryPoint traj_point;
        traj_point.pose = interpolated_pose.at(i);
        traj_point.longitudinal_velocity_mps = interpolated_v_lon.at(i);
        traj_point.lateral_velocity_mps = interpolated_v_lat.at(i);
        traj_point.heading_rate_rps = interpolated_heading_rate.at(i);
        traj_point.acceleration_mps2 = interpolated_acceleration.at(i);
        traj_point.front_wheel_angle_rad = interpolated_front_wheel_angle.at(i);
        traj_point.rear_wheel_angle_rad = interpolated_rear_wheel_angle.at(i);
        traj_point.time_from_start = interpolated_time_from_start.at(i);

        resampled_trajectory.points.at(i) = traj_point;
    }

    return resampled_trajectory;
}



//
Trajectory convertToTrajectory(const std::vector<TrajectoryPoint> &trajectory,
        const Adsfi::HafHeader& header)
{
    Trajectory output
    { };

    output.header = header;

    for (const auto &pt : trajectory)
        output.points.push_back(pt);

    return output;
}



//
void validateNonEmpty(const std::vector<TrajectoryPoint> &points)
{
    if (points.empty())
    {
        throw std::invalid_argument(
                "[autoware_motion_utils] validateNonEmpty(): Points is empty.");
    }
}


std::vector<TrajectoryPoint> removeOverlapPoints(
        const std::vector<TrajectoryPoint> &points, const size_t start_idx)
{
    if (points.size() < start_idx + 1)
    {
        return points;
    }

    std::vector<TrajectoryPoint> dst;
    dst.reserve(points.size());

    for (size_t i = 0; i <= start_idx; ++i)
    {
        dst.push_back(points.at(i));
    }

    constexpr double eps = 1.0E-08;

    for (size_t i = start_idx + 1; i < points.size(); ++i)
    {
        const auto prev_p = dst.back();
        const auto curr_p = points.at(i);

        if (std::abs(prev_p.pose.position.x - curr_p.pose.position.x) < eps
                && std::abs(prev_p.pose.position.y - curr_p.pose.position.y)
                        < eps)
        {
            continue;
        }

        dst.push_back(points.at(i));
    }

    return dst;
}



//
size_t findNearestIndexPoint(const std::vector<TrajectoryPoint> &points,
        const Adsfi::Point3d &point)
{
    validateNonEmpty(points);

    double min_dist = std::numeric_limits<double>::max();
    size_t min_idx = 0;

    for (size_t i = 0; i < points.size(); ++i)
    {
        const auto dist = calcDistSquared2D(
                points.at(i).pose.position, point);
        if (dist < min_dist)
        {
            min_dist = dist;
            min_idx = i;
        }
    }
    return min_idx;
}



//
std::optional<size_t> findNearestIndex(
        const std::vector<TrajectoryPoint> &points,
        const Adsfi::HafPose &pose,
        const double max_dist,
        const double max_yaw)
{

    try
    {
        validateNonEmpty(points);
    } catch (const std::exception &e)
    {
        printf("%s", e.what());
        return std::nullopt;
    }

    const double max_squared_dist = max_dist * max_dist;

    double min_squared_dist = std::numeric_limits<double>::max();
    bool is_nearest_found = false;
    size_t min_idx = 0;

    for (size_t i = 0; i < points.size(); ++i)
    {
        const auto squared_dist = calcDistSquared2D(
                points.at(i).pose.position, pose.position);

        if (squared_dist > max_squared_dist || squared_dist >= min_squared_dist)
        {
            continue;
        }

        const auto yaw = commStruct::calcYawDeviation(points.at(i).pose, pose);

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

    return std::nullopt;
}

//
size_t findNearestSegmentIndex(const std::vector<TrajectoryPoint> &points,
        const Adsfi::Point3d &point)
{
    const size_t nearest_idx = findNearestIndexPoint(points, point);

    if (nearest_idx == 0)
    {
        return 0;
    }

    if (nearest_idx == points.size() - 1)
    {
        return points.size() - 2;
    }

    const double signed_length = calcLongitudinalOffsetToSegment(points,
            nearest_idx, point);

    if (signed_length <= 0)
    {
        return nearest_idx - 1;
    }

    return nearest_idx;
}

//
double calcLongitudinalOffsetToSegment(
        const std::vector<TrajectoryPoint> &points, const size_t seg_idx,
        const Adsfi::Point3d &p_target, const bool throw_exception)
{
    if (seg_idx >= points.size() - 1)
    {
        const std::string error_message(
                "[autoware_motion_utils] " + std::string(__func__)
                        + ": Failed to calculate longitudinal offset because the given segment index is out of the "
                                "points size.");

        if (throw_exception)
        {
            throw std::out_of_range(error_message);
        }

        printf(
                "%s Return NaN since no_throw option is enabled. The maintainer must check the code.",
                error_message.c_str());

        return std::nan("");
    }

    const auto overlap_removed_points = removeOverlapPoints(points, seg_idx);

    if (throw_exception)
    {
        validateNonEmpty(overlap_removed_points);
    }
    else
    {
        try
        {
            validateNonEmpty(overlap_removed_points);
        } catch (const std::exception &e)
        {
            printf("%s", e.what());
            return std::nan("");
        }
    }

    if (seg_idx >= overlap_removed_points.size() - 1)
    {
        const std::string error_message(
                "[autoware_motion_utils] " + std::string(__func__)
                        + ": Longitudinal offset calculation is not supported for the same points.");

        if (throw_exception)
        {
            throw std::runtime_error(error_message);
        }

        printf(
                "%s Return NaN since no_throw option is enabled. The maintainer must check the code.",
                error_message.c_str());

        return std::nan("");
    }

    const auto p_front = overlap_removed_points.at(seg_idx).pose.position;
    const auto p_back = overlap_removed_points.at(seg_idx + 1).pose.position;

    const Eigen::Vector3d segment_vec
    { p_back.x - p_front.x, p_back.y - p_front.y, 0 };

    const Eigen::Vector3d target_vec
    { p_target.x - p_front.x, p_target.y - p_front.y, 0 };

    return segment_vec.dot(target_vec) / segment_vec.norm();
}



//
double calcSignedArcLength(const std::vector<TrajectoryPoint> &points,
        const size_t src_idx, const size_t dst_idx)
{
    try
    {
        validateNonEmpty(points);
    } catch (const std::exception &e)
    {
        printf("%s", e.what());
        return 0.0;
    }

    if (src_idx > dst_idx)
    {
        return -calcSignedArcLength(points, dst_idx, src_idx);
    }

    double dist_sum = 0.0;
    for (size_t i = src_idx; i < dst_idx; ++i)
    {
        dist_sum += calcDistance2D(points.at(i).pose.position,
                points.at(i + 1).pose.position);
    }
    return dist_sum;
}



//
double calcArcLength(const std::vector<TrajectoryPoint> &points)
{
    try
    {
        validateNonEmpty(points);
    } catch (const std::exception &e)
    {
        printf("%s", e.what());
        return 0.0;
    }

    return calcSignedArcLength(points, 0, points.size() - 1);
}



//
double calcLateralOffset(const std::vector<TrajectoryPoint> &points,
        const Adsfi::Point3d &p_target, const bool throw_exception)
{
    const auto overlap_removed_points = removeOverlapPoints(points, 0);

    if (throw_exception)
    {
        validateNonEmpty(overlap_removed_points);
    }
    else
    {
        try
        {
            validateNonEmpty(overlap_removed_points);
        } catch (const std::exception &e)
        {
            printf(
                    "%s Return NaN since no_throw option is enabled. The maintainer must check the code.",
                    e.what());
            return std::nan("");
        }
    }

    if (overlap_removed_points.size() == 1)
    {
        const std::string error_message(
                "[motion_utils] " + std::string(__func__)
                        + ": Lateral offset calculation is not supported for the same points.");

        if (throw_exception)
        {
            throw std::runtime_error(error_message);
        }

        printf(
                "%s Return NaN since no_throw option is enabled. The maintainer must check the code.",
                error_message.c_str());

        return std::nan("");

    }

    const size_t seg_idx = findNearestSegmentIndex(overlap_removed_points,
            p_target);
    const auto p_indices = overlap_removed_points.size() - 2;
    const auto p_front_idx = (p_indices > seg_idx) ? seg_idx : p_indices;
    const auto p_back_idx = p_front_idx + 1;

    const auto p_front = overlap_removed_points.at(p_front_idx).pose.position;
    const auto p_back = overlap_removed_points.at(p_back_idx).pose.position;

    const Eigen::Vector3d segment_vec
    { p_back.x - p_front.x, p_back.y - p_front.y, 0.0 };
    const Eigen::Vector3d target_vec
    { p_target.x - p_front.x, p_target.y - p_front.y, 0.0 };

    const Eigen::Vector3d cross_vec = segment_vec.cross(target_vec);
    return cross_vec(2) / segment_vec.norm();
}


bool validate_resampling_range(const std::vector<TrajectoryPoint> &points,
        const std::vector<double> &resampling_intervals)
{
    const double points_length = calcArcLength(points);

    return points_length >= resampling_intervals.back();
}



std::vector<Adsfi::Point3d> resamplePointVector(
        const std::vector<Adsfi::Point3d> &points,
        const std::vector<double> &resampled_arclength,
        const bool use_akima_spline_for_xy, const bool use_lerp_for_z)
{

    std::vector<double> input_arclength;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;

    input_arclength.reserve(points.size());

    x.reserve(points.size());
    y.reserve(points.size());
    z.reserve(points.size());

    input_arclength.push_back(0.0);

    x.push_back(points.front().x);
    y.push_back(points.front().y);
    z.push_back(points.front().z);

    for (size_t i = 1; i < points.size(); ++i)
    {
        const auto &prev_pt = points.at(i - 1);
        const auto &curr_pt = points.at(i);
        const double ds = calcDistance2D(prev_pt, curr_pt);

        input_arclength.push_back(ds + input_arclength.back());

        x.push_back(curr_pt.x);
        y.push_back(curr_pt.y);
        z.push_back(curr_pt.z);
    }

    // Interpolate
    const auto lerp = [&](const auto &input)
    {
        return interpolation::lerp(input_arclength, input, resampled_arclength);
    };

    const auto spline =
            [&](
                    const auto &input)
                    {
                        return interpolation::spline(input_arclength, input, resampled_arclength);
                    };

    const auto spline_by_akima =
            [&](
                    const auto &input)
                    {
                        return interpolation::splineByAkima(input_arclength, input, resampled_arclength);
                    };

    const auto interpolated_x =
            use_akima_spline_for_xy ? lerp(x) : spline_by_akima(x);

    const auto interpolated_y =
            use_akima_spline_for_xy ? lerp(y) : spline_by_akima(y);

    const auto interpolated_z = use_lerp_for_z ? lerp(z) : spline(z);

    std::vector<Adsfi::Point3d> resampled_points;

    resampled_points.resize(interpolated_x.size());

    // Insert Position
    for (size_t i = 0; i < resampled_points.size(); ++i)
    {
        Adsfi::Point3d point;
        point.x = interpolated_x.at(i);
        point.y = interpolated_y.at(i);
        point.z = interpolated_z.at(i);
        resampled_points.at(i) = point;
    }

    return resampled_points;
}



std::vector<Adsfi::HafPose> resamplePoseVector(
        const std::vector<Adsfi::HafPose> &points_raw,
        const std::vector<double> &resampled_arclength,
        const bool use_akima_spline_for_xy, const bool use_lerp_for_z)
{
    const auto points = points_raw;


    std::vector<Adsfi::Point3d> position(points.size());

    for (size_t i = 0; i < points.size(); ++i)
    {
        position.at(i) = points.at(i).position;
    }

    //
    const auto resampled_position = resamplePointVector(position,
            resampled_arclength, use_akima_spline_for_xy, use_lerp_for_z);

    //
    std::vector<Adsfi::HafPose> resampled_points(
            resampled_position.size());

    // Insert Position
    for (size_t i = 0; i < resampled_position.size(); ++i)
    {
        Adsfi::HafPose pose;
        pose.position.x = resampled_position.at(i).x;
        pose.position.y = resampled_position.at(i).y;
        pose.position.z = resampled_position.at(i).z;

        resampled_points.at(i) = pose;
    }

    bool is_driving_forward = isDirectionForward(points.at(0), points.at(1));

    //
    insertOrientation(resampled_points, is_driving_forward);

    if (!is_driving_forward && resampled_arclength.front() < 1e-3)
    {
        resampled_points.at(0).orientation = points.at(0).orientation;
    }

    return resampled_points;
}



double calcElevationAngle(const Adsfi::Point3d &p_from,
        const Adsfi::Point3d &p_to)
{
    const double dz = p_to.z - p_from.z;
    const double dist_2d = calcDistance2D(p_from, p_to);

    return std::atan2(dz, dist_2d);
}



double calcAzimuthAngle(const Adsfi::Point3d &p_from,
        const Adsfi::Point3d &p_to)
{
    const double dx = p_to.x - p_from.x;
    const double dy = p_to.y - p_from.y;

    return std::atan2(dy, dx);
}



void setOrientation(const Adsfi::HafQuaternion &orientation,
        Adsfi::HafPose &p)
{
    p.orientation = orientation;
}



void insertOrientation(std::vector<Adsfi::HafPose> &points,
        const bool is_driving_forward)
{
    {
        for (size_t i = 0; i < points.size() - 1; ++i)
        {
            const auto &src_point = points.at(i);
            const auto &dst_point = points.at(i + 1);

            const double pitch = calcElevationAngle(src_point.position,
                    dst_point.position);

            const double yaw = calcAzimuthAngle(src_point.position,
                    dst_point.position);

            setOrientation(commStruct::createQuaternionFromYaw(yaw),
                    points.at(i));

            if (i == points.size() - 2)
            {
                setOrientation(points.at(i).orientation, points.at(i + 1));
            }
        }
    }

}


//
mdc::visual::Times getCurrentTimeVis()
{
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();

    uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % (uint32_t(1e9));

    return mdc::visual::Times { sec, nsec };
}



}  // namespace planning_utils
}  // namespace pure_pursuit
