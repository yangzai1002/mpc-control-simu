#pragma once

#include <stdio.h>
#include <string.h>

#include <vector>

#include "Protocol.h"
#include "common.h"

namespace protocol_common {

    // 底盘状态反馈自主接口
    class __attribute__(()) chassis2manager_chassis_status {
#pragma region 底盘发送给自主系统的状态
    public:
        // 帧头
        CommunicationHead header;

        // 底盘心跳信号,按0x00到0xFF顺序加1（模式切换时需要保持心跳信号按顺序发）
        uint8_t heart;

        // VCU故障等级
        // 0：正常
        // 1:一级警告，VCU不做处理
        // 2:二级限制（对车速、扭矩等进行限制）
        // 3:三级故障：紧急停车
        // 4：四级故障：下高压
        uint8_t VCU_faultlevel = 0;

        // 当前油门/制动反馈，-100(+100)~100(+100)
        // 0～99：制动，100：空，101~200：油门
        uint16_t throttle_braking = 0;

        // 当前转向指令，-100(+100)~100(+100)
        // 0～99：右转，100：直行，101~200：左转
        uint16_t steering_mode_req = 0;

        // 车速反馈，单位：0.1米/秒
        int16_t speed_fb = 0;

        // 急停反馈
        // 0:未急停，1:急停
        uint8_t emergency_stop_fb = 0;

        // 驻车反馈
        // 0:未驻车，1:驻车
        uint8_t parking_break_fb = 0;

        // 底盘整备反馈：
        // 0:未就绪，1:就绪(整车高压上电完成，无三级及以上故障、无急停为底盘Ready使能)
        uint8_t vechile_ready_fb = 0;

        // 转向模式状态反馈
        // 0:默认值，1:中心转向
        uint8_t steering_mode_fb = 0;

        // 驾驶模式反馈
        // 0：待机，1：纯遥控，2：主动应急避险，3：纯自主，4：遥控干预，5：静默值守
        uint8_t drive_mode_fb = 0;

        // 越野工作模式反馈
        // 0：常规，1：越野，2:爬坡
        uint8_t run_mode_fb = 0;

        // 驻车模式反馈
        // 0：自动驻车（默认）（仅中型响应），1：手动驻车
        uint8_t parking_mode = 0;

        // 手刹反馈
        // 0：拉起手刹，1：松开手刹
        uint8_t parking_brake = 0;

        // 电机模式反馈
        // 0：扭矩模式，1：转速模式
        uint8_t motor_mode = 0;

        // 整车挡位反馈
        // 0：驻车P，1：倒车档R，2：空挡N，3：前进档D，4：中心转向C
        uint8_t gear_fb = 0;

        // 当前动力模式
        // 0：纯电，1：混动
        uint8_t engine_mode_fb = 0;

        // 当前自主限速，单位：千米/小时
        uint8_t auto_speed_limit_fb = 0;

        // 平台配电状态反馈
        // 0：未上高压，1：已上高压，2：紧急下电
        uint8_t eds_fb = 0;

        // 大灯（近光灯）状态反馈
        // 0:关闭，1：打开，2：过流，8：短路
        uint8_t low_beams_fb = 0;

        // 大灯（远光灯）状态反馈
        // 0:关闭，1：打开，2：过流，8：短路
        uint8_t high_beams_fb = 0;

        // 左转向灯状态反馈
        // 0:关闭，1：打开，2：过流，8：短路
        uint8_t leftturn_signals_fb = 0;

        // 右转向灯状态反馈
        // 0:关闭，1：打开，2：过流，8：短路
        uint8_t rightturn_signals_fb = 0;

        // 激光雷达配电反馈
        // 0:关闭，1：打开，2：过流，8：短路
        uint8_t lidar_eds_fb = 0;

        // 毫米波配电反馈
        // 0:关闭，1：打开，2：过流，8：短路
        uint8_t radar_eds_fb = 0;

        // 夜视相机配电反馈
        // 0:关闭，1：打开，2：过流，8：短路
        uint8_t night_vision_fb = 0;

        // 喇叭状态反馈接口
        // 0:关闭，1：打开，2：过流，8：短路
        uint8_t horn_fb = 0;

        // 油量，单位L，精度0.1
        // *10
        uint16_t fuel_fb = 0;

        // 剩余里程反馈，单位km
        uint32_t remaining_mile = 0;

        // 电量（soc值），容量的百分比
        // [0,100]
        uint8_t soc = 0;

        // 电压，单位：伏，精度0.1
        // *10
        uint16_t voltage = 0;

        // 帧尾
        CommunicationTail tail;

#pragma endregion
    };

    class __attribute__(()) chassis2manager_chassis_complete_status {
#pragma region 底盘发送给CK系统的状态
        // TODO: 需要添加完整的状态信息
    public:
#pragma endregion
    };

    template <>
    inline void msg_pack<chassis2manager_chassis_status>(const chassis2manager_chassis_status &mcs, uint8_t *buffer,
                                                         size_t &offset) {
        // 帧头
        msg_pack(mcs.header, buffer, offset);

        memcpy(buffer + offset, &mcs.heart, sizeof(mcs.heart));
        offset += sizeof(mcs.heart);

        memcpy(buffer + offset, &mcs.VCU_faultlevel, sizeof(mcs.VCU_faultlevel));
        offset += sizeof(mcs.VCU_faultlevel);

        memcpy(buffer + offset, &mcs.throttle_braking, sizeof(mcs.throttle_braking));
        reverse_endian(buffer + offset, sizeof(mcs.throttle_braking));
        offset += sizeof(mcs.throttle_braking);

        memcpy(buffer + offset, &mcs.steering_mode_req, sizeof(mcs.steering_mode_req));
        reverse_endian(buffer + offset, sizeof(mcs.steering_mode_req));
        offset += sizeof(mcs.steering_mode_req);

        memcpy(buffer + offset, &mcs.speed_fb, sizeof(mcs.speed_fb));
        reverse_endian(buffer + offset, sizeof(mcs.speed_fb));
        offset += sizeof(mcs.speed_fb);

        memcpy(buffer + offset, &mcs.emergency_stop_fb, sizeof(mcs.emergency_stop_fb));
        offset += sizeof(mcs.emergency_stop_fb);

        memcpy(buffer + offset, &mcs.parking_break_fb, sizeof(mcs.parking_break_fb));
        offset += sizeof(mcs.parking_break_fb);

        memcpy(buffer + offset, &mcs.vechile_ready_fb, sizeof(mcs.vechile_ready_fb));
        offset += sizeof(mcs.vechile_ready_fb);

        memcpy(buffer + offset, &mcs.steering_mode_fb, sizeof(mcs.steering_mode_fb));
        offset += sizeof(mcs.steering_mode_fb);

        memcpy(buffer + offset, &mcs.drive_mode_fb, sizeof(mcs.drive_mode_fb));
        offset += sizeof(mcs.drive_mode_fb);

        memcpy(buffer + offset, &mcs.run_mode_fb, sizeof(mcs.run_mode_fb));
        offset += sizeof(mcs.run_mode_fb);

        memcpy(buffer + offset, &mcs.parking_mode, sizeof(mcs.parking_mode));
        offset += sizeof(mcs.parking_mode);

        memcpy(buffer + offset, &mcs.parking_brake, sizeof(mcs.parking_brake));
        offset += sizeof(mcs.parking_brake);

        memcpy(buffer + offset, &mcs.motor_mode, sizeof(mcs.motor_mode));
        offset += sizeof(mcs.motor_mode);

        memcpy(buffer + offset, &mcs.gear_fb, sizeof(mcs.gear_fb));
        offset += sizeof(mcs.gear_fb);

        memcpy(buffer + offset, &mcs.engine_mode_fb, sizeof(mcs.engine_mode_fb));
        offset += sizeof(mcs.engine_mode_fb);

        memcpy(buffer + offset, &mcs.auto_speed_limit_fb, sizeof(mcs.auto_speed_limit_fb));
        offset += sizeof(mcs.auto_speed_limit_fb);

        memcpy(buffer + offset, &mcs.eds_fb, sizeof(mcs.eds_fb));
        offset += sizeof(mcs.eds_fb);

        memcpy(buffer + offset, &mcs.low_beams_fb, sizeof(mcs.low_beams_fb));
        offset += sizeof(mcs.low_beams_fb);

        memcpy(buffer + offset, &mcs.high_beams_fb, sizeof(mcs.high_beams_fb));
        offset += sizeof(mcs.high_beams_fb);

        memcpy(buffer + offset, &mcs.leftturn_signals_fb, sizeof(mcs.leftturn_signals_fb));
        offset += sizeof(mcs.leftturn_signals_fb);

        memcpy(buffer + offset, &mcs.rightturn_signals_fb, sizeof(mcs.rightturn_signals_fb));
        offset += sizeof(mcs.rightturn_signals_fb);

        memcpy(buffer + offset, &mcs.lidar_eds_fb, sizeof(mcs.lidar_eds_fb));
        offset += sizeof(mcs.lidar_eds_fb);

        memcpy(buffer + offset, &mcs.radar_eds_fb, sizeof(mcs.radar_eds_fb));
        offset += sizeof(mcs.radar_eds_fb);

        memcpy(buffer + offset, &mcs.night_vision_fb, sizeof(mcs.night_vision_fb));
        offset += sizeof(mcs.night_vision_fb);

        memcpy(buffer + offset, &mcs.horn_fb, sizeof(mcs.horn_fb));
        offset += sizeof(mcs.horn_fb);

        memcpy(buffer + offset, &mcs.fuel_fb, sizeof(mcs.fuel_fb));
        reverse_endian(buffer + offset, sizeof(mcs.fuel_fb));
        offset += sizeof(mcs.fuel_fb);

        memcpy(buffer + offset, &mcs.remaining_mile, sizeof(mcs.remaining_mile));
        reverse_endian(buffer + offset, sizeof(mcs.remaining_mile));
        offset += sizeof(mcs.remaining_mile);

        memcpy(buffer + offset, &mcs.soc, sizeof(mcs.soc));
        offset += sizeof(mcs.soc);

        memcpy(buffer + offset, &mcs.voltage, sizeof(mcs.voltage));
        reverse_endian(buffer + offset, sizeof(mcs.voltage));
        offset += sizeof(mcs.voltage);

        // 帧尾
        msg_pack(mcs.tail, buffer, offset);
    }

    template <>
    inline chassis2manager_chassis_status msg_parse<chassis2manager_chassis_status>(const uint8_t *buffer,
                                                                                    size_t &       offset) {
        chassis2manager_chassis_status obj;
        // 帧头
        obj.header = msg_parse<CommunicationHead>(buffer, offset);

        memcpy(&obj.heart, buffer + offset, sizeof(obj.heart));
        offset += sizeof(obj.heart);

        memcpy(&obj.VCU_faultlevel, buffer + offset, sizeof(obj.VCU_faultlevel));
        offset += sizeof(obj.VCU_faultlevel);

        memcpy(&obj.throttle_braking, buffer + offset, sizeof(obj.throttle_braking));
        reverse_endian(&obj.throttle_braking, sizeof(obj.throttle_braking));
        offset += sizeof(obj.throttle_braking);

        memcpy(&obj.steering_mode_req, buffer + offset, sizeof(obj.steering_mode_req));
        reverse_endian(&obj.steering_mode_req, sizeof(obj.steering_mode_req));
        offset += sizeof(obj.steering_mode_req);

        memcpy(&obj.speed_fb, buffer + offset, sizeof(obj.speed_fb));
        reverse_endian(&obj.speed_fb, sizeof(obj.speed_fb));
        offset += sizeof(obj.speed_fb);

        memcpy(&obj.emergency_stop_fb, buffer + offset, sizeof(obj.emergency_stop_fb));
        offset += sizeof(obj.emergency_stop_fb);

        memcpy(&obj.parking_break_fb, buffer + offset, sizeof(obj.parking_break_fb));
        offset += sizeof(obj.parking_break_fb);

        memcpy(&obj.vechile_ready_fb, buffer + offset, sizeof(obj.vechile_ready_fb));
        offset += sizeof(obj.vechile_ready_fb);

        memcpy(&obj.steering_mode_fb, buffer + offset, sizeof(obj.steering_mode_fb));
        offset += sizeof(obj.steering_mode_fb);

        memcpy(&obj.drive_mode_fb, buffer + offset, sizeof(obj.drive_mode_fb));
        offset += sizeof(obj.drive_mode_fb);

        memcpy(&obj.run_mode_fb, buffer + offset, sizeof(obj.run_mode_fb));
        offset += sizeof(obj.run_mode_fb);

        memcpy(&obj.parking_mode, buffer + offset, sizeof(obj.parking_mode));
        offset += sizeof(obj.parking_mode);

        memcpy(&obj.parking_brake, buffer + offset, sizeof(obj.parking_brake));
        offset += sizeof(obj.parking_brake);

        memcpy(&obj.motor_mode, buffer + offset, sizeof(obj.motor_mode));
        offset += sizeof(obj.motor_mode);

        memcpy(&obj.gear_fb, buffer + offset, sizeof(obj.gear_fb));
        offset += sizeof(obj.gear_fb);

        memcpy(&obj.engine_mode_fb, buffer + offset, sizeof(obj.engine_mode_fb));
        offset += sizeof(obj.engine_mode_fb);

        memcpy(&obj.auto_speed_limit_fb, buffer + offset, sizeof(obj.auto_speed_limit_fb));
        offset += sizeof(obj.auto_speed_limit_fb);

        memcpy(&obj.eds_fb, buffer + offset, sizeof(obj.eds_fb));
        offset += sizeof(obj.eds_fb);

        memcpy(&obj.low_beams_fb, buffer + offset, sizeof(obj.low_beams_fb));
        offset += sizeof(obj.low_beams_fb);

        memcpy(&obj.high_beams_fb, buffer + offset, sizeof(obj.high_beams_fb));
        offset += sizeof(obj.high_beams_fb);

        memcpy(&obj.leftturn_signals_fb, buffer + offset, sizeof(obj.leftturn_signals_fb));
        offset += sizeof(obj.leftturn_signals_fb);

        memcpy(&obj.rightturn_signals_fb, buffer + offset, sizeof(obj.rightturn_signals_fb));
        offset += sizeof(obj.rightturn_signals_fb);

        memcpy(&obj.lidar_eds_fb, buffer + offset, sizeof(obj.lidar_eds_fb));
        offset += sizeof(obj.lidar_eds_fb);

        memcpy(&obj.radar_eds_fb, buffer + offset, sizeof(obj.radar_eds_fb));
        offset += sizeof(obj.radar_eds_fb);

        memcpy(&obj.night_vision_fb, buffer + offset, sizeof(obj.night_vision_fb));
        offset += sizeof(obj.night_vision_fb);

        memcpy(&obj.horn_fb, buffer + offset, sizeof(obj.horn_fb));
        offset += sizeof(obj.horn_fb);

        memcpy(&obj.fuel_fb, buffer + offset, sizeof(obj.fuel_fb));
        reverse_endian(&obj.fuel_fb, sizeof(obj.fuel_fb));
        offset += sizeof(obj.fuel_fb);

        memcpy(&obj.remaining_mile, buffer + offset, sizeof(obj.remaining_mile));
        reverse_endian(&obj.remaining_mile, sizeof(obj.remaining_mile));
        offset += sizeof(obj.remaining_mile);

        memcpy(&obj.soc, buffer + offset, sizeof(obj.soc));
        offset += sizeof(obj.soc);

        memcpy(&obj.voltage, buffer + offset, sizeof(obj.voltage));
        reverse_endian(&obj.voltage, sizeof(obj.voltage));
        offset += sizeof(obj.voltage);

        // 帧尾
        obj.tail = msg_parse<CommunicationTail>(buffer, offset);
        return obj;
    }

} // namespace protocol_common