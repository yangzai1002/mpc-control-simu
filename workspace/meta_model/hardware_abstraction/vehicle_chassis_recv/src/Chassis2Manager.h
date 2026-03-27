#ifndef _CHASSIS_MANAGER_H_
#define _CHASSIS_MANAGER_H_

#include <stdio.h>
#include <string.h>

#include <vector>

#include "protocol/FrameCommon.h"
#include "common.h"


namespace protocol_common {
#pragma pack(1)
// 底盘状态反馈自主接口
class chassis2manager_chassis_status {
#pragma region params
 public:
  // 帧头
  CommunicationHead header;

  // 底盘心跳信号,按0x00到0xFF顺序加1（模式切换时需要保持心跳信号按顺序发）
  uint8_t heart;

  // VCU故障等级
  /*
  0：正常
  1:一级警告，VCU不做处理
  2:二级限制（对车速、扭矩等进行限制）
  3:三级故障：紧急停车
  4：四级故障：下高压
  */
  uint8_t VCU_faultlevel;

  //
  uint16_t throttle_breaking;
   //
  uint16_t steering_mode_req ;

  // 车速反馈，m/s
  // [-250,250]
  int16_t speed_fb;

  // 急停反馈
  // 0:未急停，1:急停
  uint8_t emergency_stop_fb;

  // 驻车反馈
  // 0:未驻车，1:驻车
  uint8_t parking_break_fb;

  // 底盘整备反馈：
  // 0:未就绪，1:就绪(整车高压上电完成，无三级及以上故障、无急停为底盘Ready使能)
  uint8_t vechile_ready_fb;

  // 转向模式状态反馈
  // 0:默认值，1:中心转向
  uint8_t steering_mode_fb;

  // 驾驶模式反馈
  // 0:待机，1：远程遥控，2:自主驾驶，3:近程遥控
  uint8_t drive_mode_fb;

  // 越野工作模式反馈
  // 0：常规，1：越野，2:爬坡
  uint8_t run_mode_fb;

  // 电机模式反馈
  // 0：扭矩模式，1：转速模式
  uint8_t motor_mode;

  // 整车挡位反馈
  // 0：驻车P，1：倒车档R，2：空挡N，3：前进档D，4：中心转向C
  uint8_t gear_fb;

  // 当前动力模式
  // 0：纯电，1：混动
  uint8_t engine_mode_fb;

  // 当前自主限速，单位：m/h
  // [0,250]
  uint8_t auto_speed_limit_fb;

  //注释后显示错误
  // 静默模式
  // 0:未静默，1:静默
  uint8_t silent_mode_fb;

  // 制动百分比，精度1
  // 底盘实际制动执行器响应百分比(%)
  uint8_t braking_fb;

  // 平台配电状态反馈
  // 0：未上高压，1：已上高压，2：紧急下电
  uint8_t eds_fb;

  // 大灯（近光灯）状态反馈
  // 0:关闭，1：打开，2：过流，8：短路
  uint8_t low_beams_fb;

  // 大灯（远光灯）状态反馈
  // 0:关闭，1：打开，2：过流，8：短路
  uint8_t high_beams_fb;

  // 左转向灯状态反馈
  // 0:关闭，1：打开，2：过流，8：短路
  uint8_t leftturn_signals_fb;

  // 右转向灯状态反馈
  // 0:关闭，1：打开，2：过流，8：短路
  uint8_t rightturn_signals_fb;

  // 激光雷达配电反馈
  // 0:关闭，1：打开，2：过流，8：短路
  uint8_t lidar_eds_fb;

  // 毫米波配电反馈
  // 0:关闭，1：打开，2：过流，8：短路
  uint8_t radar_eds_fb;

  // 夜视相机配电反馈
  // 0:关闭，1：打开，2：过流，8：短路
  uint8_t night_vision_fb;

  // 喇叭状态反馈接口
  // 0:关闭，1：打开，2：过流，8：短路
  uint8_t horn_fb;

  // 油量，单位L，精度0.1
  // *10
  uint16_t fuel_fb;

  // 总里程反馈，单位km
  uint32_t driving_range;

  // 电量（soc值），容量的百分比
  // [0,100]
  uint8_t soc;

  // 电压，单位：伏，精度0.1
  // *10
  uint16_t voltage;

  // 帧尾
  CommunicationTail tail;

#pragma endregion
};

// chassis2manager_chassis_status
template <>
inline chassis2manager_chassis_status msg_parse<chassis2manager_chassis_status>(
    const uint8_t* buffer, size_t& offset) {
  chassis2manager_chassis_status status;
  status.header = msg_parse<CommunicationHead>(buffer, offset);
  memcpy(&status.heart, buffer + offset, sizeof(status.heart));
  offset += sizeof(status.heart);
  memcpy(&status.VCU_faultlevel, buffer + offset,
         sizeof(status.VCU_faultlevel));
  offset += sizeof(status.VCU_faultlevel);
  memcpy(&status.speed_fb, buffer + offset, sizeof(status.speed_fb));
  offset += sizeof(status.speed_fb);
  memcpy(&status.emergency_stop_fb, buffer + offset,
         sizeof(status.emergency_stop_fb));
  offset += sizeof(status.emergency_stop_fb);
  memcpy(&status.parking_break_fb, buffer + offset,
         sizeof(status.parking_break_fb));
  offset += sizeof(status.parking_break_fb);
  memcpy(&status.vechile_ready_fb, buffer + offset,
         sizeof(status.vechile_ready_fb));
  offset += sizeof(status.vechile_ready_fb);
  memcpy(&status.steering_mode_fb, buffer + offset,
         sizeof(status.steering_mode_fb));
  offset += sizeof(status.steering_mode_fb);
  memcpy(&status.drive_mode_fb, buffer + offset, sizeof(status.drive_mode_fb));
  offset += sizeof(status.drive_mode_fb);
  memcpy(&status.run_mode_fb, buffer + offset, sizeof(status.run_mode_fb));
  offset += sizeof(status.run_mode_fb);
  memcpy(&status.motor_mode, buffer + offset, sizeof(status.motor_mode));
  offset += sizeof(status.motor_mode);
  memcpy(&status.gear_fb, buffer + offset, sizeof(status.gear_fb));
  offset += sizeof(status.gear_fb);
  memcpy(&status.engine_mode_fb, buffer + offset,
         sizeof(status.engine_mode_fb));
  offset += sizeof(status.engine_mode_fb);
  memcpy(&status.auto_speed_limit_fb, buffer + offset,
         sizeof(status.auto_speed_limit_fb));
  offset += sizeof(status.auto_speed_limit_fb);
  memcpy(&status.silent_mode_fb, buffer + offset,
         sizeof(status.silent_mode_fb));
  offset += sizeof(status.silent_mode_fb);
  memcpy(&status.braking_fb, buffer + offset, sizeof(status.braking_fb));
  offset += sizeof(status.braking_fb);
  memcpy(&status.eds_fb, buffer + offset, sizeof(status.eds_fb));
  offset += sizeof(status.eds_fb);
  memcpy(&status.low_beams_fb, buffer + offset, sizeof(status.low_beams_fb));
  offset += sizeof(status.low_beams_fb);
  memcpy(&status.high_beams_fb, buffer + offset, sizeof(status.high_beams_fb));
  offset += sizeof(status.high_beams_fb);
  memcpy(&status.leftturn_signals_fb, buffer + offset,
         sizeof(status.leftturn_signals_fb));
  offset += sizeof(status.leftturn_signals_fb);
  memcpy(&status.rightturn_signals_fb, buffer + offset,
         sizeof(status.rightturn_signals_fb));
  offset += sizeof(status.rightturn_signals_fb);
  memcpy(&status.lidar_eds_fb, buffer + offset, sizeof(status.lidar_eds_fb));
  offset += sizeof(status.lidar_eds_fb);
  memcpy(&status.radar_eds_fb, buffer + offset, sizeof(status.radar_eds_fb));
  offset += sizeof(status.radar_eds_fb);
  memcpy(&status.night_vision_fb, buffer + offset,
         sizeof(status.night_vision_fb));
  offset += sizeof(status.night_vision_fb);
  memcpy(&status.horn_fb, buffer + offset, sizeof(status.horn_fb));
  offset += sizeof(status.horn_fb);
  memcpy(&status.fuel_fb, buffer + offset, sizeof(status.fuel_fb));
  offset += sizeof(status.fuel_fb);
  memcpy(&status.driving_range, buffer + offset, sizeof(status.driving_range));
  offset += sizeof(status.driving_range);
  memcpy(&status.soc, buffer + offset, sizeof(status.soc));
  offset += sizeof(status.soc);
  memcpy(&status.voltage, buffer + offset, sizeof(status.voltage));
  offset += sizeof(status.voltage);
  status.tail = msg_parse<CommunicationTail>(buffer, offset);
  return status;
}

// chassis2manager_chassis_status
template <>
inline void msg_pack<chassis2manager_chassis_status>(
    const chassis2manager_chassis_status& status, uint8_t* buffer,
    size_t& offset) {
  msg_pack(status.header, buffer, offset);
  memcpy(buffer + offset, &status.heart, sizeof(status.heart));
  offset += sizeof(status.heart);
  memcpy(buffer + offset, &status.VCU_faultlevel,
         sizeof(status.VCU_faultlevel));
  offset += sizeof(status.VCU_faultlevel);
  memcpy(buffer + offset, &status.speed_fb, sizeof(status.speed_fb));
  offset += sizeof(status.speed_fb);
  memcpy(buffer + offset, &status.emergency_stop_fb,
         sizeof(status.emergency_stop_fb));
  offset += sizeof(status.emergency_stop_fb);
  memcpy(buffer + offset, &status.parking_break_fb,
         sizeof(status.parking_break_fb));
  offset += sizeof(status.parking_break_fb);
  memcpy(buffer + offset, &status.vechile_ready_fb,
         sizeof(status.vechile_ready_fb));
  offset += sizeof(status.vechile_ready_fb);
  memcpy(buffer + offset, &status.steering_mode_fb,
         sizeof(status.steering_mode_fb));
  offset += sizeof(status.steering_mode_fb);
  memcpy(buffer + offset, &status.drive_mode_fb, sizeof(status.drive_mode_fb));
  offset += sizeof(status.drive_mode_fb);
  memcpy(buffer + offset, &status.run_mode_fb, sizeof(status.run_mode_fb));
  offset += sizeof(status.run_mode_fb);
  memcpy(buffer + offset, &status.motor_mode, sizeof(status.motor_mode));
  offset += sizeof(status.motor_mode);
  memcpy(buffer + offset, &status.gear_fb, sizeof(status.gear_fb));
  offset += sizeof(status.gear_fb);
  memcpy(buffer + offset, &status.engine_mode_fb,
         sizeof(status.engine_mode_fb));
  offset += sizeof(status.engine_mode_fb);
  memcpy(buffer + offset, &status.auto_speed_limit_fb,
         sizeof(status.auto_speed_limit_fb));
  offset += sizeof(status.auto_speed_limit_fb);
  memcpy(buffer + offset, &status.silent_mode_fb,
         sizeof(status.silent_mode_fb));
  offset += sizeof(status.silent_mode_fb);
  memcpy(buffer + offset, &status.braking_fb, sizeof(status.braking_fb));
  offset += sizeof(status.braking_fb);
  memcpy(buffer + offset, &status.eds_fb, sizeof(status.eds_fb));
  offset += sizeof(status.eds_fb);
  memcpy(buffer + offset, &status.low_beams_fb, sizeof(status.low_beams_fb));
  offset += sizeof(status.low_beams_fb);
  memcpy(buffer + offset, &status.high_beams_fb, sizeof(status.high_beams_fb));
  offset += sizeof(status.high_beams_fb);
  memcpy(buffer + offset, &status.leftturn_signals_fb,
         sizeof(status.leftturn_signals_fb));
  offset += sizeof(status.leftturn_signals_fb);
  memcpy(buffer + offset, &status.rightturn_signals_fb,
         sizeof(status.rightturn_signals_fb));
  offset += sizeof(status.rightturn_signals_fb);
  memcpy(buffer + offset, &status.lidar_eds_fb, sizeof(status.lidar_eds_fb));
  offset += sizeof(status.lidar_eds_fb);
  memcpy(buffer + offset, &status.radar_eds_fb, sizeof(status.radar_eds_fb));
  offset += sizeof(status.radar_eds_fb);
  memcpy(buffer + offset, &status.night_vision_fb,
         sizeof(status.night_vision_fb));
  offset += sizeof(status.night_vision_fb);
  memcpy(buffer + offset, &status.horn_fb, sizeof(status.horn_fb));
  offset += sizeof(status.horn_fb);
  memcpy(buffer + offset, &status.fuel_fb, sizeof(status.fuel_fb));
  offset += sizeof(status.fuel_fb);
  memcpy(buffer + offset, &status.driving_range, sizeof(status.driving_range));
  offset += sizeof(status.driving_range);
  memcpy(buffer + offset, &status.soc, sizeof(status.soc));
  offset += sizeof(status.soc);
  memcpy(buffer + offset, &status.voltage, sizeof(status.voltage));
  offset += sizeof(status.voltage);
  msg_pack(status.tail, buffer, offset);
}

#pragma.pack()
}  // namespace protocol_common

#endif