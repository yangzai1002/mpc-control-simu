#ifndef EC409_HPP
#define EC409_HPP

#include <map>
#include <mutex>
#include <thread>

#include "faulthandle_api.hpp"

class Ec409 {
public:
    Ec409() = default;

    std::uint32_t ec_add(const std::string& ec) {
        std::lock_guard<std::mutex> lg(_mtx);
        auto                        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            auto r = _ec_map.insert({ec, 0});
            if (!r.second) {
                return 0;
            }
            ptr = r.first;
        }

        ++ptr->second;
        if (ptr->second % 20 == 1) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec);
        }
        return ptr->second;
    }

    std::uint32_t ec_add(const std::string& ec, const std::string& ec_detail) {
        std::lock_guard<std::mutex> lg(_mtx);
        auto                        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            _ec_map[ec] = 0;
        }
        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            return 0;
        }

        ++ptr->second;
        if (ptr->second % 20 == 1) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec, ec_detail);
        }
        return ptr->second;
    }

    void ec_remove(const std::string& ec) {
        std::lock_guard<std::mutex> lg(_mtx);
        if (_ec_map.find(ec) != _ec_map.end()) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec);
            _ec_map.erase(ec);
        }
    }

    void ec_clear() {
        std::lock_guard<std::mutex> lg(_mtx);
        for (const auto& ec : _ec_map) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec.first);
        }
        _ec_map.clear();
    }

public:
    static const constexpr char* _ERRORCODE_CONFIG = "4092001";                      // 读取配置文件失败
    static const constexpr char* _ERRORCODE_INIT = "4092002";                        // 初始化失败
    static const constexpr char* _ERRORCODE_MQTT_CONNECT = "4092011";                // 连接失败
    static const constexpr char* _ERRORCODE_MQTT_SUBSCRIBE = "4092012";              //订阅失败
    static const constexpr char* _ERRORCODE_MQTT_PUBLISH = "4092013";                //发布失败
    static const constexpr char* _ERRORCODE_ZMQ_LISTEN = "4092021";                  // ZMQ 监听失败
    static const constexpr char* _ERRORCODE_ZMQ_CONNECT = "4092022";                 // ZMQ 连接失败
    static const constexpr char* _ERRORCODE_ZMQ_SEND = "4092023";                    // ZMQ 发送失败
    static const constexpr char* _ERRORCODE_ZMQ_RECV = "4092024";                    // ZMQ 接收失败
    static const constexpr char* _ERRORCODE_ZMQ_DESERIALIZE = "4092025";             // ZMQ 反序列化失败
    static const constexpr char* _ERRORCODE_CAN_SEND = "4092026";                    // CAN 发送失败
    static const constexpr char* _ERRORCODE_REMOTE_CONTROL_RECV_TIMEOUT = "4092041"; // 遥控数据接收超时
    static const constexpr char* _ERRORCODE_REMOTE_CONTROL_DELAY_ACCUM = "4092042";  // 遥控数据接收延迟累积
    static const constexpr char* _ERRORCODE_AUTONOMY = "4092031"; // 未进入自主状态，或者不允许在非自主状态下发任务
    static const constexpr char* _ERRORCODE_GPS = "4092032"; // 定位状态异常，或者不允许在非固定解状态下发任务
    static const constexpr char* _ERRORCODE_OTHER_TASK = "4092033";             // 已有其它任务在运行
    static const constexpr char* _ERRORCODE_TRACK_WAYPOINTS_NUMBER = "4092034"; // 循迹任务路径点数太少
    static const constexpr char* _ERRORCODE_TRACK_WAYPOINTS_SAVE = "4092035";   // 循迹任务路径点保存失败
    static const constexpr char* _ERRORCODE_TRACK_RECORD = "4092036";           // 轨迹文件操作失败
    static const constexpr char* _ERRORCODE_EXPIRED_WAIT_EXECUTING = "4092037"; // 任务下发后长时间未开始执行
    static const constexpr char* _ERRORCODE_VIDEO_PERSIST = "4092038";          // 状态保持配置异常

private:
    std::mutex                           _mtx;
    std::map<std::string, std::uint32_t> _ec_map;
};
#endif