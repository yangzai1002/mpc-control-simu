#ifndef CHASSIS_STATUS_H
#define CHASSIS_STATUS_H

#include <cstdint>
#include "protocol/FrameCommon.h"

#pragma pack(push, 1)

namespace protocol_common {


struct ChassisStatus {
    // 帧头16
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
    // 0：待机；1：纯遥控；2：应急干预；3：纯自主；4：干预自主；5：静默值守；6：近程遥控
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
};

#pragma pack(pop)

} // namespace protocol_common

#endif // CHASSIS_STATUS_H
