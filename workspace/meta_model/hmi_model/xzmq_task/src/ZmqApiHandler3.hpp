#ifndef ZMQ_API_HANDLER_HPP
#define ZMQ_API_HANDLER_HPP

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <thread>

#include "Ec409.hpp"
#include "FenceFileController.hpp"
#include "FenceRecorder.hpp"
#include "GpxFileHandler.hpp"
#include "MapFileController.hpp"
#include "MapPlanController.hpp"
#include "MapRecorder.hpp"
#include "PerceptionFinalObjects.h"
#include "PerceptionTargetObj.h"
#include "PerfIndicators.hpp"
#include "Persistence.hpp"
#include "StringUtils.hpp"
#include "TrackFileController.hpp"
#include "TrackRecorder.hpp"
#include "WaypointDirectRecorder.hpp"
#include "ZmqConstruct.hpp"
#include "adsfi/impl_type_locationinfo.h"
#include "ara/adsfi/impl_type_appstatusmessage.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_taskexecutestatus.h"
#include "ara/adsfi/impl_type_taskmanagestatus.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_vehiclesensorcontrol.h"
#include "ara/adsfi/impl_type_videortpparameter.h"
#include "ap_log/ap_log_interface.h"

#pragma pack(push, 16)

class ZmqApiHandler3 {
public:
    ZmqApiHandler3();

    ~ZmqApiHandler3() {
        _stopped = true;
        if (_thread.joinable()) {
            _thread.join();
        }
    }

    void on_data_change(const ara::adsfi::MsgHafLocation&       gps,                 // 当前位置
                        const ara::adsfi::TaskExecuteStatus&    task_execute_status, // 规划状态（自主）
                        const ara::adsfi::VehicleInformation&   vehicle_information,
                        const ara::adsfi::MsgHafFusionOutArray& detected_objects,
                        const ara::adsfi::MsgHafFusionOutArray& target_obj);

    void values(ara::adsfi::BusinessCommand& business_command);

    void values(ara::adsfi::VehicleSensorControl& sensor_control);

    void values(ara::adsfi::AppVideoProfile& video_profile);

    void values(ara::adsfi::VideoRTPParameter& rtp_parameter);

    void values(ara::adsfi::TaskManageStatus& task_manage_status);

private:
    enum COMMAND { IDLE = 0, RUNNING, PAUSED };

    enum PLANNING_TYPE {
        NONE = 0,
        TRACK = 1,
        ARRIVE = 2,
        FOLLOW = 3,
        POSE_ADJUST = 4,
        HEADING_ADJUST = 5,
        FULL_POSE_ADJUST = 6,
        TRACK_DR = 11,
        FOLLOW_DR = 31,
    };

    static bool is_track(std::int32_t mode) {
        return (TRACK == mode || TRACK_DR == mode);
    }

    static bool is_follow(std::int32_t mode) {
        return (FOLLOW == mode || FOLLOW_DR == mode);
    }

    static bool is_arrive(std::int32_t mode) {
        return (ARRIVE == mode);
    }

    static bool is_track_or_arrive(std::int32_t mode) {
        return (is_track(mode) || is_arrive(mode));
    }

    void loop();
    void loop_ins();

    void sendRouterTopicBuffer(const uint32_t ret_result, const std::string& topic,
                               const std::string& res_topic) noexcept(false);
    void sendInsRouterTopicBuffer(const uint32_t ret_result, const std::string& topic,
                                  const std::string& res_topic) noexcept(false);
    void handleTargetPointsAction(const std::string& topic, bool is_test_topic = false) noexcept(false);

    void handleTargetFollowAction(const std::string& topic) noexcept(false);

    void handlemanager2auto_aciton_config(const std::string& topic) noexcept(false);
    void handlemanager2auto_maneuvering_strategy(const std::string& topic) noexcept(false); // 机动策略
    void handlemanager2auto_perception_strategy(const std::string& topic) noexcept(false);  // 感知策略
    void handlemanager2auto_positioning_usage(const std::string& topic) noexcept(false);    // 定位策略
    void handlemanager2auto_return_point(const std::string& topic) noexcept(false);
    void handlemanager2auto_return_point_set(const std::string& topic) noexcept(false);
    void handlemanager2auto_return_action(const std::string& topic) noexcept(false);

    void handlemanager2auto_command_control(const std::string& topic) noexcept(false);
    void handlemanager2auto_speed_limit_config(const std::string& topic) noexcept(false);
    void handlemanager2auto_emergency_brake(const std::string& topic) noexcept(false);
    void handlemanager2auto_video_enable(const std::string& topic) noexcept(false);
    void handlemanager2auto_video_set(const std::string& topic) noexcept(false);
    void handlemanager2auto_video_assist(const std::string& topic) noexcept(false);
    void handlemanager2auto_data_logger(const std::string& topic) noexcept(false);

    void handlemanager2loc_ins_control(const std::string& topic) noexcept(false); // 惯导相关控制
    void handlemanager2loc_loc_mode(const std::string& topic) noexcept(false);    // 定位模式控制

    //解析视频配置切换数据
    int parseVideoProfileData(const protocol_common::manager2auto_video_set& data);

    //解析视频辅助数据
    // void parseVideoAssistData(std::shared_ptr<protocol_common::manager2auto_video_assist> data);

    //获取相关topic对应的id_msg_str_
    std::string getIdentityMsgStr(const std::string& topic);

    // 获取惯导相关topic对应的id_msg_str_
    std::string getInsIdentityMsgStr(const std::string& topic);

private:
    std::atomic_bool _stopped{false};
    std::mutex       _mtx;
    std::thread      _thread;

private: // 循迹相关
    bool         _enable_pre_autonomy_tasking;
    std::int32_t _min_waypoints = 0;
    std::string  _key_file_storage_path;
    std::string  _task_endpoint;
    std::string  _loc_control_endpoint;

private: // 视频切换
private: // 全局路径相关
    // 必经点
    static constexpr const char* THROUGH = "THROUGH";
    // 禁行点
    static constexpr const char* FORBIDDEN = "FORBIDDEN";

private:
    Ec409                          _ec;
    std::shared_ptr<ZmqConstruct>  _zmq_construct;
    std::shared_ptr<zmq::socket_t> _zmq_socket;
    std::shared_ptr<zmq::socket_t> _zmq_socket_sub;

    std::shared_ptr<ZmqConstruct>  _zmq_construct_ins;
    std::shared_ptr<zmq::socket_t> _zmq_socket_ins;

    std::shared_ptr<ara::adsfi::VehicleInformation>   _vehicle_information;
    std::shared_ptr<ara::adsfi::MsgHafLocation>       _last_gps;
    std::shared_ptr<CommonPoint3f>                    _return_point;
    std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> _target_obj;
    std::shared_ptr<ara::adsfi::BusinessCommand>      _business_command; // 业务消息
    std::shared_ptr<ara::adsfi::TaskExecuteStatus>    _last_task_status; // 任务执行状态
    std::shared_ptr<ara::adsfi::AppVideoProfile>      _video_profile;    // 视频配置
    std::shared_ptr<ara::adsfi::VehicleSensorControl> _sensor_control;
    std::shared_ptr<ara::adsfi::TaskManageStatus>     _task_manage_status;
    std::shared_ptr<ara::adsfi::VideoRTPParameter>    _video_rtp;

    std::shared_ptr<app_common::GpxFileHandler>         _gpx_file_handler_for_track;
    std::shared_ptr<app_common::WaypointDirectRecorder> _track_direct_recorder_return;
    std::string                                         _track_direct_record_file_path;

    std::shared_ptr<ReturnPointStatus> _return_point_status;

    std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> _record_last_trigger;

    // 全局车速配置
    double _target_max_speed = 0.0;

    uint8_t                            current_task_id = -1;
    std::map<std::string, std::string> topic_maps;
    std::map<std::string, std::string> topic_maps_ins;

    std::shared_ptr<app_common::TimeWindowIndicator<std::int32_t>> _task_status_indicator;
    std::chrono::steady_clock::time_point _last_state_save_time = std::chrono::steady_clock::time_point::min();
    std::condition_variable               _cv_persistence; // 用于唤醒持久化线程的条件变量
    void                                  persistence_thread_func();
};
#pragma pack(pop)
#endif
