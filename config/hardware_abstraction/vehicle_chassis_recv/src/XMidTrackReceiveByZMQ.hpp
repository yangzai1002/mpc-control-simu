
#ifndef __XMID_TRACK_RECEVIVE_BY_ZMQ_HPP__
#define __XMID_TRACK_RECEVIVE_BY_ZMQ_HPP__
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <zmq.hpp>

#include "ChassisStatus.h"
#include "DatetimeUtil.hpp"
#include "FaultManager.hpp"
#include "StringUtils.hpp"
#include "ZmqHelper.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "config_loader/custom_stack.h"
#include "log/log.h"
#include "monitor/faulthandle_api.hpp"

static const constexpr char _ERRORCODE_DEVICE_CONNECT_ERROR[] = "1052002";  // ZMQ 设备连接失败
static const constexpr char _ERRORCODE_DATA_READ_ERROR[] = "1052003";       // ZMQ 设备数据读取失败
static const constexpr char _ERRORCODE_MISSING_CRITICAL_DATA[] = "1052004"; // ZMQ 缺少关键数据
static const constexpr char _ERRORCODE_INVALID_DATA_CONTENT[] = "1052005";  // ZMQ 数据内容错误
static const constexpr char _ERRORCODE_DATA_FREQUENCY_ERROR[] = "1052006";  // ZMQ 数据频率异常
static const constexpr char _ERRORCODE_CHASSIS_ERROR[] = "1052007";         // ZMQ 底盘故障

namespace protocol_common {} // namespace protocol_common

class XMidTrackReceiveByZMQDataInteract {
public:
    XMidTrackReceiveByZMQDataInteract() = default;

    template <typename T> T read_be(const uint8_t *buf, size_t &offset) {
        T v = 0;
        for (size_t i = 0; i < sizeof(T); ++i) {
            // 每读一字节，就左移再或进 v，保证第一个字节是最高位
            v = (v << 8) | buf[offset++];
        }
        return v;
    }

    // 简单累加校验函数（可按需替换成 CRC）
    uint32_t CalcChecksum(const uint8_t *data, size_t len) {
        uint32_t sum = 0;
        for (size_t i = 0; i < len; ++i) {
            sum += data[i];
        }
        return sum;
    }

    // ———— 2. 按大端解析 ChassisStatus ————
    protocol_common::ChassisStatus parseChassisStatus(const uint8_t *buffer, size_t &offset, size_t total_length) {
        // 头、尾、整包、业务字段长度
        constexpr size_t kHeaderSize = sizeof(protocol_common::CommunicationHead); // 16
        constexpr size_t kTailSize = sizeof(protocol_common::CommunicationTail);   //  4
        constexpr size_t kFullLen = sizeof(protocol_common::ChassisStatus);        // 总共 54
        constexpr size_t kPayloadSize = kFullLen - kHeaderSize - kTailSize;        // 34

        if (total_length != kFullLen) {
            throw std::runtime_error("报文长度不对: 实际=" + std::to_string(total_length) +
                                     "，期望=" + std::to_string(kFullLen));
        }

        protocol_common::ChassisStatus cs;
        cs.header = protocol_common::msg_parse<protocol_common::CommunicationHead>(buffer, offset);

        // 记录业务字段起始位置（用于校验和）
        size_t payload_start = offset;

        // 多字节字段自动按大端组装 ————

        cs.heart = read_be<uint8_t>(buffer, offset);
        cs.VCU_faultlevel = read_be<uint8_t>(buffer, offset);
        cs.throttle_braking = read_be<uint16_t>(buffer, offset);
        cs.steering_mode_req = read_be<uint16_t>(buffer, offset);

        cs.speed_fb = static_cast<int16_t>(read_be<uint16_t>(buffer, offset)); // 2 字节有符号
        cs.emergency_stop_fb = read_be<uint8_t>(buffer, offset);
        cs.parking_break_fb = read_be<uint8_t>(buffer, offset);
        cs.vechile_ready_fb = read_be<uint8_t>(buffer, offset);
        cs.steering_mode_fb = read_be<uint8_t>(buffer, offset);
        cs.drive_mode_fb = read_be<uint8_t>(buffer, offset);
        cs.run_mode_fb = read_be<uint8_t>(buffer, offset);
        cs.parking_mode = read_be<uint8_t>(buffer, offset);
        cs.parking_brake = read_be<uint8_t>(buffer, offset);

        cs.motor_mode = read_be<uint8_t>(buffer, offset);
        cs.gear_fb = read_be<uint8_t>(buffer, offset);
        cs.engine_mode_fb = read_be<uint8_t>(buffer, offset);
        cs.auto_speed_limit_fb = read_be<uint8_t>(buffer, offset);
        cs.eds_fb = read_be<uint8_t>(buffer, offset);
        cs.low_beams_fb = read_be<uint8_t>(buffer, offset);
        cs.high_beams_fb = read_be<uint8_t>(buffer, offset);
        cs.leftturn_signals_fb = read_be<uint8_t>(buffer, offset);
        cs.rightturn_signals_fb = read_be<uint8_t>(buffer, offset);
        cs.lidar_eds_fb = read_be<uint8_t>(buffer, offset);
        cs.radar_eds_fb = read_be<uint8_t>(buffer, offset);
        cs.night_vision_fb = read_be<uint8_t>(buffer, offset);
        cs.horn_fb = read_be<uint8_t>(buffer, offset);
        cs.fuel_fb = read_be<uint16_t>(buffer, offset);        // 2 字节无符号
        cs.remaining_mile = read_be<uint32_t>(buffer, offset); // 4 字节无符号
        cs.soc = read_be<uint8_t>(buffer, offset);
        cs.voltage = read_be<uint16_t>(buffer, offset);
        cs.tail.payload_checksum = read_be<uint32_t>(buffer, offset); // 大端解析

        // 4) 校验和检查：只对 payload（不含头尾）部分做累加
        // uint32_t actual = CalcChecksum(buffer + payload_start, kPayloadSize);
        // if (actual != cs.tail.payload_checksum) {
        //     throw std::runtime_error("Checksum 错误: 包中=" + std::to_string(cs.tail.payload_checksum) +
        //                              "，计算=" + std::to_string(actual));
        // }

        return cs;
    }

    void Init(const std::string &endpoint, const std::string &topic, int32_t timeout) {
        topic_name_ = topic;
        timeout_ = timeout;

        zmq_ = std::make_shared<ZmqHelper>(1);

        try {
            AINFO << "初始化ZMQ订阅: endpoint=" << endpoint << ", topic=" << topic;
            zmq::socket_t sub_socket_raw = zmq_->createSubscriber(endpoint, {topic}, false, true);

            sub_socket_ = std::make_shared<zmq::socket_t>(std::move(sub_socket_raw));

        } catch (const std::exception &e) {
            // 异常处理
            AINFO << "初始化ZMQ订阅失败: " << e.what();
            _fault.add(_ERRORCODE_DEVICE_CONNECT_ERROR, e.what());
            return;
        }

        // 启动后台线程执行接收
        recv_thread_ = std::thread(&XMidTrackReceiveByZMQDataInteract::RecvLoop, this);
        recv_thread_.detach();
    }

    void GetVehicleInformation(ara::adsfi::VehicleInformation &vehicle_info) {
        std::lock_guard<std::mutex> lk(mutex_);
        auto                        now = std::chrono::steady_clock::now();
        bool                        timeout = (now - last_recv_time_) > std::chrono::milliseconds(timeout_);
        if (timeout) {
            vehicle_info = ara::adsfi::VehicleInformation();
            _fault.add(_ERRORCODE_DATA_FREQUENCY_ERROR, "数据读取超时");
            AINFO << "数据读取超时，已上报故障码" << _ERRORCODE_DATA_FREQUENCY_ERROR;
        } else {
            vehicle_info = current_vehicle_info_;
            _fault.remove(_ERRORCODE_DATA_FREQUENCY_ERROR);
        }
    }

private:
    void RecvLoop() {
        constexpr bool   kPrintFullPayload = true;
        constexpr size_t kHeaderSize = sizeof(protocol_common::CommunicationHead);
        constexpr size_t kTailSize = sizeof(protocol_common::CommunicationTail);
        constexpr size_t kTotalSize = sizeof(protocol_common::ChassisStatus);
        constexpr size_t kPayloadSize = kTotalSize - kHeaderSize - kTailSize;
        constexpr size_t kFullLen = kHeaderSize + kPayloadSize + kTailSize;

        while (true) {
            zmq::message_t topic_msg, data_msg;
            try {
                if (!sub_socket_->recv(topic_msg, zmq::recv_flags::dontwait)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                if (!sub_socket_->recv(data_msg, zmq::recv_flags::dontwait)) {
                    continue;
                }

                std::string topic((char *)topic_msg.data(), topic_msg.size());
                if (topic != topic_name_)
                    continue;

                AINFO << "[ZMQ] 接收到数据，长度: " << data_msg.size();

                const uint8_t *buffer = (const uint8_t *)data_msg.data();
                size_t         length = data_msg.size();

                AINFO << "[ZMQ] 收到数据的长度: " << length;
                if (length != kFullLen) {
                    AERROR << "[ZMQ] 收到包长度 " << length << "，期望 " << kFullLen << "，丢弃该包";
                    _fault.add(_ERRORCODE_DATA_READ_ERROR, "数据读取失败");
                    AINFO << "数据读取失败，已上报故障码" << _ERRORCODE_DATA_READ_ERROR;
                    continue;
                }
                _fault.remove(_ERRORCODE_DATA_READ_ERROR);

                // 打印接收数据长度和全部 HEX
                AINFO << "[ZMQ] 接收到数据长度: " << length << " 字节";
                if (kPrintFullPayload) {
                    std::ostringstream oss_all;
                    oss_all << "[ZMQ] 全部负载数据: ";
                    for (size_t i = 0; i < length; ++i) {
                        oss_all << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << int(buffer[i])
                                << " ";
                    }
                    AINFO << oss_all.str();
                }

                // —— 把解析放到 try/catch ——
                protocol_common::ChassisStatus status;
                try {
                    size_t offset = 0;
                    status = parseChassisStatus(buffer, offset, length);
                } catch (const std::runtime_error &e) {
                    // 1) 从包尾取出“包中 checksum”
                    uint32_t pkg_checksum = 0;
                    pkg_checksum = (uint32_t(buffer[length - 4]) << 24) | (uint32_t(buffer[length - 3]) << 16) |
                                   (uint32_t(buffer[length - 2]) << 8) | (uint32_t(buffer[length - 1]) << 0);

                    AERROR << "[ZMQ][ParseError] " << e.what() << " 包中 checksum=0x" << std::hex << pkg_checksum
                           << std::dec;

                    // 2) 再次打印整帧 HEX 以便对比
                    std::ostringstream oss_err;
                    oss_err << "[ZMQ][ParseError] FullFrame HEX: ";
                    for (size_t i = 0; i < length; ++i) {
                        oss_err << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << int(buffer[i])
                                << " ";
                    }
                    AERROR << oss_err.str();

                    // 等下一帧
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }

                ara::adsfi::VehicleInformation info{};
                info.vehicle_act_state.speed = static_cast<float>(status.speed_fb) / 10.0f;
                // info.vehicle_act_state.brkpedal_state = status.parking_brake;
                info.vehicle_act_state.shift_position = status.gear_fb;
                // drive_mode == 3 为自动驾驶模式，其余均为手动驾驶模式
                // if(drive_mode == 3){
                //     info.vehicle_act_state.drive_mode = 1;
                // }else{
                //     info.vehicle_act_state.drive_mode = 0;
                // }
                // FD项目产品要求 对于服务来说系统默认为自主模式，不存在手动模式，所以此处默认为1
                info.vehicle_act_state.drive_mode = 1;
                // info.vehicle_sensor_state.wiper_state = status.drive_mode_fb;
                info.vehicle_act_state.aeb_active = status.emergency_stop_fb;
                info.vehicle_sensor_state.low_beam_status = status.low_beams_fb;
                info.vehicle_sensor_state.far_beam_status = status.high_beams_fb;
                info.vehicle_sensor_state.turn_left_light_status = status.leftturn_signals_fb;
                info.vehicle_sensor_state.turn_right_light_status = status.rightturn_signals_fb;
                info.vehicle_sensor_state.horn_status = status.horn_fb;
                info.vehicle_sensor_state.vehicle_total_mile = static_cast<float>(status.remaining_mile);
                info.vehicle_sensor_state.bms_battery_soc = static_cast<float>(status.soc);
                info.vehicle_sensor_state.fuel_level = static_cast<float>(status.fuel_fb) / 10.0f;
                auto double_time = app_common::DatetimeUtil::current_second_double();
                info.header.time.sec = static_cast<decltype(info.header.time.sec)>(double_time);
                info.header.time.nsec = static_cast<decltype(info.header.time.nsec)>((double_time - info.header.time.sec) * 1e9);

                auto vcu = static_cast<int>(status.VCU_faultlevel);
                _fault.remove(_ERRORCODE_CHASSIS_ERROR);

                if (vcu > 0) {
                    const char *desc = nullptr;
                    switch (vcu) {
                        case 1:
                            desc = "VCU一级警告，VCU不做处理";
                            break;
                        case 2:
                            desc = "VCU二级限制（对车速、扭矩等进行限制）";
                            break;
                        case 3:
                            desc = "VCU三级故障：紧急停车";
                            break;
                        case 4:
                            desc = "VCU四级故障：下高压";
                            break;
                        default:
                            desc = "VCU未知故障";
                            break;
                    }
                    _fault.add(_ERRORCODE_CHASSIS_ERROR, desc);
                    AINFO << "[VCU] 级别=" << vcu << " 描述=" << desc << " 已上报故障码=" << _ERRORCODE_CHASSIS_ERROR;
                }

                AINFO << "[ZMQ] 底盘状态反馈:\n"
                      << "  message_id = " << static_cast<int32_t>(status.header.message_id) << "\n"
                      << "  心跳 = " << static_cast<int>(status.heart) << "\n"
                      << "  VCU故障等级 = " << static_cast<int>(status.VCU_faultlevel) << "\n"
                      << "  车速 = " << status.speed_fb / 10.0f << " m/s\n"
                      << "  当前自主限速 = " << static_cast<int>(status.auto_speed_limit_fb) << " km/h\n"
                      << "  急停 = " << static_cast<int>(status.emergency_stop_fb) << "\n"
                      << "  驻车 = " << static_cast<int>(status.parking_brake) << "\n"
                      << "  底盘整备 = " << static_cast<int>(status.vechile_ready_fb) << "\n"
                      << "  转向模式 = " << static_cast<int>(status.steering_mode_fb) << "\n"
                      << "  驾驶模式 = " << static_cast<int>(status.drive_mode_fb) << "\n"
                      << "  越野模式 = " << static_cast<int>(status.run_mode_fb) << "\n"
                      << "  电机模式 = " << static_cast<int>(status.motor_mode) << "\n"
                      << "  挡位 = " << static_cast<int>(status.gear_fb) << "\n"
                      << "  动力模式 = " << static_cast<int>(status.engine_mode_fb) << "\n"
                      << "  平台配电 = " << static_cast<int>(status.eds_fb) << "\n"
                      << "  近光灯 = " << static_cast<int>(status.low_beams_fb) << "\n"
                      << "  远光灯 = " << static_cast<int>(status.high_beams_fb) << "\n"
                      << "  左转向灯 = " << static_cast<int>(status.leftturn_signals_fb) << "\n"
                      << "  右转向灯 = " << static_cast<int>(status.rightturn_signals_fb) << "\n"
                      << "  激光雷达配电 = " << static_cast<int>(status.lidar_eds_fb) << "\n"
                      << "  毫米波配电 = " << static_cast<int>(status.radar_eds_fb) << "\n"
                      << "  夜视相机配电 = " << static_cast<int>(status.night_vision_fb) << "\n"
                      << "  喇叭状态 = " << static_cast<int>(status.horn_fb) << "\n"
                      << "  油量 = " << static_cast<float>(status.fuel_fb) / 10.0f << " L\n"
                      << "  剩余里程 = " << status.remaining_mile << " km\n"
                      << "  电量 (SOC) = " << static_cast<int>(status.soc) << "%\n"
                      << "  电压 = " << static_cast<float>(status.voltage) / 10.0f << " V\n";

                {
                    std::lock_guard<std::mutex> lk(mutex_);
                    current_vehicle_info_ = info;
                    last_recv_time_ = std::chrono::steady_clock::now();
                }

            } catch (const zmq::error_t &e) {
                AINFO << "[ZMQ] RecvLoop 异常: " << e.what();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

private:
    std::shared_ptr<ZmqHelper> zmq_;

    std::shared_ptr<zmq::socket_t> sub_socket_;

    std::thread recv_thread_;

    ara::adsfi::VehicleInformation current_vehicle_info_;

    std::mutex mutex_;

    std::string topic_name_;

    int32_t timeout_;

    std::chrono::steady_clock::time_point last_recv_time_;

public:
    FaultManager                              _fault;
    static XMidTrackReceiveByZMQDataInteract *Instance() {
        static XMidTrackReceiveByZMQDataInteract *instance = nullptr;
        if (!instance) {
            instance = new XMidTrackReceiveByZMQDataInteract();
        }
        return instance;
    }
};

// void XMidTrackReceiveByZMQ(VehicleInformation &vehicle_info) {
//     STATIC_DATA(bool, _init, false);

//     if (_init == false) {
//         auto ptr = CustomStack::Instance();

//         std::string addr;
//         int32_t     port;
//         std::string topic_name;
//         int32_t     timeout_local;
//         string      prefix;

//         if (ptr) {
//             if (ptr->GetConfig("config_path_prefix", prefix) < 0) {
//                 _fault.add(_ERRORCODE_CONFIG_READ_ERROR, "config_path_prefix");
//                 AINFO << "初始化参数加载错误: read local config config_path_prefix failed, 上报故障码: "
//                       << _ERRORCODE_CONFIG_READ_ERROR;
//                 return;
//             }

//             app_common::StringUtils::trim(prefix);
//             if (!prefix.empty() && prefix.back() != '/') {
//                 prefix += "/";
//             }
//             AINFO << "config_path_prefix: " << prefix;

//             std::string base = prefix + "canbus/receive/";

//             auto full_path = base + "addr";
//             if (!ptr->GetProjectConfigValue(full_path, addr) || addr.empty()) {
//                 _fault.add(_ERRORCODE_CONFIG_READ_ERROR, full_path);
//                 AINFO << "初始化参数加载错误: read project config " << full_path
//                       << " failed, 上报故障码: " << _ERRORCODE_CONFIG_READ_ERROR;
//                 return;
//             }

//             full_path = base + "port";
//             if (!ptr->GetProjectConfigValue(base + "port", port) || port <= 0) {
//                 _fault.add(_ERRORCODE_CONFIG_READ_ERROR, full_path);
//                 AINFO << "初始化参数加载错误: read project config " << base + "port"
//                       << " failed, 上报故障码: " << _ERRORCODE_CONFIG_READ_ERROR;
//                 return;
//             }

//             full_path = base + "topic_name";
//             if (!ptr->GetProjectConfigValue(full_path, topic_name) || topic_name.empty()) {
//                 _fault.add(_ERRORCODE_CONFIG_READ_ERROR, full_path);
//                 AINFO << "初始化参数加载错误: read project config " << full_path
//                       << " failed, 上报故障码: " << _ERRORCODE_CONFIG_READ_ERROR;
//                 return;
//             }

//             full_path = base + "over_time";
//             if (!ptr->GetProjectConfigValue(full_path, timeout_local) || timeout_local <= 0) {
//                 _fault.add(_ERRORCODE_CONFIG_READ_ERROR, full_path);
//                 AINFO << "初始化参数加载错误: read project config " << full_path
//                       << " failed, 上报故障码: " << _ERRORCODE_CONFIG_READ_ERROR;
//                 return;
//             }

//             // std::string endpoint = "tcp://" + ip + ":" + std::to_string(port);
//             std::string endpoint = addr;
//             AINFO << " ip: ==== " << addr;
//             AINFO << " port: ==== " << port;
//             AINFO << " topic_name: ==== " << topic_name;
//             AINFO << " timeout==== " << timeout_local;
//             AINFO << "连接地址: ====" << endpoint << " 话题: " << topic_name;

//             try {
//                 XMidTrackReceiveByZMQDataInteract::Instance()->Init(endpoint, topic_name, timeout_local);
//                 _init = true;
//                 AINFO << "ZMQ接收初始化成功-----";
//                 _fault.remove("1062001");
//             } catch (const std::exception &e) {
//                 AINFO << "初始化失败-----:" << e.what();
//             }
//         } else {
//             AINFO << "获取CustomStack实例失败";
//         }
//     }

//     XMidTrackReceiveByZMQDataInteract::Instance()->GetVehicleInformation(vehicle_info);
// }

#endif
