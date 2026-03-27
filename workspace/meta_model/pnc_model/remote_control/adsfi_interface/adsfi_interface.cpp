#include <sys/syscall.h>
#include <unistd.h>

#include "adsfi_interface.h"
#include "custom_stack.h"
#include "log/avos_node.h"
#include "base_control/adsfi_struct_adapter.h"

int AdsfiInterface::GetParam() {
    char temp[PATH_MAX];
    if (getcwd(temp, PATH_MAX) != NULL) {
        std::string current_path = temp;
        ApInfo("remote_control") << "[Remote]: Current work path: " << current_path;
    } else {
        perror("getcwd() error");
    }

    if (!lat_config.LoadVechleParamFromModle()) {
        ApInfo("remote_control") << "[LAT_CONTTOL] load vehicle_interface failed";
        return -1;
    }
    if (!lat_config.LoadLatControlParamFromModle()) {
        ApInfo("remote_control") << "[LAT_CONTTOL] load lat control failed";
        return -1;
    }

    LoncontrollerInterfaceConfig lon_config;
    if (!lon_config.LoadVechleParamFromModle()) {
        ApInfo("remote_control") << "[LON_CONTROL] : vehilce_interface load failed";
        return -1;
    };
    if (!lon_config.LoadLonControlParamFromModle()) {
        ApInfo("remote_control") << "[LON_CONTROL] : lon_control conf load failed";
        return -1;
    }

    if (lon_config.upper_controller_config.ret_enable == 0) {
        FLAGS_enable_log = false;
    } else {
        FLAGS_enable_log = true;
    }

    avos::remote_control_purepursuit::LatManager::Instance()->SetConfig(lat_config);
    avos::remote_control_pid::LonParamManager::Instance()->SetConfig(lon_config);
    avos::remote_control_aeb::RemoteSafetyAlert::Instance()->SetVehicleParam(lat_config, lon_config);

    has_load_config_ = true;
    ApInfo("remote_control") << "----------------- load conf ok -----------------";
    return 0;
}

std::vector<PlanningTrajectoryPoint> AdsfiInterface::UpdatePathInfoByLatestLoc(
    const std::vector<PlanningTrajectoryPoint>& origin_path, const double xg, const double yg, const double yaw) {
    std::vector<PlanningTrajectoryPoint> origin_path_match;
    PlanningTrajectoryPoint              pre_point, current_point;
    memset(&pre_point, 0, sizeof(pre_point));
    memset(&current_point, 0, sizeof(current_point));
    double loc_xg_while = xg;
    double loc_yg_while = yg;
    for (int i = 0; i < origin_path.size(); i++) {
        PointGCCS vehicle_loc = {loc_xg_while, loc_yg_while, yaw};
        PointGCCS path_point_loc = {origin_path[i].xg, origin_path[i].yg, origin_path[i].angleglobal};
        PointVCS  vcs;
        avos::planning::CoordTransform::Instance()->GCCS2VCS(vehicle_loc, path_point_loc, vcs);

        current_point.x = vcs.x;
        current_point.y = vcs.y;

        current_point.angle = vcs.angle;
        current_point.xg = origin_path[i].xg;
        current_point.yg = origin_path[i].yg;
        current_point.curvature = origin_path[i].curvature;
        if (i <= 0) {
            current_point.length = 0;
        } else {
            current_point.length += std::hypot(pre_point.x - current_point.x, pre_point.y - current_point.y);
        }
        current_point.velocity = origin_path[i].velocity;
        current_point.a = origin_path[i].a;
        current_point.angleglobal = origin_path[i].angleglobal;
        pre_point = current_point;
        origin_path_match.push_back(current_point);
    }
    return origin_path_match;
}

void AdsfiInterface::FillPurepursuitLatInput(LatcontrolInput& lat_input, VehicleParam& vehicle_param,
                                             const std::vector<PlanningTrajectoryPoint>& traj, const double xg,
                                             const double yg, const double yaw) {
    ApInfo("remote_control") << "----------- fill lat input ------------------";
    int                                  path_size = traj.size();
    std::vector<PlanningTrajectoryPoint> origin_path_bak;
    origin_path_bak = traj;
    origin_path_bak = UpdatePathInfoByLatestLoc(origin_path_bak, xg, yg, yaw);
    lat_input.traject_path.x = Eigen::VectorXd(path_size);
    lat_input.traject_path.y = Eigen::VectorXd(path_size);
    lat_input.traject_path.xg = Eigen::VectorXd(path_size);
    lat_input.traject_path.yg = Eigen::VectorXd(path_size);
    lat_input.traject_path.angleglobal = Eigen::VectorXd(path_size);
    lat_input.traject_path.angle = Eigen::VectorXd(path_size);
    lat_input.traject_path.distance = Eigen::VectorXd(path_size);
    lat_input.traject_path.velocity = Eigen::VectorXd(path_size);
    lat_input.traject_path.curvature = Eigen::VectorXd(path_size);
    for (int i = 0; i < path_size; ++i) {
        lat_input.traject_path.x(i) = origin_path_bak[i].x;
        lat_input.traject_path.y(i) = origin_path_bak[i].y;
        lat_input.traject_path.xg(i) = origin_path_bak[i].xg;
        lat_input.traject_path.yg(i) = origin_path_bak[i].yg;
        lat_input.traject_path.angleglobal(i) = origin_path_bak[i].angleglobal;
        lat_input.traject_path.angle(i) = origin_path_bak[i].angle;
        lat_input.traject_path.distance(i) = origin_path_bak[i].length;
        lat_input.traject_path.velocity(i) = origin_path_bak[i].velocity;
        lat_input.traject_path.curvature(i) = origin_path_bak[i].curvature;
    }
    lat_input.traject_path.PathOptimizer();
    // FillVehicleCoordinate();
    memset(lat_input.vehicle_coordinate.x_car, 0, sizeof(lat_input.vehicle_coordinate.x_car));
    memset(lat_input.vehicle_coordinate.y_car, 0, sizeof(lat_input.vehicle_coordinate.y_car));
    lat_input.vehicle_coordinate.x_car[0] = vehicle_param.rear_mid_x + vehicle_param.car_wheel_base;
    lat_input.vehicle_coordinate.y_car[0] = vehicle_param.rear_mid_y;
    lat_input.vehicle_coordinate.x_car[1] = vehicle_param.rear_mid_x;
    lat_input.vehicle_coordinate.y_car[1] = vehicle_param.rear_mid_y + vehicle_param.car_width * 0.5;
    lat_input.vehicle_coordinate.x_car[2] = vehicle_param.rear_mid_x;
    lat_input.vehicle_coordinate.y_car[2] = vehicle_param.rear_mid_y - vehicle_param.car_width * 0.5;
    lat_input.vehicle_coordinate.x_car[3] = vehicle_param.rear_mid_x;
    lat_input.vehicle_coordinate.y_car[3] = vehicle_param.rear_mid_y;
    //---------底盘反馈的速度转角-----------//

    lat_input.vehicle_state.car_speed = vehicle_param.sbjspeed;
    lat_input.vehicle_state.wheel_angle = vehicle_param.uisteerangle;
    lat_input.reverse_flag = traj.front().direction;
    lat_input.loc_xg = xg;
    lat_input.loc_yg = yg;
    lat_input.loc_yaw = yaw;
    // lat_input.virtual_speed = loncontrol_debug.virtual_speed;
}

void AdsfiInterface::FillPidLonInput(LoncontrolInput&                                      lon_input,
                                     const std::shared_ptr<ara::adsfi::VehicleInformation> vehicle_param) {
    // lon_input.path = traj;
    lon_input.sbjspeed = vehicle_param->vehicle_act_state.speed;
    // lon_input.current_direction = traj.front().direction;
    lon_input.system_state = 0;
    lon_input.accel = vehicle_param->vehicle_act_state.acceleration;
    lon_input.task_executing = false;
    // lon_input.not_accurate_stop = taskinfo.not_accurate_stop;
    if (vehicle_param->vehicle_act_state.drive_mode == 0) {
        lon_input.is_auto_mode = 1;
    } else {
        lon_input.is_auto_mode = 0;
    }
    lon_input.uisteerangle = vehicle_param->vehicle_act_state.steer_angle;
    lon_input.shiftlvlposition = (unsigned int)vehicle_param->vehicle_act_state.shift_position;
    // lon_input.taskmode = taskmode;
    // lon_input.pitch = pitch;
    // lon_input.yaw = yaw;
    // lon_input.perception_info.cells = perception_info.cells;
    // lon_input.perception_info.objects = perception_info.objects;
    // lon_input.car_fault_level = car_fault_level;
    lon_input.torque_max =
        avos::remote_control_pid::LonParamManager::Instance()->GetConfig().lower_controller_config.torque_max;
}

int AdsfiInterface::BasicRemote_GetZone(const double longitude) {
    double longZone = 0.0;
    if (longitude < 0.0) {
        longZone = ((180.0 + longitude) / 6.0) + 1;
    } else {
        longZone = (longitude / 6.0) + 31;
    }
    return static_cast<int>(longZone);
};

std::vector<PlanningTrajectoryPoint> AdsfiInterface::Generate_TrackPath(
    std::shared_ptr<ara::adsfi::VehicleInformation> vehicle_info_ptr,
    std::shared_ptr<ara::adsfi::MsgHafLocation>     loc_ptr,
    std::shared_ptr<ara::adsfi::AppRemoteControl>   app_remote_ctrl_ptr,
    std::vector<PlanningTrajectoryPoint>&           origin_path) {
    float                   car_sbjspeed = vehicle_info_ptr->vehicle_act_state.speed;
    PlanningTrajectoryPoint current_point;
    memset(&current_point, 0, sizeof(current_point));
    const float   planning_time = 8.0;
    const float   delta_t = 0.05;
    const float   app_steering_wheel_threshold = 9000.0;
    const float   time_interval_threshold = 1.0; // 1s
    const double  delta_angle = 5.0;

    PointGCCS car_pos, target_pos;
    PointVCS  vcs_pose;
    car_pos.xg = loc_ptr->pose.pose.position.x;
    car_pos.yg = loc_ptr->pose.pose.position.y;
    car_pos.angle = loc_ptr->pose.pose.orientation.z;

    int    amount_last_path_point = last_path.size();
    double distance_to_car_min = 1000;
    double distance_point_to_car = 1000;
    float  current_length = 0;

    //摇杆数值大于app_steering_wheel_threshold, 持续时间大于time_interval_threshold, yaw调整delta_angle
    if (std::fabs(app_remote_ctrl_ptr->steering_wheel) >= app_steering_wheel_threshold) {
        // ApInfo("remote_control") << "11111111111 app_remote_ctrl_ptr->steering_wheel = " <<
        // app_remote_ctrl_ptr->steering_wheel;
        if (!track_timer_started_) {
            track_timer_started_ = true;
            track_start_t_ = avos::common::NodeTime::Now().ToSecond();
        } else {
            // Adjacent frames with the same sign
            lat_debug.InputAngle_start_t = track_start_t_;
            lat_debug.InputAngle_delta_t = avos::common::NodeTime::Now().ToSecond() - track_start_t_;
            if (app_remote_ctrl_ptr->steering_wheel * app_steering_wheel_last_) {
                if ((avos::common::NodeTime::Now().ToSecond() - track_start_t_) >= time_interval_threshold) {
                    start_planning_globalangle +=
                        app_remote_ctrl_ptr->steering_wheel > 0.0 ? delta_angle : -delta_angle;
                    track_timer_started_ = false;
                }
            } else {
                track_timer_started_ = false;
            }
        }
    } else {
        track_timer_started_ = false;
    }
    start_planning_globalangle = latmath_.normalize_angle(start_planning_globalangle);
    ApInfo("remote_control") << "current_car_angle : " << car_pos.angle
                               << " start_planning_globalangle = " << start_planning_globalangle
                               << " error = " << start_planning_globalangle - car_pos.angle;
    int index = 0;
    if (!last_path.empty()) {
        for (int i = 0; i < amount_last_path_point; i++) {
            distance_point_to_car = std::hypot(last_path[i].xg - car_pos.xg, last_path[i].yg - car_pos.yg);
            if (distance_point_to_car > distance_to_car_min + 5) {
                break;
            }
            if (distance_point_to_car < distance_to_car_min) {
                distance_to_car_min = distance_point_to_car;
                current_length = last_path[i].length;
                start_planning_xg = last_path[i].xg;
                start_planning_yg = last_path[i].yg;
                auto it =
                    std::lower_bound(last_path.begin(), last_path.begin() + i, std::fmax(current_length - 2.0, 0.0),
                                     [](const PlanningTrajectoryPoint& l, double value) { return l.length < value; });
                if (it != last_path.end()) {
                    index = std::distance(last_path.begin(), it);
                }
            }
        }
        if (current_length < car_sbjspeed * 0.2) {
            ApInfo("remote_control") << "current_length_is_too_short!!!!!!!!" << current_length;
            return last_path;
        }
    }
    for (int i = index; i < planning_time / delta_t; i++) {
        current_point.t = (i - index) * 0.05;
        current_point.angleglobal = start_planning_globalangle;
        current_point.xg = start_planning_xg + std::fmax(car_sbjspeed, 2.5) * current_point.t *
                                                   std::cos(current_point.angleglobal * M_PI / 180.0);
        current_point.yg = start_planning_yg + std::fmax(car_sbjspeed, 2.5) * current_point.t *
                                                   std::sin(current_point.angleglobal * M_PI / 180.0);
        current_point.curvature = 0.0;
        target_pos.xg = current_point.xg;
        target_pos.yg = current_point.yg;
        target_pos.angle = current_point.angleglobal;
        avos::planning::CoordTransform::Instance()->GCCS2VCS(car_pos, target_pos, vcs_pose);
        current_point.x = vcs_pose.x;
        current_point.y = vcs_pose.y;
        current_point.angle = vcs_pose.angle;
        if (i <= 0) {
            current_point.length = 0;
        } else if (i > 0 && !origin_path.empty()) {
            current_point.length +=
                std::hypot(current_point.xg - origin_path.back().xg, current_point.yg - origin_path.back().yg);
        } else {
            current_point.length +=
                std::hypot(current_point.xg - last_path.back().xg, current_point.yg - last_path.back().yg);
        }
        // ApInfo("remote_control")<<setprecision(12)<<"xg = "<<current_point.xg<<" yg = "<<current_point.yg<<"
        // angleglobal = "
        // <<current_point.angleglobal<<" length = "<<current_point.length;
        origin_path.push_back(current_point);
    }
    last_path = origin_path;
    app_steering_wheel_last_ = app_remote_ctrl_ptr->steering_wheel;
    return origin_path;
}

void AdsfiInterface::Init() {
    SetScheduleInfo("timmer", 50);
	// ApLogInterface::Instance()->Init("RemoteControl");

    double current_time = avos::common::NodeTime::Now().ToSecond();
    disconnect_start_time_ = current_time; 
    loc_last_time_         = current_time;
    process_start_time_    = current_time;
    last_tick_ = 0;
    hold_on_   = false;
    track_start_t_ = 0.0;
    track_timer_started_ = false;
    app_steering_wheel_last_ = 0.0;
    fixed_reference_path_.clear();
    has_locked_path_ = false;

    ApInfo("remote_control") << "init";
}

void AdsfiInterface::DeInit() {
    ApInfo("remote_control") << "deinit";
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::AppRemoteControl>& ptr) {
    ApInfo("remote_control") << "IdpAppRemoteControl callback:" << ptr->header.seq;
    app_remote_buf_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray>& ptr) {
    ApInfo("remote_control") << "FusionObj Callback: " << ptr->seq;
    fusion_buf_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::VehicleInformation>& ptr) {
    ApInfo("remote_control") << "IdpVehicleInformation  callback:" << ptr->header.seq;
    vehicleinfo_buf_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgHafLocation>& ptr) {
    ApInfo("remote_control") << "LocationInfo  callback:" << ptr->header.seq;
    locationinfo_buf_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::FaultData>& ptr) {
    ApInfo("remote_control") << "FaultData callback seq: " << ptr->header.seq;
    fault_data_buf_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv>& ptr) {
    ApInfo("remote_control") << "PerceptionStaticEnv callback";
    static_env_buf_.Set(ptr);
}

void AdsfiInterface::PackAndNotifyOutput(const ara::adsfi::CommonHeader& control_header, int fcw_level,
                                         double speed_limit, double desired_vel_ms, double target_angle,
                                         const PlanningTrajectory& center_traj, const PlanningTrajectory& left_traj,
                                         const PlanningTrajectory& right_traj) {
    // 1. 定义并填充 PlanningStatus
    ara::adsfi::PlanningStatus status_out;

    // 1.1 填充基础状态信息
    status_out.header = control_header;
    status_out.risk_level = fcw_level;
    status_out.current_limit_speed = speed_limit;
    status_out.current_set_speed = desired_vel_ms * 3.6; // m/s -> km/h
    status_out.current_set_angle = target_angle;

    // 1.2 填充三条轨迹 (内部 -> 外部格式转换)
    adsfi::utils::ConvertTrajectoryToMsgHafEgoTrajectory(center_traj, status_out.driving_center_trajectory);
    adsfi::utils::ConvertTrajectoryToMsgHafEgoTrajectory(left_traj, status_out.driving_left_boundary);
    adsfi::utils::ConvertTrajectoryToMsgHafEgoTrajectory(right_traj, status_out.driving_right_boundary);

    // 1.3 修正轨迹内部 Header (确保 seq 与外层同步)
    status_out.driving_center_trajectory.egoTrajectoryHeader.seq = control_header.seq;
    status_out.driving_left_boundary.egoTrajectoryHeader.seq = control_header.seq;
    status_out.driving_right_boundary.egoTrajectoryHeader.seq = control_header.seq;

    // 2. 独立的 MsgHafEgoTrajectory 输出
    ara::adsfi::MsgHafEgoTrajectory ego_traj_out = status_out.driving_center_trajectory;

    // 使用 Mutex 保护写入
    {
        std::lock_guard<std::mutex> lock(output_data_mutex_);
        cached_ego_trajectory_ = ego_traj_out;
        cached_planning_status_ = status_out;
        has_valid_output_cache_ = true;
    }

    // 4. 发送信号唤醒从 Process (去 SHM 里拿数据)
    safebuf_flag_ego_trajectory_.SetNotify(flag_ptr_);
    safebuf_flag_planning_status_.SetNotify(flag_ptr_);
}

int AdsfiInterface::Process(const std::string& name, std::shared_ptr<ara::adsfi::VehicleActControl>& control_ptr) {
    std::shared_ptr<ara::adsfi::AppRemoteControl>   app_remote_ctrl_ptr = nullptr;
    std::shared_ptr<ara::adsfi::VehicleInformation> vehicle_info_ptr = nullptr;
    std::shared_ptr<ara::adsfi::MsgHafLocation>     loc_ptr = nullptr;
    std::shared_ptr<ara::adsfi::PerceptionStaticEnv> static_env_ptr = nullptr;
    std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> fusion_ptr = nullptr;
    std::shared_ptr<ara::adsfi::FaultData>           fault_data_ptr = nullptr;

    bool ret = true;
    ret = app_remote_buf_.Get(app_remote_ctrl_ptr);
    if (ret == false) {
        ApInfo("remote_control") << "Process error IdpAppRemoteControl have not new data";
        
        // 唤醒副通道，并发送旧的主控数据
        *flag_ptr_ = -1; // 依然置为-1，作为内部状态标记
        safebuf_flag_ego_trajectory_.SetNotify(flag_ptr_);
        safebuf_flag_planning_status_.SetNotify(flag_ptr_);
        
        if (has_valid_control_cache_) {
            *control_ptr = cached_vehicle_act_control_; // 拷贝带有旧 seq 的上一帧数据
            return 0; // 返回 0，强制框架发送
        }
        return -1; // 只有在程序刚启动、连第一帧有效数据都没算出来时，才不发
    }
    
    ret = vehicleinfo_buf_.Get(vehicle_info_ptr);
    if (ret == false) {
        ApInfo("remote_control") << "Process error IdpVehicleInformation have not new data";
        
        *flag_ptr_ = -1; 
        safebuf_flag_ego_trajectory_.SetNotify(flag_ptr_);
        safebuf_flag_planning_status_.SetNotify(flag_ptr_);
        if (has_valid_control_cache_) {
            *control_ptr = cached_vehicle_act_control_;
            return 0;
        }
        return -1;
    }
    
    ret = locationinfo_buf_.Get(loc_ptr);
    if (ret == false) {
        ApInfo("remote_control") << "Process error LocationInfo have not new data";
        
        *flag_ptr_ = -1; 
        safebuf_flag_ego_trajectory_.SetNotify(flag_ptr_);
        safebuf_flag_planning_status_.SetNotify(flag_ptr_);
        if (has_valid_control_cache_) {
            *control_ptr = cached_vehicle_act_control_;
            return 0;
        }
        return -1;
    }
    std::shared_ptr<ara::adsfi::MsgHafLocation> adapted_loc = std::make_shared<ara::adsfi::MsgHafLocation>(*loc_ptr);
    {
        adapted_loc->pose.pose.position.x = loc_ptr->dr_pose.pose.position.x;
        adapted_loc->pose.pose.position.y = loc_ptr->dr_pose.pose.position.y;
        adapted_loc->pose.pose.position.z = loc_ptr->dr_pose.pose.position.z;

        adapted_loc->pose.pose.orientation.x = loc_ptr->dr_pose.pose.orientation.x;
        adapted_loc->pose.pose.orientation.y = loc_ptr->dr_pose.pose.orientation.y;
        adapted_loc->pose.pose.orientation.z = loc_ptr->dr_pose.pose.orientation.z;
    }
    ret = static_env_buf_.Get(static_env_ptr);
    if (ret == false) {
        ApInfo("remote_control") << "Process warning: PerceptionStaticEnv have not new data";
        static_env_ptr = std::make_shared<ara::adsfi::PerceptionStaticEnv>();
    }
    
    ret = fusion_buf_.Get(fusion_ptr);
    if (ret == false) {
        
    }
    fault_data_buf_.Get(fault_data_ptr);

    auto func_start_time = std::chrono::high_resolution_clock::now();

    control_ptr->header.time = DoubleToHafTime(avos::common::NodeTime::Now().ToSecond());
    control_ptr->header.seq = remote_control_seq_++;
    control_ptr->header.module_name = "BasicRemoteControl";
    control_ptr->header.data_name = "BasicRemoteControl_20260305";
    const double  steering_wheel_limit = 32767.0;       // range -32767 ~ +32767
    const double  brake_limit = 32767.0;                // range -32767 ~ 0
    const double  app_desired_velocity_limit = 32767.0; // range 0 ~ +32767
    const double  turn_value = 15000.0;
    const double  app_max_decel = -2.0;
    const double  steering_deadband = 500.0; //约1.5%
    bool          is_simulation_or_aion = false;
    bool          is_sim = false;
    bool          loc_ptr_init;
    double        disconnect_time_interval = 0.0;
    ApInfo("remote_control") << "loc_ptr->pose.pose.orientation.z: " << adapted_loc->pose.pose.orientation.z;
    ApInfo("remote_control") << "drive_mode = " << (std::int32_t)vehicle_info_ptr->vehicle_act_state.drive_mode;

    double speed_limit = app_remote_ctrl_ptr->speed_limit <= 5.0 ? 5.0 : app_remote_ctrl_ptr->speed_limit;
    double target_angle_ratio;
    double raw_steer_abs = std::fabs(app_remote_ctrl_ptr->steering_wheel);
    double effective_steer_abs = 0.0;
    // 死区判断与重映射
    if (raw_steer_abs <= steering_deadband) {
        effective_steer_abs = 0.0;
    } else {
        effective_steer_abs =
            (raw_steer_abs - steering_deadband) * (steering_wheel_limit / (steering_wheel_limit - steering_deadband));
        // 边界保护 (防止溢出)
        if (effective_steer_abs > steering_wheel_limit) {
            effective_steer_abs = steering_wheel_limit;
        }
    }
    if (effective_steer_abs <= turn_value) {
        target_angle_ratio = 0.38 / turn_value * effective_steer_abs;
    } else {
        target_angle_ratio = 0.38 + 0.55 / (steering_wheel_limit - turn_value) * (effective_steer_abs - turn_value);
    }
    if (app_remote_ctrl_ptr->steering_wheel < 0) {
        target_angle_ratio *= -1;
    }
    ApInfo("remote_control") << "AppRemoteControl steering_wheel = " << app_remote_ctrl_ptr->steering_wheel
                               << " gear = " << app_remote_ctrl_ptr->gear << " brake = " << app_remote_ctrl_ptr->brake
                               << " app_desired_velocity = " << app_remote_ctrl_ptr->accelerator
                               << " speed_limit = " << app_remote_ctrl_ptr->speed_limit
                               << " set_speed = " << app_remote_ctrl_ptr->set_speed
                               << " flag = " << app_remote_ctrl_ptr->remotectrl_flag
                               << " stop_flag = " << app_remote_ctrl_ptr->estop_flag;

    //故障处理,配置加载失败,定位延迟,遥控断连//
    if (!has_load_config_) {
        if (!filter_) {
            filter_.reset(new SecondOrderIIRFilter());
            filter_->Construct(0.1, 5.0);
        }
        if (GetParam() < 0) {
            FaultHandle::FaultApi::Instance()->SetFaultCode("3062002");
            ApInfo("remote_control") << "GetParam failed";
            // std::exit(-1);
            safebuf_flag_ego_trajectory_.SetNotify(flag_ptr_);
            safebuf_flag_planning_status_.SetNotify(flag_ptr_);
            return -1;
        } else {
            FaultHandle::FaultApi::Instance()->ResetFaultCode("3062002");
        }
        avos::remote_control_purepursuit::PurePursuit::Instance()->SetLatcontrolConfig(
            avos::remote_control_purepursuit::LatManager::Instance()->GetConfig().latcontrol_param);
        avos::remote_control_pid::LoncontrollerInterface::Instance()->InitConfig(
            avos::remote_control_pid::LonParamManager::Instance()->GetConfig());
    }
    double current_loc_time = TimeToSec(adapted_loc->header.timestamp);
    if ((current_loc_time - loc_last_time_) > 0.0 && (current_loc_time - loc_last_time_) < 0.5) {
        loc_ptr_init = true;
    } else {
        loc_ptr_init = false;
    }
    if (app_remote_ctrl_ptr->tick == last_tick_) {
        disconnect_time_interval = avos::common::NodeTime::Now().ToSecond() - disconnect_start_time_;
    } else {
        disconnect_start_time_ = avos::common::NodeTime::Now().ToSecond();
    }
    //故障处理,配置加载失败,定位延迟,遥控断连//

    //车辆参数填充横向控制
    LatcontrolInput       lat_input;
    LatcontrolOutput      lat_output;
    LatcontrolDebugStruct lat_debug;
    lat_input.vehicle_state.car_speed = vehicle_info_ptr->vehicle_act_state.speed;
    VehicleParam vehicle_param;
    vehicle_param.sbjspeed = vehicle_info_ptr->vehicle_act_state.speed;
    vehicle_param.uisteerangle = vehicle_info_ptr->vehicle_act_state.steer_angle;
    vehicle_param.car_wheel_base = lat_config.latcontrol_param.wheel_base_length;
    vehicle_param.car_width = lat_config.latcontrol_param.vehicle_width;
    //车辆参数填充纵向控制
    LoncontrolInput lon_input;
    FillPidLonInput(lon_input, vehicle_info_ptr);
    // 默认故障等级为 0 (正常)
    int32_t current_fault_level = 0;
    
    if (fault_data_ptr != nullptr) {
        // 从结构体中读取 fault_level
        current_fault_level = fault_data_ptr->fault_level;
    }

    lon_input.fault_level = current_fault_level;

    // 结合心跳超时逻辑：如果遥控断连超过0.5s且当前故障等级较低，强制设为2级故障
    if (disconnect_time_interval > 0.5 && lon_input.fault_level < 2) {
         lon_input.fault_level = 2;
         ApInfo("remote_control") << "Remote Disconnected > 0.5s, force fault_level = 2";
    }

    lon_input.pitch = adapted_loc->pose.pose.orientation.y;
    if (speed_limit / 3.6 <= desired_velocity) {
        desired_velocity = speed_limit / 3.6;
    }
    lon_input.vehicle_velocity = desired_velocity;

    lon_input.app_desired_velocity = app_remote_ctrl_ptr->accelerator / app_desired_velocity_limit * speed_limit / 3.6;

    if (app_remote_ctrl_ptr->brake >= 25000)
        hold_on_ = true;
    if (hold_on_ && app_remote_ctrl_ptr->accelerator > 100)
        hold_on_ = false;
    lon_input.brake = app_remote_ctrl_ptr->brake / brake_limit * app_max_decel;
    lon_input.gear = app_remote_ctrl_ptr->gear;
    // lon_input.fault_level = fault_data.fault_level;
    // if (disconnect_time_interval > 0.5 && fault_data.fault_level < 2) {
    //     lon_input.fault_level = 2;
    // }

    if (lat_config.car_type == "6x6" || lat_config.car_type == "sim")
        is_simulation_or_aion = false;
    else
        is_simulation_or_aion = true;

    if (lat_config.car_type == "sim") {
        is_sim = true;
    }

    ApInfo("remote_control") << "lat_config.car_type : " << lat_config.car_type;

    if (!is_simulation_or_aion) {
        // 当前6*6无法控制档位， 控制层自己判断档位 临时方案 add by syn
        if (app_remote_ctrl_ptr->gear == 3) {
            lon_input.app_desired_velocity =
                app_remote_ctrl_ptr->accelerator / app_desired_velocity_limit * speed_limit / 3.6;
            lon_input.app_desired_velocity = fmax(lon_input.app_desired_velocity, 0.0);
            lon_input.app_desired_velocity = fmin(lon_input.app_desired_velocity, 6.94444);
        } else if (app_remote_ctrl_ptr->gear == 1) {
            lon_input.app_desired_velocity =
                app_remote_ctrl_ptr->accelerator / app_desired_velocity_limit * speed_limit / 3.6;
            lon_input.app_desired_velocity = fmax(lon_input.app_desired_velocity, 0.0);
            lon_input.app_desired_velocity = fmin(lon_input.app_desired_velocity, 1.38888);
        } else {
            lon_input.app_desired_velocity = 0;
        }

        ApInfo("remote_control") << "current app set speed : " << lon_input.app_desired_velocity;

        if (lon_input.app_desired_velocity < 0.1) {
            zero_speed_cmd_counter_++; // 如果是停车指令，计数器+1
        } else {
            zero_speed_cmd_counter_ = 0; // 否则，只要有速度指令，立刻清零计数器
        }

        if (zero_speed_cmd_counter_ >= 3) {
            ApInfo("remote_control") << "持续停车指令，绕过滤波器！";
            filter_->Construct(0.1, 5.0);
        } else {
            // 执行滤波以保证平顺性
            lon_input.app_desired_velocity = filter_->Process(lon_input.app_desired_velocity);
        }

        if (app_remote_ctrl_ptr->accelerator < app_remote_ctrl_ptr->brake) {
            ApInfo("remote_control") << "响应brake: " << lon_input.brake;
            lon_input.app_desired_velocity = vehicle_info_ptr->vehicle_act_state.speed + lon_input.brake * 0.8;
            // double d_vel = app_remote_ctrl_ptr->brake / app_desired_velocity_limit * speed_limit / 3.6;
            // lon_input.app_desired_velocity -= d_vel;
            lon_input.app_desired_velocity = std::fmax(lon_input.app_desired_velocity, 0.0);
            if (vehicle_info_ptr->vehicle_act_state.speed < 0.2) {
                filter_->Construct(0.1, 5.0);
            }
        }
        ApInfo("remote_control") << "current control set speed : " << lon_input.app_desired_velocity;
    }

    LoncontrolDebugStruct lon_debug;
    LoncontrolOutput      lon_output;

    // 记录纵向控制模块开始执行的时间
    auto lon_control_start_time = std::chrono::high_resolution_clock::now();

    avos::remote_control_pid::LoncontrollerInterface::Instance()->ControlAgent(
        *app_remote_ctrl_ptr, lon_input, lon_output, is_simulation_or_aion, lon_debug);
    // 记录纵向控制模块执行结束的时间
    auto                                      lon_control_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> lon_duration_ms = lon_control_end_time - lon_control_start_time;
    // 打印纵向控制模块的耗时
    ApInfo("remote_control") << "纵向控制(Lon)模块耗时: " << lon_duration_ms.count() << " ms";

    if (app_remote_ctrl_ptr->remotectrl_flag == 0) {
        last_path.clear();
        lat_output.target_angle_ratio = target_angle_ratio;
        lat_output.ratio_is_valid = true;
        lat_output.targettorque = 0.0;
        ApInfo("remote_control") << " remotectrl_flag == 0 处于仅显示模式";
    } else if (app_remote_ctrl_ptr->remotectrl_flag == 1) { //基础遥控
        desired_velocity = vehicle_info_ptr->vehicle_act_state.speed;
        start_planning_xg = adapted_loc->pose.pose.position.x;
        start_planning_yg = adapted_loc->pose.pose.position.y;
        start_planning_globalangle = adapted_loc->pose.pose.orientation.z;

        last_path.clear();
        lat_output.target_angle_ratio = target_angle_ratio;
        lat_output.ratio_is_valid = true;
        lat_output.targettorque = lat_debug.midback * 100;
        ApInfo("remote_control") << "------------ Fully remote control ------------";
        process_start_time_ = avos::common::NodeTime::Now().ToSecond();
        // path_plan.trajectory = last_path;
    } else {
        ApInfo("remote_control") << "当前底盘档位： "
                                   << static_cast<int>(vehicle_info_ptr->vehicle_act_state.shift_position);
        if (vehicle_info_ptr->vehicle_act_state.shift_position == 3) {
            ApInfo("remote_control") << "当前档位为 D 档, 开始响应定速定向指令！";
            if (app_remote_ctrl_ptr->remotectrl_flag == 2) { //定速巡航

                if (app_remote_ctrl_ptr->set_speed > 0) {
                    desired_velocity = app_remote_ctrl_ptr->set_speed / 3.6;
                }

                if (!is_simulation_or_aion) {
                    lon_input.vehicle_velocity = filter_->Process(desired_velocity);
                }

                // 6*6 定速 加减速 增加阻尼 避免加减速度过大
                start_planning_xg = adapted_loc->pose.pose.position.x;
                start_planning_yg = adapted_loc->pose.pose.position.y;
                start_planning_globalangle = adapted_loc->pose.pose.orientation.z;

                last_path.clear();
                lat_output.target_angle_ratio = target_angle_ratio;
                lat_output.ratio_is_valid = true;
                lat_output.targettorque = lat_debug.midback * 100;
                ApInfo("remote_control") << "------------ Cruise at the fixed velocity ------------";
                process_start_time_ = avos::common::NodeTime::Now().ToSecond();
                // path_plan.trajectory = last_path;

            } else if (app_remote_ctrl_ptr->remotectrl_flag == 3) { //定向巡航
                lat_output.ratio_is_valid = false;
                desired_velocity = vehicle_info_ptr->vehicle_act_state.speed;
                // start fixed-direction after 0.6s
                // ApInfo("remote_control")<<"avos::common::NodeTime::Now().ToSecond() - process_start_time_ =
                // "<<avos::common::NodeTime::Now().ToSecond() - process_start_time_;
                lat_debug.direction_start_time = process_start_time_;
                lat_debug.direction_init_t = avos::common::NodeTime::Now().ToSecond() - process_start_time_;
                if (avos::common::NodeTime::Now().ToSecond() - process_start_time_ >= 0.6) {
                    //生成跟踪路径
                    std::vector<PlanningTrajectoryPoint> path;
                    path = Generate_TrackPath(vehicle_info_ptr, adapted_loc, app_remote_ctrl_ptr, path);
                    // path_plan.trajectory = path;
                    if (path.empty()) {
                        ApInfo("remote_control") << "-------------- path empty ---------------";
                    } else {
                        FillPurepursuitLatInput(lat_input, vehicle_param, path, adapted_loc->pose.pose.position.x,
                                                adapted_loc->pose.pose.position.y, adapted_loc->pose.pose.orientation.z);
                        avos::remote_control_purepursuit::PurePursuit::Instance()->SetParameter(lat_input);
                        avos::remote_control_purepursuit::PurePursuit::Instance()->CalculateMain(
                            lat_output, is_simulation_or_aion, lat_debug);
                        ApInfo("remote_control") << "------------ Cruise along the fixed direction ------------";
                    }
                } else {
                    start_planning_xg = adapted_loc->pose.pose.position.x;
                    start_planning_yg = adapted_loc->pose.pose.position.y;
                    start_planning_globalangle = adapted_loc->pose.pose.orientation.z;
                }
            } else if (app_remote_ctrl_ptr->remotectrl_flag == 4) {
                if (app_remote_ctrl_ptr->set_speed > 0) {
                    desired_velocity = app_remote_ctrl_ptr->set_speed / 3.6;
                }
                lat_debug.direction_start_time = process_start_time_;
                lat_debug.direction_init_t = avos::common::NodeTime::Now().ToSecond() - process_start_time_;
                if (avos::common::NodeTime::Now().ToSecond() - process_start_time_ >= 0.6) {
                    //生成跟踪路径
                    std::vector<PlanningTrajectoryPoint> path;
                    path = Generate_TrackPath(vehicle_info_ptr, adapted_loc, app_remote_ctrl_ptr, path);
                    // path_plan.trajectory = path;
                    if (path.empty()) {
                        ApInfo("remote_control") << "-------------- path empty ---------------";
                    } else {
                        FillPurepursuitLatInput(lat_input, vehicle_param, path, adapted_loc->pose.pose.position.x,
                                                adapted_loc->pose.pose.position.y, adapted_loc->pose.pose.orientation.z);
                        avos::remote_control_purepursuit::PurePursuit::Instance()->SetParameter(lat_input);
                        avos::remote_control_purepursuit::PurePursuit::Instance()->CalculateMain(
                            lat_output, is_simulation_or_aion, lat_debug);
                    }
                } else {
                    start_planning_xg = adapted_loc->pose.pose.position.x;
                    start_planning_yg = adapted_loc->pose.pose.position.y;
                    start_planning_globalangle = adapted_loc->pose.pose.orientation.z;
                }
                lat_output.target_angle_ratio = target_angle_ratio;
                lat_output.ratio_is_valid = false;
                lat_output.targettorque = lat_debug.midback * 100;
                ApInfo("remote_control") << "------------ Fully autonomous mode ------------";
            }
        } else {
            desired_velocity = 0.0;
            ApInfo("remote_control") << " 当前档位非 D 档， 拒绝响应任何指令！";
        }
    }

    // Aeb检测
    // TODO: 需要修改
    static PerceptionStaticEnv    perception_semantic_map;
    if (static_env_ptr != nullptr) {
        perception_semantic_map = *static_env_ptr;
    }

    static PerceptionFinalObjects perception_objects;
    static PerceptionFinalObjects output_risk_objs;
    perception_objects.objs.clear();
    output_risk_objs.objs.clear();

    if (fusion_ptr != nullptr) {
        adsfi::utils::ConvertFusionToFinalObjects(fusion_ptr, perception_objects);
    } 

    static ara::adsfi::PlanningStatus         interaction_data;
    PlanningTrajectory     temp_left_boundary;
    PlanningTrajectory     temp_right_boundary;
    PlanningTrajectory     temp_center_trajectory;

    int  fcw_level = 0;
    bool aeb_flag = false;
    auto tmp_car_status = std::make_shared<ara::adsfi::VehicleInformation>(*vehicle_info_ptr);
    // 记录遥控预警开始执行的时间
    auto aeb_start_time = std::chrono::high_resolution_clock::now();
    if (is_simulation_or_aion) {
        avos::remote_control_aeb::RemoteSafetyAlert::Instance()->ProcessSafetyCheck(
            *vehicle_info_ptr, *adapted_loc, perception_semantic_map, perception_objects, output_risk_objs,
            lat_config.car_type);
        avos::remote_control_aeb::RemoteSafetyAlert::Instance()->GetPredictedDriveArea(temp_left_boundary,
                                                                                       temp_right_boundary);
        avos::remote_control_aeb::RemoteSafetyAlert::Instance()->GetPredictedCenterTrajectory(temp_center_trajectory);

        avos::remote_control_aeb::RemoteSafetyAlert::Instance()->GetAlertSignal(fcw_level);
        auto aeb_cmd = avos::remote_control_aeb::RemoteSafetyAlert::Instance()->GetAebCommand();
        bool aeb_flag = aeb_cmd.aeb_active;
        interaction_data.risk_level = fcw_level;
        interaction_data.current_limit_speed = speed_limit;
        interaction_data.current_set_speed = desired_velocity * 3.6;
        interaction_data.current_set_angle = start_planning_globalangle;
        ApInfo("remote_control") << "0000 preception_objects size: " << perception_objects.objs.size()
                                   << " output_risk_objs.size: " << output_risk_objs.objs.size();
        //计算预测中心线误差
        PlanningTrajectoryPoint pos_matched_point =
            avos::remote_control_purepursuit::LatManager::Instance()->GetVehiclePositionMatchedPoint(
                last_center_trajectory, *adapted_loc, lat_debug);
        double lat_error = avos::remote_control_purepursuit::LatManager::Instance()->ComputeLatErr(
            adapted_loc->pose.pose.position.x, adapted_loc->pose.pose.position.y, pos_matched_point.xg, pos_matched_point.yg,
            pos_matched_point.angleglobal);
        last_center_trajectory = temp_center_trajectory.trajectory;
        lat_debug.PreTra_lat_error = lat_error;
        avos::remote_control_purepursuit::LatManager::Instance()->LimitOutput(lat_input.vehicle_state, lat_output);
        control_ptr->lat_control.target_angle = lat_output.targetangle;
        control_ptr->lat_control.target_curvature = 0.0;
        control_ptr->lon_control.target_torque = lon_output.torque;
        control_ptr->lon_control.target_pressure = lon_output.pressure;
        control_ptr->lon_control.shift_position = static_cast<char>(lon_output.shift_position);
        if (aeb_flag && app_remote_ctrl_ptr->security_assist_enabled) {
            control_ptr->lon_control.target_torque = 0;
            control_ptr->lon_control.target_pressure = 1e5;
        }
        ApInfo("remote_control") << "Vehicle target_angle = " << control_ptr->lat_control.target_angle
                                   << " target_torque = " << control_ptr->lon_control.target_torque
                                   << " target_pressure = " << control_ptr->lon_control.target_pressure;
    } else {
        if (lon_output.target_velocity < 0.2) {
            lon_output.target_velocity = 0.0;
        }
        avos::remote_control_purepursuit::LatManager::Instance()->LimitOutputCurve(lat_input.vehicle_state, lat_output);
        control_ptr->lat_control.target_angle = 0.0;
        control_ptr->lat_control.target_curvature = lat_output.targetcurvature;
        if (app_remote_ctrl_ptr->gear == 3)
            control_ptr->lon_control.target_speed = lon_output.target_velocity;
        else if (app_remote_ctrl_ptr->gear == 1)
            control_ptr->lon_control.target_speed = -lon_output.target_velocity;
        else if (app_remote_ctrl_ptr->estop_flag == 1) {
            control_ptr->lon_control.target_speed = 0.0;
        }
        control_ptr->lon_control.shift_position = static_cast<char>(lon_output.shift_position);

        ApInfo("remote_control") << "Vehicle target_curvature = " << control_ptr->lat_control.target_curvature
                                   << " target_speed = " << control_ptr->lon_control.target_speed;
        // 6X6 AEB

        if (is_sim) {
            //当前FD中型车type写的sim，所以需要做修改！！
            tmp_car_status->vehicle_act_state.shift_position = static_cast<unsigned char>(app_remote_ctrl_ptr->gear);
            avos::remote_control_aeb::RemoteSafetyAlert::Instance()->ProcessSafetyCheck(
                *tmp_car_status, *adapted_loc, perception_semantic_map, perception_objects, output_risk_objs,
                lat_config.car_type, control_ptr->lat_control.target_curvature);
        } else {
            avos::remote_control_aeb::RemoteSafetyAlert::Instance()->ProcessSafetyCheck(
                *vehicle_info_ptr, *adapted_loc, perception_semantic_map, perception_objects, output_risk_objs,
                lat_config.car_type, control_ptr->lat_control.target_curvature);
        }

        avos::remote_control_aeb::RemoteSafetyAlert::Instance()->GetPredictedDriveArea(temp_left_boundary,
                                                                                       temp_right_boundary);
        avos::remote_control_aeb::RemoteSafetyAlert::Instance()->GetPredictedCenterTrajectory(temp_center_trajectory);

        auto aeb_cmd = avos::remote_control_aeb::RemoteSafetyAlert::Instance()->GetAebCommand();
        bool aeb_flag = aeb_cmd.aeb_active;
        interaction_data.risk_level = fcw_level;
        interaction_data.current_limit_speed = speed_limit;
        interaction_data.current_set_speed = desired_velocity * 3.6;
        interaction_data.current_set_angle = start_planning_globalangle;
        ApInfo("remote_control") << "0000 preception_objects size: " << perception_objects.objs.size()
                                   << " output_risk_objs.size: " << output_risk_objs.objs.size();
        //计算预测中心线误差
        PlanningTrajectoryPoint pos_matched_point =
            avos::remote_control_purepursuit::LatManager::Instance()->GetVehiclePositionMatchedPoint(
                last_center_trajectory, *adapted_loc, lat_debug);
        double lat_error = avos::remote_control_purepursuit::LatManager::Instance()->ComputeLatErr(
            adapted_loc->pose.pose.position.x, adapted_loc->pose.pose.position.y, pos_matched_point.xg, pos_matched_point.yg,
            pos_matched_point.angleglobal);
        last_center_trajectory = temp_center_trajectory.trajectory;
        lat_debug.PreTra_lat_error = lat_error;

        ApInfo("remote_control") << "#### aeb_enable = " << app_remote_ctrl_ptr->security_assist_enabled
                                   << ", aeb_flag = " << aeb_flag;
        if (app_remote_ctrl_ptr->security_assist_enabled && aeb_flag) {
            control_ptr->lon_control.target_speed = aeb_cmd.target_speed;
        }
    }

    // 记录遥控预警执行结束的时间
    auto                                      aeb_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> aeb_duration_ms = aeb_end_time - aeb_start_time;
    // 打印遥控预警模块的耗时
    ApInfo("remote_control") << "遥控预警模块耗时: " << aeb_duration_ms.count() << " ms";

    // ================= 模型验证功能开始 =================
    // 当摇杆归零（进入死区）或切出模式0时，清除旧轨迹，准备下一次录制
    if (app_remote_ctrl_ptr->remotectrl_flag != 0 || std::abs(app_remote_ctrl_ptr->steering_wheel) < 500) {
        if (has_locked_path_) {
            ApInfo("remote_control") << "[模型验证] 摇杆归零或模式切换，清除已冻结的验证轨迹。";
            has_locked_path_ = false;
            fixed_reference_path_.clear();
        }
    }

    // 2. 录制与评估逻辑：仅在 flag == 0 (显示模式) 且 摇杆有有效输入时运行
    if (app_remote_ctrl_ptr->remotectrl_flag == 0 && std::abs(app_remote_ctrl_ptr->steering_wheel) >= 500) {
        // 已经包含绝对坐标(xg, yg)
        if (!has_locked_path_ && !temp_center_trajectory.trajectory.empty()) {
            fixed_reference_path_ = temp_center_trajectory.trajectory;
            has_locked_path_ = true;

            ApInfo("remote_control") << "[模型验证] 轨迹已冻结！"
                  << " 锁定时的车头绝对坐标: (" << adapted_loc->pose.pose.position.x << ", "
                  << adapted_loc->pose.pose.position.y << ")"
                  << " 锁定时的摇杆值: " << app_remote_ctrl_ptr->steering_wheel;
        }

        // B. 实时评估：车辆实际运动后，计算与冻结轨迹的偏差
        if (has_locked_path_ && !fixed_reference_path_.empty()) {
            // 步骤1：在冻结的轨迹中，找到离当前车辆（后轴中心）最近的点
            // 注意：lat_debug 会被更新，记录 matched_index
            PlanningTrajectoryPoint matched_point =
                avos::remote_control_purepursuit::LatManager::Instance()->GetVehiclePositionMatchedPoint(
                    fixed_reference_path_, *adapted_loc, lat_debug);

            // 步骤2：计算横向误差
            // 输入：当前车辆绝对坐标(x,y) 与 匹配点的绝对坐标(xg, yg)及航向(angleglobal)
            // 原理：计算当前点到参考点切线的垂直距离
            double model_verify_lat_error = avos::remote_control_purepursuit::LatManager::Instance()->ComputeLatErr(
                adapted_loc->pose.pose.position.x, adapted_loc->pose.pose.position.y, matched_point.xg,
                matched_point.yg, matched_point.angleglobal);

            lat_debug.PreTra_lat_error = model_verify_lat_error;
            // 打印误差日志，这就是你要评估的“差值”
            ApInfo("remote_control") << "[模型验证] 实时偏差评估 -> "
                  << " 匹配点索引: " << lat_debug.pos_matched_index << " / " << fixed_reference_path_.size()
                  << " | 理论航向: " << matched_point.angleglobal
                  << " | 实际航向: " << adapted_loc->pose.pose.orientation.z
                  << " | >> 模型横向偏差: " << model_verify_lat_error << " 米 <<";
        } else {
            lat_debug.PreTra_lat_error = 0.0;
        }
    }
    // ================= 模型验证功能结束 =================

    if (hold_on_) {
        control_ptr->lon_control.target_speed = 0;
    }

    control_ptr->lat_control.target_torque = lat_debug.midback * 100;
    control_ptr->lon_control.actuator_mode = static_cast<char>(lon_output.driver_mode);

    // 在所有控制值计算并填充完毕后，检查 remotectrl_flag
    // 如果 flag 为 0，说明不需要遥控输出控制指令，此时将 control 的内容清零
    if (app_remote_ctrl_ptr->remotectrl_flag == 0) {
        ApInfo("remote_control") << "模式0，清空遥控控制指令！";

        ara::adsfi::VehicleLatControl lat_control;
        ara::adsfi::VehicleLonControl lon_control;
        control_ptr->lat_control = lat_control;
        control_ptr->lon_control = lon_control;
    }

    ApInfo("remote_control") << "#### car_speed = " << vehicle_info_ptr->vehicle_act_state.speed
                               << " lon_output.shift_position = " << int(lon_output.shift_position);

    // 计算车辆是否受阻
    FaultDiagnosis fault_diagnosis;
    bool           is_blocked =
        fault_diagnosis.IsMotionRestricted(vehicle_info_ptr->vehicle_act_state.speed, control_ptr->lon_control);
    if (is_blocked) {
        FaultHandle::FaultApi::Instance()->SetFaultCode("3062003");
        ApInfo("remote_control") << "车辆受阻！";
    } else {
        FaultHandle::FaultApi::Instance()->ResetFaultCode("3062003");
    }

    *flag_ptr_ = 1; // 赋值 1 通知副通道可以发送全新数据！
    PackAndNotifyOutput(control_ptr->header, fcw_level, speed_limit, desired_velocity, start_planning_globalangle,
                        temp_center_trajectory, temp_left_boundary, temp_right_boundary);

    loc_last_time_ = TimeToSec(adapted_loc->header.timestamp);
    last_tick_ = app_remote_ctrl_ptr->tick;

    // 记录函数执行结束的时间
    auto                                      func_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> func_duration_ms = func_end_time - func_start_time;
    cached_vehicle_act_control_ = *control_ptr;
    has_valid_control_cache_ = true;
    // 打印总耗时
    ApInfo("remote_control") << "BasicRemoteControl 总耗时: " << func_duration_ms.count() << " ms";
    return 0;
}

// Process for MsgHafEgoTrajectory
int AdsfiInterface::Process(const std::string& name, std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory>& ptr) {
    std::shared_ptr<int> temp_flag;
    safebuf_flag_ego_trajectory_.WaitForGet(temp_flag);

    // 使用 make_shared 给框架全新的内存，与底层发送线程彻底隔离
    std::lock_guard<std::mutex> lock(output_data_mutex_);
    if (has_valid_output_cache_) {
        ptr = std::make_shared<ara::adsfi::MsgHafEgoTrajectory>(cached_ego_trajectory_);
        return 0; 
    }
    return -1;
}

// Process for PlanningStatus
int AdsfiInterface::Process(const std::string& name, std::shared_ptr<ara::adsfi::PlanningStatus>& ptr) {
    std::shared_ptr<int> temp_flag;
    safebuf_flag_planning_status_.WaitForGet(temp_flag);

    std::lock_guard<std::mutex> lock(output_data_mutex_);
    if (has_valid_output_cache_) {
        ptr = std::make_shared<ara::adsfi::PlanningStatus>(cached_planning_status_);
        return 0; 
    }
    return -1;
}
