#pragma once
#include <arpa/inet.h>

#include <cstdio>
#include <cstring>
#include <vector>

#include "Protocol.h"
#include "common.h"

inline void reverse_endian(void *data, size_t size) {
    uint8_t *bytes = static_cast<uint8_t *>(data);
    for (size_t i = 0; i < size / 2; i++) {
        std::swap(bytes[i], bytes[size - 1 - i]);
    }
}

namespace protocol_common {

    // 底盘控制信号
    class __attribute__(()) manager2chassis_control {
#pragma region 底盘控制信号
    public:
        // 帧头 必须
        CommunicationHead header;

        // 心跳信号,按0x00到440xFF顺序加1（模式切换时需要保持心跳信号按顺序发）
        //必须
        uint8_t heart = 0;

        // 控制模式, 0：油门/制动、方向盘转角控制，1：目标车速、目标曲率控制
        //必须
        uint8_t control_mode = 0;

        // 档位（控制模式为0）
        // 0：驻车P，1：倒车档R，2：空挡N，3：前进档D，4：中心转向C
        //必须
        uint8_t gear = 0;

        // 方向盘转角（控制模式为0）
        // -100(+100)~100(+100)
        // 0~99右转；100直行；101～200左转
        //必须
        uint16_t steering = 0;

        // 油门/制动指令（控制模式为0）
        // -100(+100)~100(+100)
        // 0~99制动；100空；101～200油门
        //必须
        uint16_t throttle_braking = 0;

        // 转向模式请求（控制模式为1）
        // 0：行驶转向（默认），1：中心转向
        //必须
        uint8_t steering_mode_req = 0;

        // 中心转向请求值（控制模式为1）
        // 中型：-1-右转，0-默认值， 1左转
        // 轻型：-100～100%, 左正右负
        //必须
        int8_t centrsteer_req = 0;

        // 目标曲率请求（控制模式为1），经度0.001，左正右负
        //必须
        int16_t steering_req = 0;

        // 目标车速请求（控制模式为1）：-25~25(米/秒）*10，精度0.1
        //必须
        int16_t speed_req = 0;

        // 紧急停车
        // 0：未急停，1：急停
        //必须
        uint8_t emergency_breaking = 0;

        // 底盘使能
        // 0：不使能，1：使能
        uint8_t chassis_enable = 0;

        // 操控模式（底盘不执行）
        // 0：待机，1：纯遥控，2：主动应急避险，3：纯自主，4：遥控干预，5：静默值守
        uint8_t control_mode_req = 0;

        // 发动机起停
        // 0：正常停机，1：启动发动机，2：快速停机
        uint8_t engine_control = 0;

        // 发电功率
        // 0：怠速发电，1：低功率发电，2：中功率发电，3：高功率发电，4：功率跟随模式
        uint8_t engine_power_mode = 0;

        // 底盘限速，绝对值，单位: 千米/小时
        //必须
        uint8_t speed_limit = 0;

        // 高压上电
        // 0：上高压，1：下高压，2：紧急下电
        uint8_t high_voltage_power = 1;

        // 电机驱动模式（控制模式为1）
        // 0：转矩模式（默认），1：转速模式
        //必须 实际车速为0的时候才能切换电机驱动
        uint8_t motor_mode = 0;

        // 越野模式
        // 0：常规，1：越野，2:爬坡
        //必须
        uint8_t run_mode = 0;

        // 驻车模式
        // 0：自动驻车（默认）（仅中型响应），1：手动驻车
        //
        uint8_t parking_mode = 0;

        // 手刹
        // 0：拉起手刹，1：松开手刹
        uint8_t parking_brake = 0;

        // 水泵超越控制1
        // 0：自动，1：强制开，2：强制关
        uint8_t water_pump_control1 = 0;

        // 水泵超越控制2
        // 0：自动，1：强制开，2：强制关
        uint8_t water_pump_control2 = 0;

        // 风扇超越控制
        // 0：自动，1：强制开，2：强制关
        uint8_t fan_control = 0;

        // 近光灯
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t low_beams = 0;

        // 远光灯
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t high_beams = 0;

        // 左转向灯：【在自主模式下，底盘转向灯控制由自主命令下发。】
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t leftturn_signals = 0;

        // 右转向灯：【在自主模式下，底盘转向灯控制由自主命令下发。】
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t rightturn_signals = 0;

        // 示廓灯
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t hazard_lights = 0;

        // 双闪
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t double_flash = 0;

        // 喇叭
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t horn = 0;

        // 排水泵
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t drainage_pump = 0;

        // 毫米波雷达上电
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t radar_power = 1;

        // 激光雷达上电
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t lidar_power = 1;

        // 夜视相机上电
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t night_vision_power = 1;

        // 载荷低压上电
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t load_power = 2;

        // 调试设备（预留）
        // 0：保留，1：接通，2：关闭，3：保护复位
        uint8_t debug_equipment = 1;

        // 保护屏蔽命令（预留）
        // 0：不屏蔽保护，1：屏蔽所有保护
        uint8_t protection = 0;

        // 故障复位命令（预留）
        // 0：不复位，1：复位所有故障
        uint8_t error_reset = 0;

        // 动力电池加温指令
        // 0：自动加温，1：强制加温，2：强制关闭加温
        uint8_t powerbattery_heating = 0;

        // 220V对外输出
        // 0：未开始，1：高压上电，2：高压下电
        uint8_t mains_output = 0;

        // 蓄电池加温指令
        // 0：自动加温，1：强制加温，2：强制关闭加温
        uint8_t battery_output = 0;

        // 帧尾
        CommunicationTail tail;
#pragma endregion
    };

    // 反序列化
    template <>
    inline manager2chassis_control msg_parse<manager2chassis_control>(const uint8_t *buffer, size_t size,
                                                                      size_t &offset) {
        manager2chassis_control obj;
        obj.header = msg_parse<CommunicationHead>(buffer, size, offset);

        read_le(obj.heart, buffer, size, offset);
        read_le(obj.control_mode, buffer, size, offset);
        read_le(obj.gear, buffer, size, offset);
        read_le(obj.steering, buffer, size, offset);
        read_le(obj.throttle_braking, buffer, size, offset);
        read_le(obj.steering_mode_req, buffer, size, offset);

        read_le(obj.centrsteer_req, buffer, size, offset);

        read_le(obj.steering_req, buffer, size, offset);

        read_le(obj.speed_req, buffer, size, offset);

        read_le(obj.emergency_breaking, buffer, size, offset);

        read_le(obj.chassis_enable, buffer, size, offset);

        read_le(obj.control_mode_req, buffer, size, offset);

        read_le(obj.engine_control, buffer, size, offset);

        read_le(obj.engine_power_mode, buffer, size, offset);

        read_le(obj.speed_limit, buffer, size, offset);

        read_le(obj.high_voltage_power, buffer, size, offset);

        read_le(obj.motor_mode, buffer, size, offset);

        read_le(obj.run_mode, buffer, size, offset);

        read_le(obj.parking_mode, buffer, size, offset);

        read_le(obj.parking_brake, buffer, size, offset);

        read_le(obj.water_pump_control1, buffer, size, offset);

        read_le(obj.water_pump_control2, buffer, size, offset);

        read_le(obj.fan_control, buffer, size, offset);

        read_le(obj.low_beams, buffer, size, offset);

        read_le(obj.high_beams, buffer, size, offset);

        read_le(obj.leftturn_signals, buffer, size, offset);

        read_le(obj.rightturn_signals, buffer, size, offset);

        read_le(obj.hazard_lights, buffer, size, offset);

        read_le(obj.double_flash, buffer, size, offset);

        read_le(obj.horn, buffer, size, offset);

        read_le(obj.drainage_pump, buffer, size, offset);

        read_le(obj.radar_power, buffer, size, offset);

        read_le(obj.lidar_power, buffer, size, offset);

        read_le(obj.night_vision_power, buffer, size, offset);

        read_le(obj.load_power, buffer, size, offset);

        read_le(obj.debug_equipment, buffer, size, offset);

        read_le(obj.protection, buffer, size, offset);

        read_le(obj.error_reset, buffer, size, offset);

        read_le(obj.powerbattery_heating, buffer, size, offset);

        read_le(obj.mains_output, buffer, size, offset);

        read_le(obj.battery_output, buffer, size, offset);

        reverse_endian(&obj.steering, sizeof(obj.steering));
        reverse_endian(&obj.throttle_braking, sizeof(obj.throttle_braking));
        reverse_endian(&obj.steering_req, sizeof(obj.steering_req));
        reverse_endian(&obj.speed_req, sizeof(obj.speed_req));

        // 帧尾
        obj.tail = msg_parse<CommunicationTail>(buffer, size, offset);

        return obj;
    }

    // 序列化
    template <>
    inline void msg_pack<manager2chassis_control>(const manager2chassis_control &obj, std::vector<uint8_t> &buffer) {
        // 帧头
        msg_pack(obj.header, buffer);

        msg_pack(obj.heart, buffer);
        msg_pack(obj.control_mode, buffer);
        msg_pack(obj.gear, buffer);
        msg_pack(htons(obj.steering), buffer);
        msg_pack(htons(obj.throttle_braking), buffer);
        msg_pack(obj.steering_mode_req, buffer);
        msg_pack(obj.centrsteer_req, buffer);
        msg_pack(htons(obj.steering_req), buffer);
        msg_pack(htons(obj.speed_req), buffer);
        msg_pack(obj.emergency_breaking, buffer);

        msg_pack(obj.chassis_enable, buffer);
        msg_pack(obj.control_mode_req, buffer);
        msg_pack(obj.engine_control, buffer);
        msg_pack(obj.engine_power_mode, buffer);
        msg_pack(obj.speed_limit, buffer);
        msg_pack(obj.high_voltage_power, buffer);
        msg_pack(obj.motor_mode, buffer);
        msg_pack(obj.run_mode, buffer);
        msg_pack(obj.parking_mode, buffer);
        msg_pack(obj.parking_brake, buffer);
        msg_pack(obj.water_pump_control1, buffer);
        msg_pack(obj.water_pump_control2, buffer);
        msg_pack(obj.fan_control, buffer);
        msg_pack(obj.low_beams, buffer);
        msg_pack(obj.high_beams, buffer);
        msg_pack(obj.leftturn_signals, buffer);
        msg_pack(obj.rightturn_signals, buffer);
        msg_pack(obj.hazard_lights, buffer);
        msg_pack(obj.double_flash, buffer);
        msg_pack(obj.horn, buffer);
        msg_pack(obj.drainage_pump, buffer);
        msg_pack(obj.radar_power, buffer);
        msg_pack(obj.lidar_power, buffer);
        msg_pack(obj.night_vision_power, buffer);
        msg_pack(obj.load_power, buffer);
        msg_pack(obj.debug_equipment, buffer);
        msg_pack(obj.protection, buffer);
        msg_pack(obj.error_reset, buffer);
        msg_pack(obj.powerbattery_heating, buffer);
        msg_pack(obj.mains_output, buffer);
        msg_pack(obj.battery_output, buffer);

        // 帧尾
        msg_pack(obj.tail, buffer);
    }

} // namespace protocol_common
