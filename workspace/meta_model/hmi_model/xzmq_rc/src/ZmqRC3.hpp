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
#include "PerfIndicators.hpp"
#include "StringUtils.hpp"
#include "ZmqConstruct.hpp"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ap_log/ap_log_interface.h"

#pragma pack(push, 16)

class ZmqRC3 {
public:
    ZmqRC3();
    ~ZmqRC3() = default;

    void on_data_change();
    void values(ara::adsfi::AppRemoteControl& remote_control);

private:
    void remote_loop();
    void persistence_loop();
    void parseRemoteControlData(std::shared_ptr<protocol_common::manager2chassis_control> data);

    // 变更检测逻辑
    bool check_and_notify_change(const ara::adsfi::AppRemoteControl& current);

    void state_read(ara::adsfi::AppRemoteControl& control);
    void state_write(const ara::adsfi::AppRemoteControl& control);

private:
    std::mutex   _mtx;
    std::int64_t _over_time_tick;
    std::int32_t _reset_time_fault;
    std::int64_t _recover_start_time = 0;
    std::int64_t _delay_recover_start_time = 0;
    std::int64_t _last_tick_time = 0;
    std::int32_t _last_tick_value = -1;

    bool _over_time_tick_flag = false;
    bool _delay_flag = false;

    std::vector<int64_t> _timestamps_buffer;
    std::string          _remote_control_endpoint;
    int                  _speed_limit = 0;

    // 本地管理的文件路径
    std::string _json_file_path;

private:
    Ec409                                         _ec;
    std::shared_ptr<ZmqConstruct>                 _zmq_construct;
    std::shared_ptr<zmq::socket_t>                _zmq_socket_sub;
    std::shared_ptr<ara::adsfi::AppRemoteControl> _remote_control;

    // 持久化线程同步
    std::condition_variable _cv_persistence;
    std::mutex              _mtx_persistence;
    std::atomic<bool>       _data_changed{false};

    struct CriticalState {
        int gear = -1;
        int remotectrl_flag = -1;
        int estop_flag = -1;
        int speed_limit = -1;
        int security_assist_enabled = -1;
    } _last_parsed_state;
};
#pragma pack(pop)
#endif
