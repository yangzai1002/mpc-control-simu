#ifndef __XMID_TRACK_SEND_BY_ZMQ_HPP__
#define __XMID_TRACK_SEND_BY_ZMQ_HPP__
#include <arpa/inet.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <zmq.hpp>

#include "DatetimeUtil.hpp"
#include "FaultManager.hpp"
#include "StringUtils.hpp"
#include "XMessageSend.h"
#include "ZmqHelper.h"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehiclesensorcontrol.h"
#include "config_loader/custom_stack.h"
#include "log/log.h"
#include "monitor/faulthandle_api.hpp"
#include "proto/Auto2Manager.h"

#define _ERRORCODE_CONFIG_READ "1062001"          // 配置参数读取失败
#define _ERRORCODE_TIMEOUT_ERROR "1062002"        // 上游超时
#define _ERRORCODE_DEVICE_CONNECT_ERROR "1062005" // ZMQ 建链失败
#define _ERRORCODE_SEND_FAIL "1062006"            // 发送失败

class XMidTrackSendByZMQDataInteract {
public:
    ~XMidTrackSendByZMQDataInteract() {
        stop_periodic_send();
    }

    void Init(const std::string &endpoint, const std::string &topic, int32_t timeout) {
        if (initialized_) {
            return;
        }

        topic_name_ = topic;
        timeout_ms_.store(static_cast<int64_t>(timeout), std::memory_order_relaxed);

        running_ = false;
        initialized_ = false;

        try {
            std::cout << "yrf----开始进行ZMQ初始化..." << std::endl;
            // 初始化ZMQ
            int  threading_pool_num = 2;
            auto zmq_wrapper = std::make_shared<ZmqConstruct>(threading_pool_num);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            auto publisher = zmq_wrapper->createPublisher(endpoint, true);
            bool conn_ok = static_cast<bool>(publisher);
            if (!conn_ok) {
                _fault.add(_ERRORCODE_DEVICE_CONNECT_ERROR, "创建ZMQ publisher失败: ");
                ApInfo("vehicle_chassis_send") << "Failed to create ZMQ publisher: "
                                               << "上报故障码: " << _ERRORCODE_DEVICE_CONNECT_ERROR;
                throw std::runtime_error("createPublisher failed");
            } else {
                _fault.remove(_ERRORCODE_DEVICE_CONNECT_ERROR);
            }

            if (!publisher) {
                ApError("vehicle_chassis_send") << "Failed to create ZMQ publisher";
                return;
            }
            std::cout << "yrf----ZMQ初始化成功" << std::endl;

            handler_ = std::make_shared<XMessageSend>(zmq_wrapper, publisher);

            // 初始化消息头
            initMessageHeader();

            last_valid_tp_ = std::chrono::steady_clock::now();

            running_ = true;
            periodic_thread_ = std::thread(&XMidTrackSendByZMQDataInteract::periodic_send_loop, this);

            initialized_ = true;
            ApInfo("vehicle_chassis_send") << "ZMQ sender initialized successfully";

            // 如果初始化成功，清除配置读取相关故障（若之前上报过）
            _fault.remove(_ERRORCODE_CONFIG_READ);
        } catch (const std::exception &e) {
            running_ = false;
            // 记录为设备连接失败
            _fault.add(_ERRORCODE_DEVICE_CONNECT_ERROR, e.what());
            ApInfo("vehicle_chassis_send")
                << "ZMQ initialization failed: " << e.what() << " 上报故障码: " << _ERRORCODE_DEVICE_CONNECT_ERROR;
            return;
        }
    }

    void updateSendMsg(const ara::adsfi::VehicleActControl &act_ctl, const ara::adsfi::VehicleSensorControl &sensor_ctl,
                       const ara::adsfi::FaultData &fault_data) {
        // 使用上游 header.time 判断是否为新的数据，若是则更新 last_valid_tp_
        auto double_time_this = app_common::DatetimeUtil::double_time_from_sec_nsec(act_ctl.header.time);
        if (double_time_this > 0) {
            constexpr double EPS = 1e-6; // 1 微秒级变化就认
            if (std::fabs(double_time_this - last_upstream_time_) > EPS) {
                last_upstream_time_ = double_time_this;
                last_valid_tp_ = std::chrono::steady_clock::now();
            }
        }

        std::lock_guard<std::mutex> lock(mutex_);
        if (isLinkExpired()) {
            // msg_.steering_req = 0;
            msg_.speed_req = 0;
            // msg_.gear = 0;
            _fault.add(_ERRORCODE_TIMEOUT_ERROR, "接收上游数据time 超时");
            ApInfo("vehicle_chassis_send") << "ryf--------接收上游数据time 超时啦~";
            return;
        } else {
            _fault.remove(_ERRORCODE_TIMEOUT_ERROR);
        }

        // 更新控制模式（仅在变化时设置）
        if (last_auto_mode_ != sensor_ctl.chassis_enable) {
            last_auto_mode_ = sensor_ctl.chassis_enable;
            if (last_auto_mode_ == 0) {
                // 3:纯自主
                msg_.control_mode_req = 3;
            } else {
                // 1：纯遥控
                msg_.control_mode_req = 1;
            }
            ApInfo("vehicle_chassis_send") << "Auto mode changed: " << static_cast<int>(sensor_ctl.chassis_enable);
        }

        // 更新档位
        if (last_shift_position_ != act_ctl.lon_control.shift_position) {
            last_shift_position_ = act_ctl.lon_control.shift_position;
            msg_.gear = act_ctl.lon_control.shift_position;
            ApInfo("vehicle_chassis_send") << "Gear changed: " << static_cast<int>(act_ctl.lon_control.shift_position);
        }

        if (last_streeing_mode_req_ != act_ctl.lat_control.steer_control_mode) {
            last_streeing_mode_req_ = act_ctl.lat_control.steer_control_mode;
            msg_.steering_mode_req = static_cast<uint8_t>(act_ctl.lat_control.steer_control_mode);
            ApInfo("vehicle_chassis_send")
                << "Steering mode changed: " << static_cast<uint8_t>(act_ctl.lat_control.steer_control_mode);
        }
        msg_.centrsteer_req = static_cast<int8_t>(act_ctl.lat_control.target_curvature); // -1右转，0-默认，1-左转

        // 更新消息内容
        msg_.emergency_breaking = (act_ctl.lon_control.emergency_mode == 1 || fault_data.fault_level >= 3) ? 1 : 0;
        ApInfo("vehicle_chassis_send") << "ryf--急停状态: " << static_cast<int>(msg_.emergency_breaking);
        msg_.control_mode = 1;
        msg_.chassis_enable = 1;

        // 转换并设置转向和速度（注意字节序）
        int16_t steering_req = htons(static_cast<int16_t>(std::round(act_ctl.lat_control.target_curvature / 0.001f)));
        memcpy(&msg_.steering_req, &steering_req, sizeof(steering_req));
        int16_t speed_req = htons(static_cast<int16_t>(std::round(act_ctl.lon_control.target_speed / 0.1f)));
        memcpy(&msg_.speed_req, &speed_req, sizeof(speed_req));
    }

    // 断链标志接口
    void setLinkExpired(bool v) {
        link_expired_.store(v, std::memory_order_relaxed);
    }
    bool isLinkExpired() const {
        return link_expired_.load(std::memory_order_relaxed);
    }

private:
    // 初始化消息头（把 msg_ 各个字段设为默认值）
    void initMessageHeader() {
        msg_.header.message_id = 0x61230100;
        msg_.header.message_length = sizeof(msg_) - 8;
        msg_.header.client_id = 0x01;
        msg_.header.session_id = 0x01;
        msg_.header.protocol_version = 0x01;
        msg_.header.interface_version = 0x01;
        msg_.header.message_type = 0x01;
        msg_.header.return_code = 0x00;
        msg_.heart = 0;

        msg_.control_mode = 0;
        msg_.gear = 0;
        msg_.steering = 0;
        msg_.throttle_braking = 0;
        msg_.steering_mode_req = 0;
        msg_.centrsteer_req = 0;
        msg_.steering_req = 0;
        msg_.speed_req = 0;
        msg_.emergency_breaking = 0;
        msg_.chassis_enable = 0;
        msg_.control_mode_req = 0;
        msg_.engine_control = 0;
        msg_.engine_power_mode = 0;
        msg_.speed_limit = 0;
        msg_.high_voltage_power = 1;
        msg_.motor_mode = 0;
        msg_.run_mode = 0;
        msg_.parking_mode = 0;
        msg_.parking_brake = 0;
        msg_.water_pump_control1 = 0;
        msg_.water_pump_control2 = 0;
        msg_.fan_control = 0;
        msg_.low_beams = 0;
        msg_.high_beams = 0;
        msg_.leftturn_signals = 0;
        msg_.rightturn_signals = 0;
        msg_.hazard_lights = 0;
        msg_.double_flash = 0;
        msg_.horn = 0;
        msg_.drainage_pump = 0;
        msg_.radar_power = 1;
        msg_.lidar_power = 1;
        msg_.night_vision_power = 1;
        msg_.load_power = 2;
        msg_.debug_equipment = 1;
        msg_.protection = 0;
        msg_.error_reset = 0;
        msg_.powerbattery_heating = 0;
        msg_.mains_output = 0;
        msg_.battery_output = 0;
        msg_.tail.payload_checksum = 0;
    }

    void sendData() {
        auto    now_tp = std::chrono::steady_clock::now();
        int64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_tp - last_valid_tp_).count();
        int64_t timeout_val = timeout_ms_.load(std::memory_order_relaxed);
        bool    new_expired = elapsed_ms > timeout_val;
        link_expired_.store(new_expired, std::memory_order_relaxed);

        protocol_common::manager2chassis_control msg_to_send;
        {
            std::lock_guard<std::mutex> lk(mutex_);
            if (msg_.heart > 254)
                msg_.heart = 0;
            msg_.heart++;
            msg_to_send = msg_;
        }

        if (msg_to_send.steering_req != 0.0 || msg_to_send.speed_req != 0.0) {
            ApInfo("vehicle_chassis_send")
                << "往底盘下发转向请求: " << static_cast<int16_t>(ntohs(msg_to_send.steering_req)) * 0.001f << "\n"
                << "往底盘下发速度请求: " << static_cast<int16_t>(ntohs(msg_to_send.speed_req)) * 0.1f << "\n"
                << "往底盘下发档位请求: " << static_cast<int>(msg_to_send.gear);
        }

        if (msg_to_send.steering_mode_req == 1) {
            ApInfo("vehicle_chassis_send")
                << "往底盘下发中心转向模式: " << static_cast<uint8_t>(msg_to_send.steering_mode_req) << "\n"
                << "期望的转向请求(-1 - 右转, 0 - 默认, 1 - 左转) : " << static_cast<int>(msg_to_send.centrsteer_req);
        }

        bool send_ok = true;
        if (!handler_) {
            ApError("vehicle_chassis_send") << "Handler not initialized";
            send_ok = false;
        } else {
            try {
                send_ok = handler_->sendVehicleData(topic_name_, msg_to_send);
                if (!send_ok) {
                    ApWarn("vehicle_chassis_send") << "Failed to send vehicle data";
                }
            } catch (const std::exception &e) {
                ApError("vehicle_chassis_send") << "Error sending message: " << e.what();
                send_ok = false;
            } catch (...) {
                ApError("vehicle_chassis_send") << "Unknown error sending message";
                send_ok = false;
            }
        }

        if (!send_ok) {
            _fault.add(_ERRORCODE_SEND_FAIL, "sendVehicleData failed");
        } else {
            _fault.remove(_ERRORCODE_SEND_FAIL);
        }

        if (new_expired) {
            int64_t over_ms = elapsed_ms - timeout_val;
            if (over_ms < 0)
                over_ms = 0; // 安全保护
            ApInfo("vehicle_chassis_send") << "[EC] 链路超时啦！已超时 " << over_ms << " ms (阈值=" << timeout_val
                                           << " ms, 当前累计=" << elapsed_ms << " ms)";
        }
    }

    void periodic_send_loop() {
        while (running_) {
            sendData();
            std::this_thread::sleep_for(std::chrono::milliseconds(20)); // 50Hz
        }
    }

    void stop_periodic_send() {
        running_ = false;
        if (periodic_thread_.joinable()) {
            periodic_thread_.join();
        }
    }

public:
    static XMidTrackSendByZMQDataInteract *Instance() {
        static XMidTrackSendByZMQDataInteract *instance = nullptr;
        if (!instance) {
            instance = new XMidTrackSendByZMQDataInteract();
        }
        return instance;
    }

private:
    XMidTrackSendByZMQDataInteract() = default;
    XMidTrackSendByZMQDataInteract(const XMidTrackSendByZMQDataInteract &) = delete;
    XMidTrackSendByZMQDataInteract &operator=(const XMidTrackSendByZMQDataInteract &) = delete;

    protocol_common::manager2chassis_control msg_;
    std::mutex                               mutex_;
    std::shared_ptr<XMessageSend>            handler_;
    std::string                              topic_name_;
    std::atomic<bool>                        running_{false};
    std::thread                              periodic_thread_;
    bool                                     initialized_ = false;

    std::atomic<bool> link_expired_{false}; // 断链标志

    std::chrono::steady_clock::time_point last_valid_tp_;
    std::atomic<int64_t>                  timeout_ms_{200}; // 实际用于超时判断（Init 时会写入）
    double                                last_upstream_time_{0.0};

    uint8_t heartbeat_counter_ = 0;
    int     last_shift_position_ = 0;
    int     last_auto_mode_ = 0;
    int     last_streeing_mode_req_ = 0;

public:
    FaultManager _fault;
};

#endif