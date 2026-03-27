#include "pure_pursuit/pure_pursuit_lateral_controller.hpp"

namespace
{

//
enum TYPE
{
    VEL_LD = 0,
    CURVATURE_LD = 1,
    LATERAL_ERROR_LD = 2,
    TOTAL_LD = 3,
    CURVATURE = 4,
    LATERAL_ERROR = 5,
    VELOCITY = 6,
    SIZE
};

} // namespace



namespace pure_pursuit
{

PurePursuitLateralController::PurePursuitLateralController(
        std::string params_path)
{
    pure_pursuit_ = std::make_unique<PurePursuit>();

    std::string config_path = params_path;
    YAML::Node config;

    try
    {
        config = YAML::LoadFile(config_path);
    }
    catch (...)
    {
        HAF_LOG_INFO << "The format of config file " << config_path
                << " is wrong. Please check (e.g. indentation).";
    }

    //
    YAML::Node subconfig = yamlSubNodeAbort(config, "parameters");

    param_.wheel_base = subconfig["wheel_base"].as<double>();
    param_.max_tire_steering_angle = subconfig["max_tire_steering_angle"].as<
            double>();

    param_.ld_velocity_ratio = subconfig["ld_velocity_ratio"].as<double>();
    param_.ld_lateral_error_ratio = subconfig["ld_lateral_error_ratio"].as<
            double>();
    param_.ld_curvature_ratio = subconfig["ld_curvature_ratio"].as<double>();
    param_.long_ld_lateral_error_threshold =
            subconfig["long_ld_lateral_error_threshold"].as<double>();

    param_.min_lookahead_distance = subconfig["min_lookahead_distance"].as<
            double>();
    param_.max_lookahead_distance = subconfig["max_lookahead_distance"].as<
            double>();

    param_.reverse_min_lookahead_distance =
            subconfig["reverse_min_lookahead_distance"].as<double>();
    param_.converged_steer_rad_ = subconfig["converged_steer_rad"].as<double>();

    param_.prediction_ds = subconfig["prediction_ds"].as<double>();
    param_.prediction_distance_length =
            subconfig["prediction_distance_length"].as<double>();

    param_.resampling_ds = subconfig["resampling_ds"].as<double>();

    param_.curvature_calculation_distance =
            subconfig["curvature_calculation_distance"].as<double>();

    param_.enable_path_smoothing =
            subconfig["enable_path_smoothing"].as<bool>();

    param_.path_filter_moving_ave_num =
            subconfig["path_filter_moving_ave_num"].as<int>();

}

// 计算预瞄距离
double PurePursuitLateralController::calcLookaheadDistance(
        const double lateral_error, const double curvature,
        const double velocity, const double min_ld, const bool is_control_cmd)
{

    const double vel_ld = abs(param_.ld_velocity_ratio * velocity);    //k1*v

    const double curvature_ld = -abs(param_.ld_curvature_ratio * curvature);    //-k2*kappa

    double lateral_error_ld = 0.0;

    if (abs(lateral_error) >= param_.long_ld_lateral_error_threshold)
    {
        lateral_error_ld = abs(param_.ld_lateral_error_ratio * lateral_error);    //k3*err_y
    }

    // 整体预瞄距离, 限幅
    const double total_ld = std::clamp(vel_ld + curvature_ld + lateral_error_ld,
            min_ld, param_.max_lookahead_distance);

    return total_ld;
}



// TrajectoryPoint PurePursuitLateralController::calcNextPose(
//         const double ds, TrajectoryPoint &point,
//         commStruct::AckermannLateralCommand cmd) const
// {
//     geometry_msgs::Transform transform;
//     geometry_msgs::Vector3 v;

//     v.x = ds;
//     v.y = 0.0;
//     v.z = 0.0;

//     transform.translation = v;

//     transform.rotation = planning_utils::getQuaternionFromYaw(
//             ((tan(cmd.steering_tire_angle) * ds) / param_.wheel_base));

//     TrajectoryPoint output_p;
//     tf2::Transform tf_pose;
//     tf2::Transform tf_offset;
//     tf2::fromMsg(transform, tf_offset);
//     tf2::fromMsg(point.pose, tf_pose);
//     tf2::toMsg(tf_pose * tf_offset, output_p.pose);

//     return output_p;
// }

/**
 * @brief Adsfi框架适配版 - 纯追踪算法 推算下一时刻的车辆位姿
 * @param ds 前进的距离 (单位: m)
 * @param point 当前轨迹点(位姿)
 * @param cmd 阿克曼横向控制指令(包含期望的前轮转角)
 * @return Adsfi框架的轨迹点 - 推算后的下一时刻位姿
 */
TrajectoryPoint PurePursuitLateralController::calcNextPose(
        const double ds, TrajectoryPoint &point,
        commStruct::AckermannLateralCommand cmd) const
{
    // 1. 计算 前进ds距离+阿克曼转向 带来的【偏航角增量】，核心公式与原代码一致
    const double yaw_increment = (std::tan(cmd.steering_tire_angle) * ds) / param_.wheel_base;

    // 2. 获取当前轨迹点的 原始位姿 & 当前偏航角
    Adsfi::HafPose curr_pose = point.pose;
    const double curr_yaw = MPCUtils::getYaw(curr_pose.orientation); // 和你MPC代码复用同一个工具函数
    const double new_yaw = amathutils::normalizeRadian(curr_yaw + yaw_increment); // 归一化航向角[-π, π]

    // 3. 计算 平移后的新坐标 (沿当前航向角前进ds距离)
    const double new_x = curr_pose.position.x + ds * std::cos(curr_yaw);
    const double new_y = curr_pose.position.y + ds * std::sin(curr_yaw);
    const double new_z = curr_pose.position.z; // Z轴高度不变，矿车是平面运动

    // 4. 构造新的位姿数据 - Adsfi框架标准结构
    Adsfi::HafPose new_pose;
    new_pose.position.x = new_x;
    new_pose.position.y = new_y;
    new_pose.position.z = new_z;
    // 将新的偏航角 转为 四元数 (复用框架工具函数，和你MPC代码一致)
    new_pose.orientation = MPCUtils::eulerToQuaternion(0,0,new_yaw);

    // 5. 构造返回的轨迹点 - Adsfi框架标准结构
    TrajectoryPoint output_p;
    output_p.pose = new_pose;
    // 拷贝原轨迹点的其他不变属性(速度/曲率等)，保证轨迹点完整性
//     output_p.twist = point.twist;
//     output_p.kappa = point.kappa;
//     output_p.relative_time = point.relative_time;

    return output_p;
}


void PurePursuitLateralController::setResampledTrajectory()
{
    std::vector<double> out_arclength;

    const auto input_tp_array = planning_utils::convertToTrajectoryPointArray(
            trajectory_);

    const auto traj_length = planning_utils::calcArcLength(input_tp_array);

    for (double s = 0; s < traj_length; s += param_.resampling_ds)
    {
        out_arclength.push_back(s);
    }

    trajectory_resampled_ = planning_utils::resampleTrajectory(
            planning_utils::convertToTrajectory(input_tp_array), out_arclength);

    trajectory_resampled_.points.back() = trajectory_.points.back();

    trajectory_resampled_.header = trajectory_.header;

    output_tp_array_ = planning_utils::convertToTrajectoryPointArray(
            trajectory_resampled_);

    HAF_LOG_INFO << " ## input traj_length: " << traj_length
            << ", out_arclength: " << out_arclength.size()
            << ", haf_trajectory.gear: " << int(haf_trajectory.gear)
            << ", output_tp_array_.size(): " << output_tp_array_.size();

}


double PurePursuitLateralController::calcCurvature(const size_t closest_idx)
{

    const size_t idx_dist = static_cast<size_t>(std::max(
            static_cast<int>((param_.curvature_calculation_distance)
                    / param_.resampling_ds), 1));

    size_t next_idx = trajectory_resampled_.points.size() - 1;
    size_t prev_idx = 0;

    if (static_cast<size_t>(closest_idx) >= idx_dist)
    {
        prev_idx = closest_idx - idx_dist;
    }
    else
    {
        return 0.0;
    }

    //
    if (trajectory_resampled_.points.size() - 1 >= closest_idx + idx_dist)
    {
        next_idx = closest_idx + idx_dist;
    }
    else
    {
        return 0.0;
    }


    double current_curvature = 0.0;

    try
    {
        current_curvature = commStruct::calcCurvature(
                trajectory_resampled_.points.at(prev_idx).pose.position,
                trajectory_resampled_.points.at(closest_idx).pose.position,
                trajectory_resampled_.points.at(next_idx).pose.position);

    }
    catch (std::exception const &e)
    {
        HAF_LOG_INFO << "pure_pursuit:" << e.what();
        current_curvature = 0.0;
    }

    return current_curvature;
}


void PurePursuitLateralController::averageFilterTrajectory(Trajectory &u)
{
    if (static_cast<int>(u.points.size())
            <= 2 * param_.path_filter_moving_ave_num)
    {
        HAF_LOG_INFO << "Cannot smooth path! Trajectory size is too low!";
        return;
    }

    Trajectory filtered_trajectory(u);

    for (int64_t i = 0; i < static_cast<int64_t>(u.points.size()); ++i)
    {
        TrajectoryPoint tmp
        { };

        int64_t num_tmp = param_.path_filter_moving_ave_num;
        int64_t count = 0;
        double yaw = 0.0;

        if (i - num_tmp < 0)
        {
            num_tmp = i;
        }

        if (i + num_tmp > static_cast<int64_t>(u.points.size()) - 1)
        {
            num_tmp = static_cast<int64_t>(u.points.size()) - i - 1;
        }

        for (int64_t j = -num_tmp; j <= num_tmp; ++j)
        {
            const auto &p = u.points.at(static_cast<size_t>(i + j));

            tmp.pose.position.x += p.pose.position.x;
            tmp.pose.position.y += p.pose.position.y;
            tmp.pose.position.z += p.pose.position.z;

            tmp.longitudinal_velocity_mps += p.longitudinal_velocity_mps;
            tmp.acceleration_mps2 += p.acceleration_mps2;
            tmp.front_wheel_angle_rad += p.front_wheel_angle_rad;
            tmp.heading_rate_rps += p.heading_rate_rps;

            yaw += MPCUtils::getYaw(p.pose.orientation);
            tmp.lateral_velocity_mps += p.lateral_velocity_mps;
            tmp.rear_wheel_angle_rad += p.rear_wheel_angle_rad;

            ++count;
        }

        auto &p = filtered_trajectory.points.at(static_cast<size_t>(i));

        p.pose.position.x = tmp.pose.position.x / count;
        p.pose.position.y = tmp.pose.position.y / count;
        p.pose.position.z = tmp.pose.position.z / count;

        p.longitudinal_velocity_mps = tmp.longitudinal_velocity_mps / count;
        p.acceleration_mps2 = tmp.acceleration_mps2 / count;
        p.front_wheel_angle_rad = tmp.front_wheel_angle_rad / count;
        p.heading_rate_rps = tmp.heading_rate_rps / count;
        p.lateral_velocity_mps = tmp.lateral_velocity_mps / count;
        p.rear_wheel_angle_rad = tmp.rear_wheel_angle_rad / count;

        p.pose.orientation = planning_utils::getQuaternionFromYaw(yaw / count);
    }

    trajectory_resampled_ = filtered_trajectory;
}


boost::optional<Trajectory> PurePursuitLateralController::generatePredictedTrajectory()
{
    const auto closest_idx_result = planning_utils::findNearestIndex(
            output_tp_array_, current_pose_,
            pure_pursuit_->closest_thr_dist_,    //10.0  //max_dist
            pure_pursuit_->closest_thr_ang_);    //M_PI   //max_yaw

    if (!*closest_idx_result)
    {
         HAF_LOG_INFO
                        << " Other points, Failed to find closest point";
        return boost::none;
    }

    //
    const double remaining_distance = planning_utils::calcArcLengthFromWayPoint(
            trajectory_resampled_, *closest_idx_result,
            trajectory_resampled_.points.size() - 1);

    const auto num_of_iteration = std::max(
            static_cast<int>(std::ceil(
                    std::min(remaining_distance,
                            param_.prediction_distance_length)
                            / param_.prediction_ds)), 1);

    Trajectory predicted_trajectory;

    for (int i = 0; i < num_of_iteration; i++)
    {
        if (i == 0)
        {

            TrajectoryPoint p;
            p.pose = current_pose_;
            p.longitudinal_velocity_mps =
                    current_twist_.linear.x;

            predicted_trajectory.points.push_back(p);

            const auto pp_output = calcTargetCurvature(true,
                    predicted_trajectory.points.at(i).pose);

            commStruct::AckermannLateralCommand tmp_msg;

            if (pp_output)
            {
                tmp_msg = generateCtrlCmdMsg(pp_output->curvature);

                predicted_trajectory.points.at(i).longitudinal_velocity_mps =
                        pp_output->velocity;
            }
            else
            {
                HAF_LOG_INFO << "<" << __FILE__
                        << "> | First point, Failed to solve pure_pursuit for prediction";
                tmp_msg = generateCtrlCmdMsg(0.0);
                break;
            }

            TrajectoryPoint p2;

            p2 = calcNextPose(param_.prediction_ds,
                    predicted_trajectory.points.at(i), tmp_msg);

            predicted_trajectory.points.push_back(p2);

        }
        else
        {
            const auto pp_output = calcTargetCurvature(false,
                    predicted_trajectory.points.at(i).pose);

            commStruct::AckermannLateralCommand tmp_msg;

            if (pp_output)
            {
                tmp_msg = generateCtrlCmdMsg(pp_output->curvature);
                predicted_trajectory.points.at(i).longitudinal_velocity_mps =
                        pp_output->velocity;
            }
            else
            {
                HAF_LOG_INFO
                        << " Other points, Failed to solve pure_pursuit for prediction";
                tmp_msg = generateCtrlCmdMsg(0.0);
                break;
            }

            predicted_trajectory.points.push_back(
                    calcNextPose(param_.prediction_ds,
                            predicted_trajectory.points.at(i), tmp_msg));
        }
    }

    predicted_trajectory.points.back().longitudinal_velocity_mps = 0.0;
    predicted_trajectory.header =
            trajectory_resampled_.header;
    //predicted_trajectory.header.stamp = trajectory_resampled_.header.stamp;

    return predicted_trajectory;
}


//
bool PurePursuitLateralController::isReady(
        [[maybe_unused]] const InputData &input_data)
{

    if (input_data.current_trajectory.trajectoryPoints.size() < 3)
    {
        return false;
    }

    return true;
}


//
Trajectory PurePursuitLateralController::ConvertToLocalTrajectory(
        const HafEgoTrajectory &input)
{

    Trajectory output;

    for (const HafTrajectoryPoint &p : input.trajectoryPoints)
    {
        TrajectoryPoint point;

        point.pose.position.x = p.wayPoint.x;
        point.pose.position.y = p.wayPoint.y;
        point.pose.position.z = p.wayPoint.z;

        point.longitudinal_velocity_mps = p.speed;

        output.points.push_back(point);
    }

    return output;
}


LateralOutput PurePursuitLateralController::run(const InputData &input_data)
{
    current_pose_ = input_data.current_pose;
    Adsfi::HafQuaternion q = MPCUtils::eulerToQuaternion(current_pose_.orientation.x, current_pose_.orientation.y, current_pose_.orientation.z);
    current_pose_.orientation = q;
    haf_trajectory = input_data.current_trajectory;
    current_twist_ = input_data.current_twist;
    current_steering_ = input_data.current_steering;
    trajectory_ = ConvertToLocalTrajectory(haf_trajectory);

    setResampledTrajectory();

    if (param_.enable_path_smoothing)
    {
        averageFilterTrajectory(trajectory_resampled_);
    }

    const auto cmd_msg = generateOutputControlCmd();

    LateralOutput output;
    output.control_cmd = cmd_msg;

    output.control_cmd.steering_tire_rotation_rate =
            current_twist_.linear.x
                    * std::tan(output.control_cmd.steering_tire_angle)
                    / param_.wheel_base;
HAF_LOG_INFO <<"steering_tire_angle:"<<output.control_cmd.steering_tire_angle;
HAF_LOG_INFO <<"current_twist_.linear.x:"<<current_twist_.linear.x;
HAF_LOG_INFO <<"param_.wheel_base:"<<param_.wheel_base;
    output.sync_data.is_steer_converged = calcIsSteerConverged(cmd_msg);
   
    if (cmd_msg.IsSteerConverged == false)
    {
        output.sync_data.is_steer_converged = false;
    }

    const auto predicted_trajectory = generatePredictedTrajectory();

    if (!predicted_trajectory)
    {
        HAF_LOG_INFO << " ## Failed to generate predicted trajectory";
    }
    else
    {
        publishPredictedTraj(*predicted_trajectory);
    }

    publishResampleTraj(trajectory_resampled_);

    m_seq++;

    return output;
}


void PurePursuitLateralController::publishPredictedTraj(
        const Trajectory &predicted_traj)
{
    mdc::visual::Path predicted_trajectory_msg;
    predicted_trajectory_msg.header.seq = m_seq;
    predicted_trajectory_msg.header.stamp =
            pure_pursuit::planning_utils::getCurrentTimeVis();
    predicted_trajectory_msg.header.frameId = "map";

    for (uint32_t i = 0; i < predicted_traj.points.size(); i++)
    {
        mdc::visual::PoseStamped pose;
        pose.pose.position.x = predicted_traj.points[i].pose.position.x;
        pose.pose.position.y = predicted_traj.points[i].pose.position.y;
        pose.pose.position.z = predicted_traj.points[i].pose.position.z;

        pose.pose.orientation.x = predicted_traj.points[i].pose.orientation.x;
        pose.pose.orientation.y = predicted_traj.points[i].pose.orientation.y;
        pose.pose.orientation.z = predicted_traj.points[i].pose.orientation.z;
        pose.pose.orientation.w = predicted_traj.points[i].pose.orientation.w;

        predicted_trajectory_msg.poses.push_back(pose);
    }

    HAF_LOG_INFO << "predicted_trajectory_msg.poses: "
            << predicted_trajectory_msg.poses.size();

    m_pub_predicted_traj.Publish(predicted_trajectory_msg);
}


//
void PurePursuitLateralController::publishResampleTraj(
        const Trajectory &trajectory_resampled_)
{

    mdc::visual::Path resampled_trajectory_msg;
    resampled_trajectory_msg.header.seq = m_seq;
    resampled_trajectory_msg.header.stamp =
            pure_pursuit::planning_utils::getCurrentTimeVis();
    resampled_trajectory_msg.header.frameId = "map";

    for (uint32_t i = 0; i < trajectory_resampled_.points.size(); i++)
    {
        mdc::visual::PoseStamped pose;
        pose.pose.position.x = trajectory_resampled_.points[i].pose.position.x;
        pose.pose.position.y = trajectory_resampled_.points[i].pose.position.y;
        pose.pose.position.z = trajectory_resampled_.points[i].pose.position.z;

        pose.pose.orientation.x =
                trajectory_resampled_.points[i].pose.orientation.x;
        pose.pose.orientation.y =
                trajectory_resampled_.points[i].pose.orientation.y;
        pose.pose.orientation.z =
                trajectory_resampled_.points[i].pose.orientation.z;
        pose.pose.orientation.w =
                trajectory_resampled_.points[i].pose.orientation.w;

        resampled_trajectory_msg.poses.push_back(pose);
    }

    HAF_LOG_INFO << "resampled_trajectory_msg.poses: "
            << resampled_trajectory_msg.poses.size();

    m_pub_Resample_traj.Publish(resampled_trajectory_msg);

    publishCurrentPose(current_pose_,"currentPos",0,1,0,0.2);

}
void PurePursuitLateralController::publishCurrentPose(const Adsfi::HafPose vehicle_pose,
                                      std::string ns, double r, double g, double b, double z)
{
  mdc::visual::Marker marker;
  marker.points.clear();
  //marker.header.frame_id = current_waypoints_.header.frame_id;
  marker.header.frameId = "map";
  const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
  uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
  uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % 1000000000UL;;
  marker.header.stamp.sec = sec;
  marker.header.stamp.nsec = nsec;
  marker.ns = ns;
  marker.id = 0;
  marker.type = mdc::visual::MarkerType::ARROW;
  marker.action = mdc::visual::MarkerAction::ADD;
  marker.scale.x = 1.0;
  marker.scale.y = 1.0;
  marker.scale.z = 1.0;
  marker.color.a = 1.0;
  marker.color.r = r;
  marker.color.g = g;
  marker.color.b = b;

  //geometry_msgs::Point p;
  marker.pose.position.x = vehicle_pose.position.x;
  marker.pose.position.y = vehicle_pose.position.y;
  marker.pose.position.z = 0;
  // p.z = traj.z.at(i) + z;
  //printf("p.x:%f,p.y:%f\n",p.x, p.y);
  //p.z = 0;
  //Adsfi::HafQuaternion q = MPCUtils::eulerToQuaternion(vehicle_pose.orientation.x, vehicle_pose.orientation.y, vehicle_pose.orientation.z);
  //marker.points.push_back(p);
  marker.pose.orientation.x = vehicle_pose.orientation.x;
  marker.pose.orientation.y = vehicle_pose.orientation.y;
  marker.pose.orientation.z = vehicle_pose.orientation.z;
  marker.pose.orientation.w = vehicle_pose.orientation.w;
  pursuit_current_pos_Pub.Publish(marker);
}

bool PurePursuitLateralController::calcIsSteerConverged(
        const commStruct::AckermannLateralCommand &cmd)
{
    //
    return std::abs(
            cmd.steering_tire_angle - current_steering_.steering_tire_angle)
            < static_cast<float>(param_.converged_steer_rad_);
}


commStruct::AckermannLateralCommand PurePursuitLateralController::generateOutputControlCmd()
{
    const auto pp_output = calcTargetCurvature(true,
            current_pose_);

    commStruct::AckermannLateralCommand output_cmd;

    if (pp_output)
    {
        output_cmd = generateCtrlCmdMsg(pp_output->curvature);
        prev_cmd_ = output_cmd;
        output_cmd.IsSteerConverged = true;
        publishDebugMarker();
    }
    else
    {
        HAF_LOG_INFO
                << " Failed to solve pure_pursuit for control command calculation";
        output_cmd = generateCtrlCmdMsg(0.0);
        output_cmd.IsSteerConverged = false;
    }

    return output_cmd;
}

commStruct::AckermannLateralCommand PurePursuitLateralController::generateCtrlCmdMsg(
        const double target_curvature)
{

    commStruct::AckermannLateralCommand cmd;
    timeval tv;
    gettimeofday(&tv, nullptr);

    const uint32_t usScaler = 1000U;
    cmd.stamp.sec = tv.tv_sec;
    cmd.stamp.nsec = tv.tv_usec * usScaler;

    cmd.steering_tire_angle = planning_utils::convertCurvatureToSteeringAngle(
            param_.wheel_base,
            target_curvature);
    return cmd;
}


std::vector<Adsfi::Point3d> PurePursuitLateralController::generateTrajectoryCircle(
        const Adsfi::Point3d &target,
        const Adsfi::HafPose &current_pose)
{
    constexpr double theta_range = M_PI / 10;
    constexpr double step_rad = 0.005;

    const double radius = pure_pursuit::planning_utils::calcRadius(target,
            current_pose);

    std::vector<Adsfi::Point3d> trajectory_circle;
    for (double theta = -theta_range; theta <= theta_range; theta += step_rad)
    {
        Adsfi::Point3d p;
        p.x = radius * sin(theta);
        p.y = radius * (1 - cos(theta));
        p.z = target.z;

        trajectory_circle.push_back(
                pure_pursuit::planning_utils::transformToAbsoluteCoordinate2D(p,
                        current_pose));
    }

    return trajectory_circle;
}


mdc::visual::Marker PurePursuitLateralController::createNextTargetMarker(
        const Adsfi::Point3d &next_target)
{
    auto marker = createDefaultMarker("map", "next_target", 0,
            mdc::visual::MarkerType::SPHERE, createMarkerScale(0.3, 0.3, 0.3),
            createMarkerColor(0.0, 1.0, 0.0, 1.0));

    marker.pose.position.x = next_target.x;
    marker.pose.position.y = next_target.y;
    marker.pose.position.z = next_target.z;

    return marker;
}


mdc::visual::Marker PurePursuitLateralController::createTrajectoryCircleMarker(
        const Adsfi::Point3d &target,
        const Adsfi::HafPose &current_pose)
{
    auto marker = createDefaultMarker("map", "trajectory_circle", 0,
            mdc::visual::MarkerType::LINE_STRIP, createMarkerScale(0.05, 0, 0),
            createMarkerColor(1.0, 1.0, 1.0, 1.0));

    Adsfi::Point3d geo_target_point;
    geo_target_point.x = target.x;
    geo_target_point.y = target.y;
    geo_target_point.z = target.z;

    const auto trajectory_circle = generateTrajectoryCircle(geo_target_point,
            current_pose);

    for (auto p : trajectory_circle)
    {
        mdc::visual::Point p_tmp;
        p_tmp.x = p.x;
        p_tmp.y = p.y;
        p_tmp.z = p.z;
        marker.points.push_back(p_tmp);

        marker.colors.push_back(marker.color);
    }

    return marker;
}


void PurePursuitLateralController::publishDebugMarker()
{
    mdc::visual::MarkerArray marker_array;

    marker_array.markers.push_back(
            createNextTargetMarker(debug_data_.next_target));

    marker_array.markers.push_back(
            createTrajectoryCircleMarker(debug_data_.next_target,
                    current_pose_));
pursuit_debug_Pub.Publish(marker_array);

}


boost::optional<PpOutput> PurePursuitLateralController::calcTargetCurvature(
        bool is_control_output, Adsfi::HafPose pose)
{

    if (trajectory_resampled_.points.size() < 3)
    {
        HAF_LOG_INFO << " ### Received path size is < 3, ignored";
        return
        {};
    }

    const auto closest_idx_result = planning_utils::findNearestIndex(
            output_tp_array_, pose, pure_pursuit_->closest_thr_dist_,
            pure_pursuit_->closest_thr_ang_);

    if (!closest_idx_result)
    {
        HAF_LOG_INFO << " ###$$$ Cannot find closest waypoint! ";
        return
        {};
    }

    const double target_vel = trajectory_resampled_.points.at(
            *closest_idx_result).longitudinal_velocity_mps;

    const double lateral_error = planning_utils::calcLateralOffset(
            trajectory_resampled_.points, pose.position);

    const double current_curvature = calcCurvature(*closest_idx_result);

    const bool is_reverse = (target_vel < 0);

    const double min_lookahead_distance =
            is_reverse ? param_.reverse_min_lookahead_distance :
                    param_.min_lookahead_distance;

    double lookahead_distance = min_lookahead_distance;

    //
    if (is_control_output)
    {
        lookahead_distance = calcLookaheadDistance(lateral_error,
                current_curvature, current_twist_.linear.x,
                min_lookahead_distance, is_control_output);

    }
    else
    {
        lookahead_distance = calcLookaheadDistance(lateral_error,
                current_curvature, target_vel, min_lookahead_distance,
                is_control_output);
    }

    //
   lookahead_distance = lookahead_distance + 15.0;

    pure_pursuit_->setCurrentPose(pose);

    pure_pursuit_->setWaypoints(
            planning_utils::extractPoses(trajectory_resampled_));

    pure_pursuit_->setLookaheadDistance(lookahead_distance);

    const auto pure_pursuit_result = pure_pursuit_->run();

    if (!pure_pursuit_result.first)
    {
        return
        {};
    }

    const auto kappa = pure_pursuit_result.second;

    if (is_control_output)
    {
        debug_data_.next_target = pure_pursuit_->getLocationOfNextTarget();
    }

    PpOutput output
    { };

    output.curvature = kappa;

    if (!is_control_output)
    {
        output.velocity = current_twist_.linear.x;
    }
    else
    {
        output.velocity = target_vel;
    }

    return output;
}

}    // namespace pure_pursuit
