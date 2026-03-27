#include "ZmqApiHandler3.hpp"
// #include "ara/adsfi/impl_type_faultdata.h"
// #include "ara/adsfi/impl_type_sensorgps.h"
// #include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "ap_log/ap_log_interface.h"
#include "boost/type_traits.hpp"
#include "config_loader/custom_stack.h"
#include "monitor/faulthandle_api.hpp"
#include "shm_util/shm_param.h"

enum CAMERA_TYPE {
    CAMERA_NONE = 0,
    CAMERA_MAIN,
    CAMERA_LEFT_REAR,
    CAMERA_RIGHT_REAR,
    CAMERA_LEFT_FRONT,
    CAMERA_RIGHT_FRONT,
    CAMERA_BACK,
    CAMERA_AROUND,
    CAMERA_FRONT_NIGHT,
    CAMERA_BACK_NIGHT,
    CAMERA_GRID_MAP,
    CAMERA_LEFT_BACK,
    CAMERA_RIGHT_BACK,
    CAMERA_FRONT_MIDDLE
};

const std::unordered_map<int, std::string> cameraTypeMap = {{CAMERA_NONE, "none"},
                                                            {CAMERA_MAIN, "front"},
                                                            {CAMERA_LEFT_REAR, "left_back_around"},
                                                            {CAMERA_RIGHT_REAR, "right_back_around"},
                                                            {CAMERA_LEFT_FRONT, "left_front_around"},
                                                            {CAMERA_RIGHT_FRONT, "right_front_around"},
                                                            {CAMERA_BACK, "back"},
                                                            // {CAMERA_AROUND, "right_back"},
                                                            {CAMERA_AROUND, "fisheye_stitched"},
                                                            {CAMERA_FRONT_NIGHT, "front_night"},
                                                            {CAMERA_BACK_NIGHT, "back_night"},
                                                            {CAMERA_GRID_MAP, "grid_map"},
                                                            {CAMERA_LEFT_BACK, "left_back"},
                                                            {CAMERA_RIGHT_BACK, "right_back"},
                                                            {CAMERA_FRONT_MIDDLE, "front_middle"}};

static int SendCanMessageNative() {
    int                 s;
    struct sockaddr_can addr;
    struct ifreq        ifr;
    struct can_frame    frame;

    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        ApError("zmq_task") << "SocketCAN create failed: " << strerror(errno);
        return -1;
    }

    std::strcpy(ifr.ifr_name, "can1");
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        ApError("zmq_task") << "SocketCAN interface can1 not found: " << strerror(errno);
        close(s);
        return -2;
    }

    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ApError("zmq_task") << "SocketCAN bind failed: " << strerror(errno);
        close(s);
        return -3;
    }

    // 填充报文: 01010001#FF0000
    std::memset(&frame, 0, sizeof(frame));
    // ID: 01010001 是扩展帧 (Extended Frame)，必须加上 CAN_EFF_FLAG
    frame.can_id = 0x01010001 | CAN_EFF_FLAG;
    frame.can_dlc = 3; // 数据长度 3字节
    frame.data[0] = 0xFF;
    frame.data[1] = 0x00;
    frame.data[2] = 0x00;

    if (write(s, &frame, sizeof(frame)) != sizeof(frame)) {
        ApError("zmq_task") << "SocketCAN write failed: " << strerror(errno);
        close(s);
        return -4;
    }

    close(s);
    return 0; // 成功
}

ZmqApiHandler3::ZmqApiHandler3() : _zmq_construct(nullptr), _zmq_socket(nullptr), _enable_pre_autonomy_tasking(false) {
    try {
        _vehicle_information = std::make_shared<ara::adsfi::VehicleInformation>();
        _last_gps = std::make_shared<ara::adsfi::MsgHafLocation>();
        _return_point = std::make_shared<CommonPoint3f>(); // CommonPoint3f 在全局命名空间
        _target_obj = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
        _last_task_status = std::make_shared<ara::adsfi::TaskExecuteStatus>();
        _business_command = std::make_shared<ara::adsfi::BusinessCommand>(); // 业务消息
        _video_profile = std::make_shared<ara::adsfi::AppVideoProfile>();    // 视频配置
        _sensor_control = std::make_shared<ara::adsfi::VehicleSensorControl>();
        _task_manage_status = std::make_shared<ara::adsfi::TaskManageStatus>();
        _video_rtp = std::make_shared<ara::adsfi::VideoRTPParameter>(); // 使用ara::adsfi命名空间
        _return_point_status = std::make_shared<ReturnPointStatus>();
    } catch (const std::exception &e) {
        _ec.ec_add(Ec409::_ERRORCODE_INIT);
        std::rethrow_exception(std::current_exception());
    }
    _ec.ec_remove(Ec409::_ERRORCODE_INIT);

    auto        ptr = CustomStack::Instance();
    std::string prefix;

    app_common::StringUtils::trim(prefix);
    if (!prefix.empty() && prefix.back() != '/') {
        prefix += "/";
    }

    auto full_path = prefix + "hmi/common/default/manual_bps";
    if (!ptr->GetProjectConfigValue(full_path, _video_profile->manual_bps)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _video_profile->manual_bps;

    full_path = prefix + "hmi/common/default/manual_width";
    if (!ptr->GetProjectConfigValue(full_path, _video_profile->manual_width)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _video_profile->manual_width;

    full_path = prefix + "hmi/common/default/manual_height";
    if (!ptr->GetProjectConfigValue(full_path, _video_profile->manual_height)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _video_profile->manual_height;

    full_path = prefix + "hmi/common/default/remote_control_assist";
    if (!ptr->GetProjectConfigValue(full_path, _video_profile->remote_control_assist)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _video_profile->remote_control_assist;

    full_path = prefix + "hmi/common/default/remote_control_alarm";
    if (!ptr->GetProjectConfigValue(full_path, _video_profile->remote_control_alarm)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _video_profile->remote_control_alarm;

    std::int32_t time_window = 0;
    full_path = prefix + "hmi/common/default/task_timeout_guard";
    if (!ptr->GetProjectConfigValue(full_path, time_window) || time_window <= 0) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }

    full_path = prefix + "hmi/common/default/min_waypoints";
    if (!ptr->GetProjectConfigValue(full_path, _min_waypoints)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _min_waypoints;

    full_path = prefix + "hmi/zmq/task_endpoint";
    if (!ptr->GetProjectConfigValue(full_path, _task_endpoint) || _task_endpoint.empty()) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _task_endpoint;

    full_path = prefix + "hmi/zmq/loc_control_endpoint";
    if (!ptr->GetProjectConfigValue(full_path, _loc_control_endpoint) || _loc_control_endpoint.empty()) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _loc_control_endpoint;

    full_path = "enable_pre_autonomy_tasking";
    if (!ptr->GetProjectConfigValue(full_path, _enable_pre_autonomy_tasking)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _enable_pre_autonomy_tasking;

    full_path = "keyfile_storage_path";
    if (!ptr->GetProjectConfigValue(full_path, _key_file_storage_path) || _key_file_storage_path.empty()) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _key_file_storage_path;
    _track_direct_record_file_path = _key_file_storage_path + "/route_points.dat";

    std::string _persistence_path;
    full_path = prefix + "hmi/zmq/persistence_path";
    if (!ptr->GetProjectConfigValue(full_path, _persistence_path)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_task") << full_path << "=" << _persistence_path;
    InitPersistence(_persistence_path);

    _ec.ec_remove(Ec409::_ERRORCODE_CONFIG);

    state_read(*_business_command);
    state_read(*_video_profile);
    state_read(*_video_rtp);
    state_read(*_return_point_status);
    state_read(*_task_manage_status);
    // if (_video_profile->video_third_person == 1) {
    //     ApInfo("zmq_task") << "[Restore] Detected previous state: Front Night Vision. Applying...";
    //     SHMParamApi::Instance()->AddModuleParam("global");
    //     SHMParamApi::Instance()->SetIntModuleParam("mdc_camera_nv_camera_pos", 1); // 0:主，即前。
    //     1:从，即后。默认为0。
    // } else { //   恢复回普通 / 后视状态 CameraShmParam::SetCameraModuleParam("mdc_camera", 0, 0);
    //     ApInfo("zmq_task") << "[Restore] Detected previous state: Default/Back. Applying...";
    //     SHMParamApi::Instance()->AddModuleParam("global");
    //     SHMParamApi::Instance()->SetIntModuleParam("mdc_camera_nv_camera_pos", 0); // 0:主，即前。
    //     1:从，即后。默认为0。
    // }

    // 如果返航点已设置，恢复轨迹记录器
    if (_return_point_status->return_point_flag) {
        try {
            _track_direct_recorder_return = std::make_shared<app_common::WaypointDirectRecorder>(
                _track_direct_record_file_path, app_common::DirectRecorderMode::APPEND_ONLY);
        } catch (const std::exception &e) {
            ApError("zmq_task") << "create WaypointDirectRecorder with APPEND_ONLY failed, error: " << e.what();
        } catch (...) {
            ApError("zmq_task") << "create WaypointDirectRecorder with APPEND_ONLY failed";
        }
        // backoff policy: record from start postion
        if (nullptr == _track_direct_recorder_return) {
            try {
                _track_direct_recorder_return = std::make_shared<app_common::WaypointDirectRecorder>(
                    _track_direct_record_file_path, app_common::DirectRecorderMode::CREATE_TRUNC_APPEND);
            } catch (const std::exception &e) {
                ApError("zmq_task") << "create WaypointDirectRecorder with CREATE_TRUNC_APPEND failed, error: "
                                    << e.what();
            } catch (...) {
                ApError("zmq_task") << "create WaypointDirectRecorder with CREATE_TRUNC_APPEND failed";
            }
        }
    }
    try {
        auto key_file_storage_path_track = _key_file_storage_path + "/tracks";
        _gpx_file_handler_for_track =
            std::make_shared<app_common::GpxFileHandler>(key_file_storage_path_track, "default_track.txt");
        _task_status_indicator =
            std::make_shared<app_common::TimeWindowIndicator<std::int32_t>>(std::chrono::seconds(time_window));

        _zmq_construct = std::make_shared<ZmqConstruct>(1);
        _zmq_construct_ins = std::make_shared<ZmqConstruct>(1);

        std::thread([this]() { loop(); }).detach();
        std::thread([this]() { loop_ins(); }).detach();
        std::thread([this]() { persistence_thread_func(); }).detach();
    } catch (const std::exception &e) {
        _ec.ec_add(Ec409::_ERRORCODE_INIT);
        std::rethrow_exception(std::current_exception());
    }
    _ec.ec_remove(Ec409::_ERRORCODE_INIT);
    _video_profile->output_mode = 1; // 特别注意：不能删除！！！
}

void ZmqApiHandler3::values(ara::adsfi::BusinessCommand &business_command) {
    std::lock_guard<std::mutex> lg(_mtx);

    auto now = std::chrono::steady_clock::now();
    if (_record_last_trigger.time_since_epoch().count() > 0 &&
        std::chrono::duration_cast<std::chrono::seconds>(now - _record_last_trigger).count() > 10) {
        // 超过10秒没有新的触发，自动清除录制触发标志
        _record_last_trigger = {};
        _business_command->record_trigger_flag = 0;
        ApInfo("zmq_task") << "record_trigger_flag auto cleared after 10 seconds of no trigger";
    }

    business_command = *_business_command;
}

void ZmqApiHandler3::values(ara::adsfi::VehicleSensorControl &sensor_control) {
    std::lock_guard<std::mutex> lg(_mtx);
    sensor_control = *_sensor_control;
}

void ZmqApiHandler3::values(ara::adsfi::AppVideoProfile &video_profile) {
    std::lock_guard<std::mutex> lg(_mtx);

    video_profile = *_video_profile;
}

void ZmqApiHandler3::values(ara::adsfi::VideoRTPParameter &rtp_parameter) {
    std::lock_guard<std::mutex> lg(_mtx);
    rtp_parameter = *_video_rtp;
}

void ZmqApiHandler3::values(ara::adsfi::TaskManageStatus &task_manage_status) {
    std::lock_guard<std::mutex> lg(_mtx);
    task_manage_status = *_task_manage_status;
}

void ZmqApiHandler3::on_data_change(const ara::adsfi::MsgHafLocation &   gps,                 // 当前位置
                                    const ara::adsfi::TaskExecuteStatus &task_execute_status, // 规划状态（自主）
                                    const ara::adsfi::VehicleInformation &  vehicle_information,
                                    const ara::adsfi::MsgHafFusionOutArray &detected_objects,
                                    const ara::adsfi::MsgHafFusionOutArray &target_obj) {
    std::lock_guard<std::mutex> lg(_mtx);

    // ApInfo("zmq_task") << fmt::format("TASK.type :{}, TASK.status: {}, VEHICLE.drive_mode:{}",
    // task_execute_status.type,
    //                      task_execute_status.status, vehicle_information.vehicle_act_state.drive_mode);
    // ApInfo("zmq_task") << fmt::format("BUSS.business_mode: {}, BUSS.param.command: {}, BUSS.param.avoid: {},
    // BUSS.param.speed:
    // {}",
    //                      _business_command->business_mode, _business_command->param.command,
    //                      _business_command->param.task_avoid_mode, _business_command->param.max_speed);
    // ApInfo("zmq_task") << fmt::format("TRACK.file: {}, TRACK.dest: {}, TRACK.type: {}, TRACK.loop: {}, TRACK.road:
    // {}",
    //                      _business_command->param.patrol_name, _business_command->param.patrol_dest,
    //                      _business_command->param.patrol_type, _business_command->param.patrol_loop,
    //                      _business_command->param.patrol_roads);
    _task_manage_status->distance_to_nearest = task_execute_status.distance_to_nearest;
    // 进度/里程中转：
    // - progress：XTaskExecute 按 remaining_length_m 计算并带“完成保持/清零触发”规则的 0~100 百分比；
    // - total_mileage/current_mileage：与 progress 同口径（单位：m）。
    // 当前 Xzmq_notification3 对外发布 task_status.progress 使用的是 task_manage_status.task_progress，
    // 因此这里同时同步到 task_progress 与 progress 两个字段，避免上下游字段口径不一致。
    _task_manage_status->task_progress = task_execute_status.progress;
    _task_manage_status->progress = task_execute_status.progress;
    _task_manage_status->total_mileage = task_execute_status.total_mileage;
    _task_manage_status->current_mileage = task_execute_status.current_mileage;
    // 保护机制：如果任务下发了，一定时间内还没有开始执行，则保护性停止任务
    if (_business_command->business_mode != NONE) {
        _task_status_indicator->add(task_execute_status.status);
        if (0 == task_execute_status.status) {
            if (0 != _last_task_status->status) { // 任务结束
                // _task_manage_status->task_status = static_cast<unsigned char>(0);
                _business_command->business_mode = NONE;
                _business_command->param.command = IDLE;
                ApInfo("zmq_task") << "task_status changed from " << _last_task_status->status
                                   << " to 0, set business_mode=NONE param.command=IDLE";
            } else {
                auto is_all_0 = _task_status_indicator->is_all([](std::int32_t status) { return 0 == status; });
                auto time_range = _task_status_indicator->time_range();
                if (is_all_0) {
                    _business_command->business_mode = NONE;
                    _business_command->param.command = IDLE;
                    ApInfo("zmq_task") << "task_status stay 0 from " << time_range.first.time_since_epoch().count()
                                       << " to " << time_range.second.time_since_epoch().count()
                                       << ", set business_mode=NONE param.command=IDLE";
                    _task_manage_status->task_status = static_cast<unsigned char>(6);
                    _ec.ec_add(Ec409::_ERRORCODE_EXPIRED_WAIT_EXECUTING);
                } else {
                    _ec.ec_remove(Ec409::_ERRORCODE_EXPIRED_WAIT_EXECUTING);
                }
            }
        }
    }

    if (vehicle_information.vehicle_act_state.drive_mode == 0) {
        _business_command->param.return_lat = 0;
        _business_command->param.return_lon = 0;
        if (!_video_profile->points.empty()) {
            _video_profile->points.clear();
        }
        // if (_vehicle_information->vehicle_act_state.drive_mode != 0) {
        // _sensor_control->actor_control_enable = 0;
        // }
    }

    *_last_gps = gps;
    *_target_obj = target_obj;
    *_vehicle_information = vehicle_information;
    *_last_task_status = task_execute_status;

    // 0-空闲；1-执行中；2-暂停中；3-GPS位置异常；4-到达任务点；5-用户结束；6-正常结束；7-退自主结束；
    // 状态控制------自主状态下
    //  _task_manage_status->task_type = static_cast<unsigned char>(_last_task_status->type);
    if (_last_task_status->status == 5 || _last_task_status->status == 3 || _last_task_status->status == 7) {
        _task_manage_status->task_status = static_cast<unsigned char>(4);
    } else if (6 == _last_task_status->status) {
        _task_manage_status->task_status = static_cast<unsigned char>(5);
    }

    if (_track_direct_recorder_return) {
        _track_direct_recorder_return->append(gps.llh.y, gps.llh.x);
    }
}

std::string ZmqApiHandler3::getIdentityMsgStr(const std::string &topic) {
    std::string id_msg_str = "";
    auto        it = topic_maps.find(topic);
    if (it != topic_maps.end()) {
        id_msg_str = it->second;
    }
    return id_msg_str;
}

std::string ZmqApiHandler3::getInsIdentityMsgStr(const std::string &topic) {
    std::string id_msg_str = "";
    auto        it = topic_maps_ins.find(topic);
    if (it != topic_maps_ins.end()) {
        id_msg_str = it->second;
    }
    return id_msg_str;
}

void ZmqApiHandler3::handleTargetFollowAction(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_follow_target_action>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task")
        << "zmq recv " << topic
        << fmt::format(": action_id: {}, pixel_x: {}, pixel_y: {}, safe_distance: {}, thw: {}, follow_strategy: {}",
                       struct_ptr.action_id, struct_ptr.pixel_x, struct_ptr.pixel_y, struct_ptr.safe_distance,
                       struct_ptr.thw, struct_ptr.follow_strategy);
    auto follow_type = static_cast<int>(struct_ptr.follow_strategy);
    if (_business_command->business_mode != NONE) {
        _ec.ec_add(Ec409::_ERRORCODE_OTHER_TASK);
        ApInfo("zmq_task") << "there is already a task running, business_mode: " << _business_command->business_mode;
        sendRouterTopicBuffer(800, topic, topic);
        return;
    }
    _ec.ec_remove(Ec409::_ERRORCODE_OTHER_TASK);

    current_task_id = struct_ptr.action_id;
    // 跟随模式：0牵引1轨迹
    _business_command->param.follow_mode = 1;
    _business_command->param.follow_min_dis = static_cast<double>(struct_ptr.safe_distance / 10.0);
    _business_command->param.follow_thw = 3;
    _business_command->param.follow_x = static_cast<int>(struct_ptr.pixel_x);
    _business_command->param.follow_y = static_cast<int>(struct_ptr.pixel_y);
    _business_command->param.view_width = 1920;
    _business_command->param.view_height = 1080;
    _business_command->param.task_avoid_mode = 1;
    _business_command->param.id = -1;
    if (follow_type == 1) {
        _business_command->business_mode = FOLLOW_DR;
    } else {
        _business_command->business_mode = FOLLOW;
    }
    _business_command->param.max_speed = 0.0;
    _business_command->param.command = RUNNING;
    _task_manage_status->action_id = static_cast<unsigned char>(struct_ptr.action_id);
    _task_manage_status->task_type = static_cast<unsigned char>(2);
    _task_manage_status->task_status = static_cast<unsigned char>(1);
    _task_status_indicator->reset();
    _cv_persistence.notify_one();
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::handleTargetPointsAction(const std::string &topic, bool is_test_topic) noexcept(false) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_target_points_action>(_zmq_socket, topic, 10000);
    current_task_id = struct_ptr.action_id;

    ApInfo("zmq_task")
        << "zmq recv " << topic
        << fmt::format(
               ": current_id: {}, target_points_num: {}, point_attributes size: {}, use_roadnet: {}, loop_mode: {}",
               struct_ptr.action_id, struct_ptr.target_points_num, struct_ptr.point_attributes.size(),
               struct_ptr.use_roadnet, struct_ptr.loop_mode);
    if (_business_command->business_mode != NONE) {
        _ec.ec_add(Ec409::_ERRORCODE_OTHER_TASK);
        ApInfo("zmq_task") << "there is already a task running, business_mode: " << _business_command->business_mode;
        sendRouterTopicBuffer(800, topic, topic);
        return;
    }
    _ec.ec_remove(Ec409::_ERRORCODE_OTHER_TASK);

    _task_manage_status->action_id = static_cast<unsigned char>(struct_ptr.action_id);
    _task_manage_status->task_type = static_cast<unsigned char>(1);
    _task_manage_status->task_status = static_cast<unsigned char>(0);

    if (!app_common::GeoUtils::is_location_ok(*_last_gps)) {
        _ec.ec_add(Ec409::_ERRORCODE_GPS);
        ApInfo("zmq_task") << "gps location is not ok, status: " << _last_gps->locationState
                           << ", gps: " << app_common::GeoUtils::str_gps(*_last_gps);
        _task_manage_status->task_status = static_cast<unsigned char>(6);
        sendRouterTopicBuffer(0, topic, topic);
        return;
    }
    _ec.ec_remove(Ec409::_ERRORCODE_GPS);

    if (!_enable_pre_autonomy_tasking && _vehicle_information->vehicle_act_state.drive_mode == 0) {
        _ec.ec_add(Ec409::_ERRORCODE_AUTONOMY);
        ApInfo("zmq_task") << "autonomy tasking is not enabled, drive mode: "
                           << _vehicle_information->vehicle_act_state.drive_mode;
        _task_manage_status->task_status = static_cast<unsigned char>(6);
        sendRouterTopicBuffer(0, topic, topic);
        return;
    }
    _ec.ec_remove(Ec409::_ERRORCODE_AUTONOMY);

    if (!is_test_topic) {
        std::vector<Waypoint> planned_waypoints;
        for (const auto &item : struct_ptr.point_attributes) {
            Waypoint wp{};
            wp.lat = static_cast<double>(item.geo_point.latitude) * 1e-6;
            wp.lon = static_cast<double>(item.geo_point.longitude) * 1e-6;
            planned_waypoints.emplace_back(std::move(wp));
        }

        if (struct_ptr.target_points_num != 1 && struct_ptr.target_points_num <= _min_waypoints) {
            _ec.ec_add(Ec409::_ERRORCODE_TRACK_WAYPOINTS_NUMBER, std::to_string(struct_ptr.target_points_num));
            ApInfo("zmq_task") << "waypoints too few (and not single point), target_points_num: "
                               << struct_ptr.target_points_num;
            _task_manage_status->task_status = static_cast<unsigned char>(6);
            sendRouterTopicBuffer(801, topic, topic);
            return;
        }
        _ec.ec_remove(Ec409::_ERRORCODE_TRACK_WAYPOINTS_NUMBER);

        try {
            _gpx_file_handler_for_track->gpx_file_remove("TARGET_PLAN");
            _gpx_file_handler_for_track->gpx_file_new("TARGET_PLAN", "q", "z", planned_waypoints, false);
            _ec.ec_remove(Ec409::_ERRORCODE_TRACK_WAYPOINTS_SAVE);
        } catch (const std::runtime_error &e) {
            _ec.ec_add(Ec409::_ERRORCODE_TRACK_WAYPOINTS_SAVE, std::to_string(struct_ptr.target_points_num));
            ApInfo("zmq_task") << "waypoints to file error: " << e.what();
            sendRouterTopicBuffer(0, topic, topic);
            return;
        }
    }

    if (1 == struct_ptr.loop_mode) {
        _business_command->param.patrol_type = (std::int32_t)PatrolType::ClosedLoop;
        _business_command->param.patrol_loop = true;
    } else {
        _business_command->param.patrol_type = (std::int32_t)PatrolType::Oneway;
        _business_command->param.patrol_loop = false;
    }
    _business_command->param.patrol_name = "TARGET_PLAN";
    _business_command->param.patrol_dest = "z";

    if (struct_ptr.target_points_num == 1) {
        ApInfo("zmq_task") << "Received Single Point.";

        if (!struct_ptr.point_attributes.empty()) {
            const auto &pose = struct_ptr.point_attributes[0].pose;
            bool        heading_valid = (pose.heading != 36100);

            _business_command->param.pose.position.x = 0.0;
            _business_command->param.pose.position.y = 0.0;
            _business_command->param.pose.position.z = 0.0;
            _business_command->param.pose.orientation.x = 0.0;
            _business_command->param.pose.orientation.y = 0.0;
            _business_command->param.pose.orientation.z = 0.0;

            _business_command->param.pose.orientation.x = static_cast<double>(pose.roll) / 100.0;
            _business_command->param.pose.orientation.y = static_cast<double>(pose.pitch) / 100.0;

            if (heading_valid) {
                double heading_north_zero = static_cast<double>(pose.heading) / 100.0;
                double heading_east_zero = 90.0 - heading_north_zero;
                if (heading_east_zero < 0.0) {
                    heading_east_zero += 360.0;
                }
                double angle_val = (heading_east_zero > 180.0) ? (heading_east_zero - 360.0) : heading_east_zero;

                _business_command->param.pose.orientation.z = angle_val;
            } else {
                _business_command->param.pose.orientation.z = static_cast<double>(pose.heading) / 100.0;
            }

            _business_command->business_mode = POSE_ADJUST;

            // 检查所有字段是否都无效
            bool pitch_valid = (pose.pitch != 9100);
            bool roll_valid = (pose.roll != 9100);
            if (!heading_valid && !pitch_valid && !roll_valid) {
                ApError("zmq_task") << "recv Single Point received but all pose fields are invalid!";
                _task_manage_status->task_status = static_cast<unsigned char>(6);
                sendRouterTopicBuffer(0, topic, topic);
                return;
            }
        }
    } else {
        _business_command->business_mode = TRACK_DR;
    }
    _business_command->header.seq += 1;
    _business_command->param.command = PAUSED;

    _task_manage_status->task_status = static_cast<unsigned char>(1);
    _task_status_indicator->reset();
    _cv_persistence.notify_one();

    ApInfo("zmq_task") << "===============================================";
    ApInfo("zmq_task") << "business command:"
                       << " business_mode: " << _business_command->business_mode
                       << " points_num: " << struct_ptr.target_points_num
                       << " patrol_name: " << _business_command->param.patrol_name
                       << " patrol_type: " << _business_command->param.patrol_type << " pose.position(x,y,z): ("
                       << _business_command->param.pose.position.x << ", " << _business_command->param.pose.position.y
                       << ", " << _business_command->param.pose.position.z << ")"
                       << " pose.orientation(roll,pitch,yaw): (" << _business_command->param.pose.orientation.x << ", "
                       << _business_command->param.pose.orientation.y << ", "
                       << _business_command->param.pose.orientation.z << ")";
    ApInfo("zmq_task") << "===============================================";
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::loop() {
    while (!_stopped) {
        if (nullptr == _zmq_socket) {
            try {
                _zmq_socket = _zmq_construct->createRouter(_task_endpoint);
                _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_LISTEN);
            } catch (const zmq::error_t &e) {
                ApInfo("zmq_task") << "create route error: " << e.what();
                _ec.ec_add(Ec409::_ERRORCODE_ZMQ_LISTEN, _task_endpoint);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
        }

        std::string topic_str;
        std::string id_msg_str_;
        try {
            int recv_result = _zmq_construct->recvRouterTopicBase(_zmq_socket, id_msg_str_, topic_str, 1000);
            if (recv_result != 0) {
                continue;
            }
            app_common::StringUtils::trim(id_msg_str_);
            app_common::StringUtils::trim(topic_str);
            if (id_msg_str_.empty() || topic_str.empty()) {
                throw std::runtime_error(fmt::format("empty: id ({}) or topic ({})", id_msg_str_, topic_str));
            }
            _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_RECV);
            ApInfo("zmq_task") << "recv topic: " << topic_str;
            topic_maps[topic_str] = id_msg_str_;
        } catch (const zmq::error_t &e) {
            ApInfo("zmq_task") << "zmq recv error: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_RECV);
            continue;
        } catch (const std::exception &e) {
            ApInfo("zmq_task") << "zmq recv error: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_RECV);
            continue;
        }

        try {
            std::lock_guard<std::mutex> lg(_mtx);
            if (topic_str == "manager2auto_target_points_action") {
                handleTargetPointsAction(topic_str);
            } else if (topic_str == "manager2auto_action_config") {
                handlemanager2auto_aciton_config(topic_str);
            } else if (topic_str == "manager2auto_maneuvering_strategy") {
                handlemanager2auto_maneuvering_strategy(topic_str);
            } else if (topic_str == "manager2auto_perception_strategy") {
                handlemanager2auto_perception_strategy(topic_str);
            } else if (topic_str == "manager2auto_positioning_usage") {
                handlemanager2auto_positioning_usage(topic_str);
            } else if (topic_str == "manager2auto_follow_target_action") {
                handleTargetFollowAction(topic_str);
            } else if (topic_str == "manager2auto_return_point_set") {
                handlemanager2auto_return_point_set(topic_str);
            } else if (topic_str == "manager2auto_return_action") {
                handlemanager2auto_return_action(topic_str);
            } else if (topic_str == "manager2auto_command_control") {
                handlemanager2auto_command_control(topic_str);
            } else if (topic_str == "manager2auto_speed_limit_config") {
                handlemanager2auto_speed_limit_config(topic_str);
            } else if (topic_str == "manager2auto_emergency_brake") {
                handlemanager2auto_emergency_brake(topic_str);
            } else if (topic_str == "manager2auto_video_enable") {
                handlemanager2auto_video_enable(topic_str);
            } else if (topic_str == "manager2auto_video_set") {
                handlemanager2auto_video_set(topic_str);
            } else if (topic_str == "manager2auto_video_assist") {
                handlemanager2auto_video_assist(topic_str);
            } else if (topic_str == "manager2auto_data_logger") {
                handlemanager2auto_data_logger(topic_str);
            } else if (topic_str == "new_architecture_track_test") {
                handleTargetPointsAction(topic_str, true);
            } else {
                throw ExceptionZmqRecv("unknown topic: " + topic_str);
            }
            _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_RECV);
            _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_DESERIALIZE);
            _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_SEND);
        } catch (const ExceptionZmqRecv &e) {
            ApInfo("zmq_task") << "zmq recv error, topic: " << topic_str << ", desc: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_RECV);
        } catch (const ExceptionZmqDeserialize &e) {
            ApInfo("zmq_task") << "zmq deserialize error, topic: " << topic_str << ", desc: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_DESERIALIZE);
        } catch (const ExceptionZmqSend &e) {
            ApInfo("zmq_task") << "zmq send error, topic: " << topic_str << ", desc: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_SEND);
        } catch (const std::exception &e) {
            ApInfo("zmq_task") << "zmq handle error, topic: " << topic_str;
        } catch (...) {
            ApInfo("zmq_task") << "zmq unknown error, topic: " << topic_str;
        }
    }

    ApInfo("zmq_task") << "loop ended";
}

void ZmqApiHandler3::loop_ins() {
    while (!_stopped) {
        if (nullptr == _zmq_socket_ins) {
            try {
                _zmq_socket_ins = _zmq_construct_ins->createRouter(_loc_control_endpoint);
                _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_LISTEN);
            } catch (const zmq::error_t &e) {
                ApError("zmq_task") << "create ins route error: " << e.what();
                _ec.ec_add(Ec409::_ERRORCODE_ZMQ_LISTEN, _loc_control_endpoint);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
        }
        std::string topic_str;
        std::string id_msg_str_; // 客户端 identity

        try {
            int recv_result = _zmq_construct_ins->recvRouterTopicBase(_zmq_socket_ins, id_msg_str_, topic_str, 1000);

            if (recv_result != 0) {
                continue;
            }

            app_common::StringUtils::trim(id_msg_str_);
            app_common::StringUtils::trim(topic_str);
            if (id_msg_str_.empty() || topic_str.empty()) {
                throw std::runtime_error(fmt::format("empty: id ({}) or topic ({})", id_msg_str_, topic_str));
            }
            _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_RECV);
            ApInfo("zmq_task") << "recv topic: " << topic_str;
            topic_maps_ins[topic_str] = id_msg_str_;
        } catch (const ExceptionZmqRecv &e) {
            ApError("zmq_task") << "zmq recv error, topic: " << topic_str << ", desc: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_RECV);
            continue;
        } catch (const ExceptionZmqDeserialize &e) {
            ApError("zmq_task") << "zmq deserialize error, topic: " << topic_str << ", desc: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_DESERIALIZE);
            continue;
        } catch (const ExceptionZmqSend &e) {
            ApError("zmq_task") << "zmq send error, topic: " << topic_str << ", desc: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_SEND);
            continue;
        } catch (const std::exception &e) {
            ApError("zmq_task") << "zmq handle error, topic: " << topic_str;
            continue;
        } catch (...) {
            ApError("zmq_task") << "zmq unknown error, topic: " << topic_str;
            continue;
        }

        try {
            std::lock_guard<std::mutex> lg(_mtx);

            if (topic_str == "manager2loc_ins_control") {
                handlemanager2loc_ins_control(topic_str);
            } else if (topic_str == "manager2loc_loc_mode") {
                handlemanager2loc_loc_mode(topic_str);
            } else {
                throw ExceptionZmqRecv("unknown topic: " + topic_str);
            }
            _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_RECV);
            _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_DESERIALIZE);
            _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_SEND);
        } catch (const zmq::error_t &e) {
            ApError("zmq_task") << "[INS] zmq error: " << e.what();
            _zmq_socket_ins.reset();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_RECV);
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            continue;
        } catch (const ExceptionZmqRecv &e) {
            ApError("zmq_task") << "zmq recv error, topic: " << topic_str << ", desc: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_RECV);
        } catch (const ExceptionZmqDeserialize &e) {
            ApError("zmq_task") << "zmq deserialize error, topic: " << topic_str << ", desc: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_DESERIALIZE);
        } catch (const ExceptionZmqSend &e) {
            ApError("zmq_task") << "zmq send error, topic: " << topic_str << ", desc: " << e.what();
            _ec.ec_add(Ec409::_ERRORCODE_ZMQ_SEND);
        } catch (const std::exception &e) {
            ApError("zmq_task") << "zmq handle error, topic: " << topic_str;
        } catch (...) {
            ApError("zmq_task") << "zmq unknown error, topic: " << topic_str;
        }
    }
    ApInfo("zmq_task") << "loop_ins ended";
}

void ZmqApiHandler3::sendRouterTopicBuffer(const uint32_t ret_result, const std::string &topic,
                                           const std::string &res_topic) noexcept(false) {
    std::string result;
    uint32_t    result_value = static_cast<uint32_t>(ret_result);
    result.resize(sizeof(result_value)); // 确保足够空间存放 uint32_t
    std::memcpy(&result[0], &result_value, sizeof(result_value));
    std::string id_msg_str_ = "";
    id_msg_str_ = getIdentityMsgStr(topic);
    if (id_msg_str_.empty()) {
        throw ExceptionZmqSend(fmt::format("send topic: {} error - failed to get identity message", topic));
    }

    try {
        _zmq_construct->sendRouterTopicBuffer(_zmq_socket, id_msg_str_, res_topic, result.c_str(),
                                              sizeof(result_value));
        ApInfo("zmq_task") << "zmq send " << topic << ": id_msg_str: " << id_msg_str_ << ", result: " << ret_result;
    } catch (const zmq::error_t &e) {
        throw ExceptionZmqSend(fmt::format("send topic: {} error - {}", topic, e.what()));
    } catch (const std::exception &e) {
        throw ExceptionZmqSend(fmt::format("send topic: {} error - {}", topic, e.what()));
    } catch (...) {
        throw ExceptionZmqSend(fmt::format("send topic: {} error", topic));
    }
}

void ZmqApiHandler3::sendInsRouterTopicBuffer(const uint32_t ret_result, const std::string &topic,
                                              const std::string &res_topic) noexcept(false) {
    std::string result;
    uint32_t    result_value = static_cast<uint32_t>(ret_result);
    result.resize(sizeof(result_value)); // 确保足够空间存放 uint32_t
    std::memcpy(&result[0], &result_value, sizeof(result_value));
    std::string id_msg_str_ = "";
    id_msg_str_ = getInsIdentityMsgStr(topic);
    if (id_msg_str_.empty()) {
        throw ExceptionZmqSend(fmt::format("send topic: {} error - failed to get identity message", topic));
    }
    try {
        _zmq_construct_ins->sendRouterTopicBuffer(_zmq_socket_ins, id_msg_str_, res_topic, result.c_str(),
                                                  sizeof(result_value));
        ApInfo("zmq_task") << "zmq send " << topic << ": id_msg_str: " << id_msg_str_ << ", result: " << ret_result;
    } catch (const zmq::error_t &e) {
        throw ExceptionZmqSend(fmt::format("send topic: {} error - {}", topic, e.what()));
    } catch (const std::exception &e) {
        throw ExceptionZmqSend(fmt::format("send topic: {} error - {}", topic, e.what()));
    } catch (...) {
        throw ExceptionZmqSend(fmt::format("send topic: {} error", topic));
    }
}

void ZmqApiHandler3::handlemanager2auto_aciton_config(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_action_config>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic << fmt::format(":  degrade_mode: {}", struct_ptr.degrade_mode);
    auto avoid_mode = static_cast<int>(struct_ptr.degrade_mode);
    if (avoid_mode == 0) {
        _business_command->param.task_avoid_mode = 1;
    } else if (avoid_mode == 1) {
        _business_command->param.task_avoid_mode = 3;
    } else if (avoid_mode == 2) {
        _business_command->param.task_avoid_mode = 0;
    } else {
        ApInfo("zmq_task") << "zmq recv " << topic << fmt::format(": degrade_mode: {}", struct_ptr.degrade_mode)
                           << " is invalid";
        sendRouterTopicBuffer(0, topic, topic);
        return;
    }
    _cv_persistence.notify_one();
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::handlemanager2auto_maneuvering_strategy(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_maneuvering_strategy>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic << ": maneuvering_strategy_type: "
                       << static_cast<std::uint8_t>(struct_ptr.maneuvering_strategy_type);
    auto maneuvering_strategy_type = static_cast<uint8_t>(struct_ptr.maneuvering_strategy_type);
    if (maneuvering_strategy_type == 0) {
        _business_command->business_mode = TRACK_DR;
    } else if (maneuvering_strategy_type == 1) {
        _business_command->business_mode = ARRIVE;
    } else {
        ApInfo("zmq_task") << "zmq recv " << topic << ": maneuvering_strategy_type: "
                           << static_cast<std::uint8_t>(struct_ptr.maneuvering_strategy_type) << " is invalid";
        sendRouterTopicBuffer(0, topic, topic);
        return;
    }
    _cv_persistence.notify_one();
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::handlemanager2auto_perception_strategy(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_pereception_strategy>(_zmq_socket, topic, 10000);

    ApInfo("zmq_task") << "zmq recv " << topic
                       << fmt::format(":  pereception_strategy_type: {}", struct_ptr.pereception_strategy_type);
    auto pereception_strategy_type = static_cast<uint8_t>(struct_ptr.pereception_strategy_type);

    if (pereception_strategy_type < 1 || pereception_strategy_type > 4) {
        ApInfo("zmq_task") << "zmq recv " << topic
                           << fmt::format(": pereception_strategy_type: {}", struct_ptr.pereception_strategy_type)
                           << "is invalid";
        sendRouterTopicBuffer(0, topic, topic);
        return;
    } else if (pereception_strategy_type == 1) {
        _business_command->business_mode = 101;
    } else if (pereception_strategy_type == 2) {
        _business_command->business_mode = 1;
    } else if (pereception_strategy_type == 3) {
        _business_command->business_mode = 102;
    } else if (pereception_strategy_type == 4) {
        _business_command->business_mode = 103;
    }
    _cv_persistence.notify_one();
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::handlemanager2auto_positioning_usage(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_positioning_usage>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic
                       << ": positioning_type: " << static_cast<std::uint8_t>(struct_ptr.positioning_type);
    auto positioning_type = static_cast<uint8_t>(struct_ptr.positioning_type);
    // TODO: 预留接口，暂时不处理，后续根据实际需求再添加
    if (positioning_type < 0 || positioning_type > 2) {
        ApInfo("zmq_task") << "zmq recv " << topic << fmt::format(": positioning_type: {}", struct_ptr.positioning_type)
                           << " is invalid";
        sendRouterTopicBuffer(0, topic, topic);
        return;
    } else if (positioning_type == 1) {
        _business_command->business_mode = TRACK_DR;
    }
    sendRouterTopicBuffer(1, topic, topic);
}

/**
 * @brief 设置返航点，从返航点开始记录行驶轨迹。
 *        收到返航指令后，停止记录轨迹。收到行动控制指令后开始返航。
 *
 * @param topic
 */
void ZmqApiHandler3::handlemanager2auto_return_point_set(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_return_point_set>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic << fmt::format(": flag: {}", struct_ptr.flag);
    if (!app_common::GeoUtils::is_location_ok(*_last_gps)) {
        _ec.ec_add(Ec409::_ERRORCODE_GPS);
        ApInfo("zmq_task") << "zmq recv " << topic
                           << ": gps_status: " << static_cast<std::int32_t>(_last_gps->locationState);
        sendRouterTopicBuffer(0, topic, topic);
        _task_manage_status->task_status = static_cast<unsigned char>(6);
        return;
    }
    _ec.ec_remove(Ec409::_ERRORCODE_GPS);

    ApInfo("zmq_task") << "zmq recv " << topic << ": start record track from "
                       << app_common::GeoUtils::str_gps(*_last_gps);

    try {
        // 根据协议要求：收到返航点设置消息后，清除之前记录，重新开始记录路径。
        if (_track_direct_recorder_return) {
            _track_direct_recorder_return.reset();
        }
        _track_direct_recorder_return = std::make_shared<app_common::WaypointDirectRecorder>(
            _track_direct_record_file_path, app_common::DirectRecorderMode::CREATE_TRUNC_APPEND);

        _ec.ec_remove(Ec409::_ERRORCODE_TRACK_RECORD);
    } catch (const std::runtime_error &e) {
        _ec.ec_add(Ec409::_ERRORCODE_TRACK_RECORD);
        ApInfo("zmq_task") << "zmq recv " << topic << ": recorder error: " << e.what();
        sendRouterTopicBuffer(0, topic, topic);
        return;
    } catch (...) {
        _ec.ec_add(Ec409::_ERRORCODE_TRACK_RECORD);
        ApInfo("zmq_task") << "zmq recv " << topic << ": recorder error";
        sendRouterTopicBuffer(0, topic, topic);
        return;
    }
    _return_point_status->return_point_flag = true;
    _cv_persistence.notify_one();
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::handlemanager2auto_return_action(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_return_action>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic
                       << fmt::format(": action_id: {}, return_type: {}", struct_ptr.action_id, struct_ptr.return_type);
    if (_business_command->business_mode != NONE) {
        _ec.ec_add(Ec409::_ERRORCODE_OTHER_TASK);
        ApInfo("zmq_task") << "there is already a task running, business_mode: " << _business_command->business_mode;
        sendRouterTopicBuffer(800, topic, topic);
        return;
    }
    _ec.ec_remove(Ec409::_ERRORCODE_OTHER_TASK);

    if (!_return_point_status->return_point_flag || nullptr == _track_direct_recorder_return) {
        ApInfo("zmq_task") << "zmq recv " << topic << ": return point not set";
        sendRouterTopicBuffer(801, topic, topic);
        return;
    }
    try {
        auto waypoints = _track_direct_recorder_return->read_all();
        _gpx_file_handler_for_track->gpx_file_new("return_road", "q", "z", waypoints, false);
        _track_direct_recorder_return = nullptr;
        _ec.ec_remove(Ec409::_ERRORCODE_TRACK_RECORD);
    } catch (const std::exception &e) {
        _ec.ec_add(Ec409::_ERRORCODE_TRACK_RECORD);
        ApInfo("zmq_task") << "zmq recv " << topic << ": recorder error: " << e.what();
        sendRouterTopicBuffer(0, topic, topic);
        _task_manage_status->task_status = static_cast<unsigned char>(6);
        return;
    } catch (...) {
        ApInfo("zmq_task") << "zmq recv " << topic << ": recorder error";
        sendRouterTopicBuffer(0, topic, topic);
        _task_manage_status->task_status = static_cast<unsigned char>(6);
        return;
    }

    _business_command->param.patrol_name = "return_road";
    _business_command->param.patrol_type = (std::int32_t)PatrolType::Return;
    _business_command->param.patrol_dest = "q";
    _business_command->param.patrol_loop = false;
    _business_command->param.command = PAUSED;
    _business_command->header.seq += 1; // 规划会根据这个 seq 来判断是否需要清除数据并重新规划
    _business_command->business_mode = TRACK_DR;
    _business_command->param.task_avoid_mode = 1;

    _task_manage_status->action_id = static_cast<unsigned char>(struct_ptr.action_id);
    _task_manage_status->task_type = static_cast<unsigned char>(4);
    _task_manage_status->task_status = static_cast<unsigned char>(1);
    _return_point_status->return_point_flag = false;
    _cv_persistence.notify_one();

    ApInfo("zmq_task") << "zmq recv " << topic << ": business command:"
                       << " business_mode: " << _business_command->business_mode
                       << " patrol_name: " << _business_command->param.patrol_name
                       << " patrol_dest: " << _business_command->param.patrol_dest
                       << " patrol_type: " << _business_command->param.patrol_type
                       << " patrol_loop: " << _business_command->param.patrol_loop;
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::handlemanager2auto_command_control(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_command_control>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic
                       << fmt::format(": action_id: {}, command: {}", struct_ptr.action_id, struct_ptr.commond);
    {
        _task_manage_status->action_id = static_cast<unsigned char>(struct_ptr.action_id);

        // TODO: 协议设计成了异步协议，很恼火:
        // 1. 是否需要判断各自任务状态？
        // 2. 是否需要判断任务是否匹配？
        // 3. 收到行动指令后，如果当前有行动存在，如何处理？
        if (1 == struct_ptr.commond) {
            if (!_enable_pre_autonomy_tasking && _vehicle_information->vehicle_act_state.drive_mode == 0) {
                _ec.ec_add(Ec409::_ERRORCODE_AUTONOMY);
                ApInfo("zmq_task") << "zmq recv " << topic << ": drive_mode: "
                                   << static_cast<std::int32_t>(_vehicle_information->vehicle_act_state.drive_mode)
                                   << ", enable_pre_autonomy_tasking: " << _enable_pre_autonomy_tasking;
                sendRouterTopicBuffer(0, topic, topic);
                _task_manage_status->task_status = static_cast<unsigned char>(6);
                return;
            }
            _ec.ec_remove(Ec409::_ERRORCODE_AUTONOMY);

            if (!app_common::GeoUtils::is_location_ok(*_last_gps)) {
                _ec.ec_add(Ec409::_ERRORCODE_GPS);
                ApInfo("zmq_task") << "zmq recv " << topic
                                   << ": gps_status: " << static_cast<std::int32_t>(_last_gps->locationState);
                sendRouterTopicBuffer(0, topic, topic);
                _task_manage_status->task_status = static_cast<unsigned char>(6);
                return;
            }
            _ec.ec_remove(Ec409::_ERRORCODE_GPS);

            if (_business_command->param.max_speed && _target_max_speed <= 1.0) {
                ApInfo("zmq_task") << "zmq recv " << topic << ": change max_speed from "
                                   << _business_command->param.max_speed << " to 5.0";
                _business_command->param.max_speed = 5.0;
                _target_max_speed = 5.0;
            }

            if (_task_manage_status->task_type == 1) {
                _business_command->param.command = RUNNING;
            } else if (_task_manage_status->task_type == 2) {
                _business_command->param.max_speed = _target_max_speed;
            } else if (_task_manage_status->task_type == 4) {
                _business_command->param.command = RUNNING;
            }
            _task_manage_status->task_status = static_cast<unsigned char>(2);
        } else if (2 == struct_ptr.commond) {
            _business_command->param.command = PAUSED;
            _task_manage_status->task_status = static_cast<unsigned char>(3);

        } else if (static_cast<int>(struct_ptr.commond) == 3) {
            _business_command->param.command = RUNNING;
            _task_manage_status->task_status = static_cast<unsigned char>(2);
        } else if (static_cast<int>(struct_ptr.commond) == 4) {
            _business_command->param.command = IDLE;
            _task_manage_status->task_status = static_cast<unsigned char>(4);

            // 清除过期未执行任务错误码
            _ec.ec_remove(Ec409::_ERRORCODE_AUTONOMY);
            _ec.ec_remove(Ec409::_ERRORCODE_GPS);
            _ec.ec_remove(Ec409::_ERRORCODE_OTHER_TASK);
            _ec.ec_remove(Ec409::_ERRORCODE_TRACK_WAYPOINTS_NUMBER);
            _ec.ec_remove(Ec409::_ERRORCODE_TRACK_WAYPOINTS_SAVE);
            _ec.ec_remove(Ec409::_ERRORCODE_TRACK_RECORD);
            _ec.ec_remove(Ec409::_ERRORCODE_EXPIRED_WAIT_EXECUTING);
        } else if (static_cast<int>(struct_ptr.commond) == 99) {
            _sensor_control->chassis_enable = 1; // g318临时调试
        } else if (static_cast<int>(struct_ptr.commond) == 100) {
            _sensor_control->chassis_enable = 0; // g318临时调试
        }
        _task_status_indicator->reset();
        _cv_persistence.notify_one();
    }
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::handlemanager2auto_speed_limit_config(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_speed_limit_config>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic << fmt::format(": speed_limit: {}", struct_ptr.speed_limit);
    if (static_cast<uint16_t>(struct_ptr.speed_limit) > 300) {
        ApInfo("zmq_task") << "zmq recv " << topic << fmt::format(": speed_limit: {}", struct_ptr.speed_limit)
                           << " is out of range [0, 300]m/s ";
        sendRouterTopicBuffer(0, topic, topic);
        return;
    }
    double speed_km = static_cast<double>(struct_ptr.speed_limit) / 10.0 * 3.6;
    if (speed_km < 1.0) {
        ApInfo("zmq_task") << "zmq recv " << topic << ": change speed from " << speed_km << " to 5.0";
        speed_km = 5.0;
    }

    _business_command->param.max_speed = speed_km;
    _target_max_speed = speed_km;
    if (is_follow(_business_command->business_mode)) {
        if (0 == _business_command->param.max_speed) { // just set an action, not started yet
            _business_command->param.max_speed = 0;
        } else {
            _business_command->param.max_speed = _target_max_speed;
        }
    } else {
        _business_command->param.max_speed = speed_km;
    }
    _cv_persistence.notify_one();
    sendRouterTopicBuffer(1, topic, topic);
}

int ZmqApiHandler3::parseVideoProfileData(const protocol_common::manager2auto_video_set &data) {
    if (static_cast<int>(data.resolution) == 1) {
        _video_profile->manual_width = 1280;
        _video_profile->manual_height = 720;
    } else {
        _video_profile->manual_width = 1920;
        _video_profile->manual_height = 1080;
    }

    switch (static_cast<uint>(data.bps)) {
        case 1:
            _video_profile->manual_bps = 256;
            break;
        case 2:
            _video_profile->manual_bps = 512;
            break;
        case 3:
            _video_profile->manual_bps = 1024;
            break;
        case 4:
            _video_profile->manual_bps = 2048;
            break;
        default:
            break;
    }

    bool is_split = (data.splicing_flag == 1);

    if (is_split) {
        _video_profile->split_area_parames.clear();

        for (const auto &element : data.video_splicing) {
            ::ara::adsfi::AppVideoSplitParam point_param_;

            // 映射通道名称
            auto it = cameraTypeMap.find(static_cast<int>(element.video_channel));
            if (it != cameraTypeMap.end()) {
                point_param_.channel_name = it->second;
            }
            if (point_param_.channel_name.empty()) {
                continue; // 忽略无效通道
            }

            point_param_.view_area_points.left_top_point_x = element.video_position.top_left_x;
            point_param_.view_area_points.left_top_point_y = element.video_position.top_left_y;
            point_param_.view_area_points.right_bottom_point_x = element.video_position.bottom_right_x;
            point_param_.view_area_points.right_bottom_point_y = element.video_position.bottom_right_y;

            point_param_.crop_area_points.left_top_point_x = element.video_crop_position.top_left_x;
            point_param_.crop_area_points.left_top_point_y = element.video_crop_position.top_left_y;
            point_param_.crop_area_points.right_bottom_point_x = element.video_crop_position.bottom_right_x;
            point_param_.crop_area_points.right_bottom_point_y = element.video_crop_position.bottom_right_y;

            _video_profile->split_area_parames.push_back(point_param_);
        }

        _video_profile->video_third_person = 0;
        auto iter = std::find_if(
            _video_profile->split_area_parames.begin(), _video_profile->split_area_parames.end(),
            [](const auto &c) { return c.channel_name == "front_night" || c.channel_name == "back_night"; });

        if (iter != _video_profile->split_area_parames.end()) {
            if ("front_night" == iter->channel_name) {
                _video_profile->video_third_person = 1;
                // SHMParamApi::Instance()->AddModuleParam("global");
                // SHMParamApi::Instance()->SetIntModuleParam("mdc_camera_nv_camera_pos",
                //                                            1); // 0:主，即前。 1:从，即后。默认为0。
            } else {
                // _video_profile->video_third_person = 0;
                // SHMParamApi::Instance()->AddModuleParam("global");
                // SHMParamApi::Instance()->SetIntModuleParam("mdc_camera_nv_camera_pos",
                //                                            0); // 0:主，即前。 1:从，即后。默认为0。
            }
            iter->channel_name = "night";
        }

        // 检验提供的参数是否合理
        for (const auto &p : _video_profile->split_area_parames) {
            int view_w =
                static_cast<int>(p.view_area_points.right_bottom_point_x - p.view_area_points.left_top_point_x);
            int view_h =
                static_cast<int>(p.view_area_points.right_bottom_point_y - p.view_area_points.left_top_point_y);

            if (view_w <= 0 || view_h <= 0) {
                ApError("zmq_task") << "显示区域无效 (w=" << view_w << ", h=" << view_h << ")，无法拼接";
                return 3;
            }
        }
    }

    return 1;
}

void ZmqApiHandler3::handlemanager2auto_emergency_brake(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_emergency_brake>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic
                       << ": emergency_top: " << static_cast<std::int32_t>(struct_ptr.emergancy_stop);
    _business_command->estop_flag = static_cast<int>(struct_ptr.emergancy_stop);
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::handlemanager2auto_video_enable(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_video_enable>(_zmq_socket, topic, 10000);

    if (struct_ptr.video_enable > 1) {
        ApInfo("zmq_task") << "zmq recv " << topic << fmt::format(": enabled: {}", struct_ptr.video_enable)
                           << " is out of range [0, 1] ";
        sendRouterTopicBuffer(0, topic, topic);
        return;
    }

    _video_rtp->push_enabled = struct_ptr.video_enable;
    _video_rtp->peer_port = struct_ptr.video_target_port;
    in_addr addr{};
    addr.s_addr = struct_ptr.video_target_ip;
    _video_rtp->peer_host = inet_ntoa(addr);
    _video_rtp->ssrc = 0;

    ApInfo("zmq_task") << "zmq recv " << topic
                       << fmt::format(" , enable: {}, target_ip: {}, target_port: {}", _video_rtp->push_enabled,
                                      _video_rtp->peer_host, _video_rtp->peer_port);
    _cv_persistence.notify_one();
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::handlemanager2auto_video_set(const std::string &topic) {
    auto struct_ptr = _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_video_set>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic
                       << fmt::format(", resolution: {}, bps: {}, fps: {}, splicing_flag: {}, video_num: {}",
                                      struct_ptr.resolution, struct_ptr.bps, struct_ptr.fps, struct_ptr.splicing_flag,
                                      struct_ptr.video_num);

    for (const auto &splice : struct_ptr.video_splicing) {
        ApInfo("zmq_task") << "zmq recv " << topic
                           << ": video_channel: " << static_cast<std::int32_t>(splice.video_channel)
                           << ", video_position: " << splice.video_position.top_left_x << ","
                           << splice.video_position.top_left_y << "," << splice.video_position.bottom_right_x << ","
                           << splice.video_position.bottom_right_y
                           << ", crop_position: " << splice.video_crop_position.top_left_x << ","
                           << splice.video_crop_position.top_left_y << "," << splice.video_crop_position.bottom_right_x
                           << "," << splice.video_crop_position.bottom_right_y;
    }
    int ret_result = parseVideoProfileData(struct_ptr);
    _cv_persistence.notify_one();

    sendRouterTopicBuffer(ret_result, topic, topic);
}
void ZmqApiHandler3::handlemanager2auto_video_assist(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_video_assist>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic
                       << fmt::format(
                              ", remote_assist_line: {}, remote_warning_info: {}, auto_assist_line: {},drivable_area{}",
                              struct_ptr.remote_assist_line, struct_ptr.remote_warning_info,
                              struct_ptr.auto_assist_line, struct_ptr.drivable_area);

    _video_profile->remote_control_assist = static_cast<int>(struct_ptr.remote_assist_line);
    _video_profile->remote_control_alarm = static_cast<int>(struct_ptr.remote_warning_info);
    _video_profile->auto_control_assist = static_cast<int>(struct_ptr.auto_assist_line);
    _video_profile->drivable_area = static_cast<int>(struct_ptr.drivable_area);
    state_write(*_video_profile);
    sendRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::handlemanager2auto_data_logger(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct->zmqRecvStruct<protocol_common::manager2auto_data_logger>(_zmq_socket, topic, 10000);
    ApInfo("zmq_task") << "zmq recv " << topic << fmt::format(", data_logger: {}, ", struct_ptr.data_logger);
    auto trigger_flag = static_cast<uint8_t>(struct_ptr.data_logger);
    if (trigger_flag > 1 || trigger_flag < 0) {
        ApError("zmq_task") << "data_logger: " << static_cast<uint8_t>(struct_ptr.data_logger)
                            << " is out of range [0, 1]";
        sendRouterTopicBuffer(0, topic, topic);
        return;
    } else if (trigger_flag == 1) {
        _business_command->record_trigger_flag = trigger_flag;
        _record_last_trigger =
            std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
    } else if (trigger_flag == 0) {
        _business_command->record_trigger_flag = trigger_flag;
    }
    sendRouterTopicBuffer(1, topic, topic);
}

// TODO  后续根据实际需求完善
void ZmqApiHandler3::handlemanager2loc_ins_control(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct_ins->zmqRecvStruct<protocol_common::manager2loc_ins_control>(_zmq_socket_ins, topic, 10000);

    ApInfo("zmq_task") << "zmq recv " << topic << fmt::format(", ins_control: {}, ", struct_ptr.ins_control);

    try {
        if (struct_ptr.ins_control == 1) {
            int result = SendCanMessageNative();

            if (result == 0) {
                ApInfo("zmq_task") << "recv CAN报文发送成功 (Native)";
                _ec.ec_remove(Ec409::_ERRORCODE_CAN_SEND);
            } else {
                // result 包含具体的错误代码 (-1, -2 等)
                ApError("zmq_task") << "recv CAN报文发送失败，内部错误码: " << result;
                _ec.ec_add(Ec409::_ERRORCODE_CAN_SEND, "惯导寻北指令发送失败！");
                sendInsRouterTopicBuffer(0, topic, topic);
                return;
            }

        } else if (struct_ptr.ins_control == 0) {
            ApInfo("zmq_task") << "ins_control == 0, no action needed";
        } else {
            ApError("zmq_task") << "ins_control value invalid: " << struct_ptr.ins_control;
            sendInsRouterTopicBuffer(0, topic, topic);
            return;
        }
    } catch (...) {
        ApError("zmq_task") << "Unexpected exception in handlemanager2loc_ins_control";
        _ec.ec_add(Ec409::_ERRORCODE_CAN_SEND, "惯导寻北指令发送失败！");
        sendInsRouterTopicBuffer(0, topic, topic);
        return;
    }
    sendInsRouterTopicBuffer(1, topic, topic);
}

// TODO  后续根据实际需求完善
void ZmqApiHandler3::handlemanager2loc_loc_mode(const std::string &topic) {
    auto struct_ptr =
        _zmq_construct_ins->zmqRecvStruct<protocol_common::manager2loc_loc_mode>(_zmq_socket_ins, topic, 10000);

    ApInfo("zmq_task") << "zmq recv " << topic << fmt::format(", loc_mode: {}, ", struct_ptr.loc_mode);

    if (struct_ptr.loc_mode > 255 || struct_ptr.loc_mode < 0) {
        ApError("zmq_task") << "zmq recv:   loc_mode out of range (0-255): " << struct_ptr.loc_mode;
        sendInsRouterTopicBuffer(0, topic, topic);
        return;
    }
    auto loc_mode = static_cast<uint8_t>(struct_ptr.loc_mode);

    // 检查保留位 Bit6 (0x40 = 0100 0000)这是预留位 是否被设置
    if ((loc_mode & 0x40) != 0) {
        ApError("zmq_task") << "zmq recv:  Invalid loc_mode: Reserved Bit6 is set! val: " << (int)loc_mode;
        sendInsRouterTopicBuffer(0, topic, topic);
        return;
    }

    if ((loc_mode & 0x80) != 0) {
        // 如果 Bit7 置位，强制将其他位置 0，确保协议一致性
        loc_mode = 0x80;
        ApInfo("zmq_task") << "zmq recv: Bit7 is set, forcing logic to Adaptive mode only.";
    }

    _business_command->loc_control_mode = loc_mode;
    _cv_persistence.notify_one();
    sendInsRouterTopicBuffer(1, topic, topic);
}

void ZmqApiHandler3::persistence_thread_func() {
    while (true) {
        try {
            // 定义局部快照变量
            ara::adsfi::BusinessCommand   temp_business;
            ara::adsfi::AppVideoProfile   temp_video;
            ara::adsfi::VideoRTPParameter temp_rtp;
            ReturnPointStatus             temp_return;
            ara::adsfi::TaskManageStatus  temp_task;

            {
                std::unique_lock<std::mutex> lck(_mtx);

                // 等待 1 秒 (心跳)，或被 notify 唤醒 (数据变更)
                _cv_persistence.wait_for(lck, std::chrono::seconds(1));

                if (_business_command)
                    temp_business = *_business_command;
                if (_video_profile)
                    temp_video = *_video_profile;
                if (_video_rtp)
                    temp_rtp = *_video_rtp;
                if (_return_point_status)
                    temp_return = *_return_point_status;
                if (_task_manage_status)
                    temp_task = *_task_manage_status;
            }

            state_write(temp_business);
            state_write(temp_video);
            state_write(temp_rtp);
            state_write(temp_return);
            state_write(temp_task);

        } catch (const std::exception &e) {
            ApError("zmq_task") << "[PersistenceThread] Exception caught: " << e.what();

            std::this_thread::sleep_for(std::chrono::seconds(1));
        } catch (...) {
            ApError("zmq_task") << "[PersistenceThread] Unknown exception caught!";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}
