#ifndef _CHASSIS_MANAGER_H_
#define _CHASSIS_MANAGER_H_

#include <stdio.h>
#include <string.h>

#include <vector>

#include "common.h"
// #include "Protocol.h"
#include "proto/FrameCommon.h"

namespace protocol_common {
#pragma pack(1)
// 底盘状态反馈自主接口
class VehicleDataClasss {
#pragma region params
 public:
  // 帧头
  CommunicationHead header;

  // 底盘心跳信号,按0x00到0xFF顺序加1（模式切换时需要保持心跳信号按顺序发）
  uint8_t heart;
  //控制模式 0 
  uint8_t control_mode;

  // 档位
  int8_t shift;

  // 方向盘转角
  uint16_t str_angle;

  // 
  // 油门/制动
  uint16_t parking_break;

  

  // 帧尾
//   CommunicationTail tail;

#pragma endregion
};

// VehicleDataClasss
template <>
inline VehicleDataClasss msg_parse<VehicleDataClasss>(
    const uint8_t* buffer, size_t& offset) {
  VehicleDataClasss status;
  status.header = msg_parse<CommunicationHead>(buffer, offset);
  memcpy(&status.heart, buffer + offset, sizeof(status.heart));
  offset += sizeof(status.heart);
  memcpy(&status.control_mode, buffer + offset,
         sizeof(status.control_mode));
  offset += sizeof(status.control_mode);
  memcpy(&status.shift, buffer + offset, sizeof(status.shift));
  offset += sizeof(status.shift);
  memcpy(&status.str_angle, buffer + offset,
         sizeof(status.str_angle));
  offset += sizeof(status.str_angle);
  memcpy(&status.parking_break, buffer + offset,
         sizeof(status.parking_break));
  offset += sizeof(status.parking_break);
//   memcpy(&status.steering_mode_fb, buffer + offset,
//          sizeof(status.steering_mode_fb));
//   offset += sizeof(status.steering_mode_fb);
//   memcpy(&status.drive_mode_fb, buffer + offset, sizeof(status.drive_mode_fb));
//   offset += sizeof(status.drive_mode_fb);
//   memcpy(&status.run_mode_fb, buffer + offset, sizeof(status.run_mode_fb));
//   offset += sizeof(status.run_mode_fb);
//   memcpy(&status.motor_mode, buffer + offset, sizeof(status.motor_mode));
//   offset += sizeof(status.motor_mode);
//   memcpy(&status.gear_fb, buffer + offset, sizeof(status.gear_fb));
//   offset += sizeof(status.gear_fb);
//   memcpy(&status.engine_mode_fb, buffer + offset,
//          sizeof(status.engine_mode_fb));
//   offset += sizeof(status.engine_mode_fb);
//   memcpy(&status.auto_speed_limit_fb, buffer + offset,
//          sizeof(status.auto_speed_limit_fb));
//   offset += sizeof(status.auto_speed_limit_fb);
//   memcpy(&status.silent_mode_fb, buffer + offset,
//          sizeof(status.silent_mode_fb));
//   offset += sizeof(status.silent_mode_fb);
//   memcpy(&status.braking_fb, buffer + offset, sizeof(status.braking_fb));
//   offset += sizeof(status.braking_fb);
//   memcpy(&status.eds_fb, buffer + offset, sizeof(status.eds_fb));
//   offset += sizeof(status.eds_fb);
//   memcpy(&status.low_beams_fb, buffer + offset, sizeof(status.low_beams_fb));
//   offset += sizeof(status.low_beams_fb);
//   memcpy(&status.high_beams_fb, buffer + offset, sizeof(status.high_beams_fb));
//   offset += sizeof(status.high_beams_fb);
//   memcpy(&status.leftturn_signals_fb, buffer + offset,
//          sizeof(status.leftturn_signals_fb));
//   offset += sizeof(status.leftturn_signals_fb);
//   memcpy(&status.rightturn_signals_fb, buffer + offset,
//          sizeof(status.rightturn_signals_fb));
//   offset += sizeof(status.rightturn_signals_fb);
//   memcpy(&status.lidar_eds_fb, buffer + offset, sizeof(status.lidar_eds_fb));
//   offset += sizeof(status.lidar_eds_fb);
//   memcpy(&status.radar_eds_fb, buffer + offset, sizeof(status.radar_eds_fb));
//   offset += sizeof(status.radar_eds_fb);
//   memcpy(&status.night_vision_fb, buffer + offset,
//          sizeof(status.night_vision_fb));
//   offset += sizeof(status.night_vision_fb);
//   memcpy(&status.horn_fb, buffer + offset, sizeof(status.horn_fb));
//   offset += sizeof(status.horn_fb);
//   memcpy(&status.fuel_fb, buffer + offset, sizeof(status.fuel_fb));
//   offset += sizeof(status.fuel_fb);
//   memcpy(&status.driving_range, buffer + offset, sizeof(status.driving_range));
//   offset += sizeof(status.driving_range);
//   memcpy(&status.soc, buffer + offset, sizeof(status.soc));
//   offset += sizeof(status.soc);
//   memcpy(&status.voltage, buffer + offset, sizeof(status.voltage));
//   offset += sizeof(status.voltage);
//   status.tail = msg_parse<CommunicationTail>(buffer, offset);
  return status;
}

// VehicleDataClasss
template <>
inline void msg_pack<VehicleDataClasss>(
    const VehicleDataClasss& status, uint8_t* buffer,
    size_t& offset) {
  msg_pack(status.header, buffer, offset);
  memcpy(buffer + offset, &status.heart, sizeof(status.heart));
  offset += sizeof(status.heart);
  memcpy(buffer + offset, &status.shift,
         sizeof(status.shift));
  offset += sizeof(status.shift);
  memcpy(buffer + offset, &status.parking_break, sizeof(status.parking_break));
  offset += sizeof(status.parking_break);
  memcpy(buffer + offset, &status.str_angle, sizeof(status.str_angle));
  offset += sizeof(status.str_angle);
   memcpy(buffer + offset, &status.parking_break, sizeof(status.str_angle));
  offset += sizeof(status.parking_break);
}

#pragma.pack()
}  // namespace protocol_common

#endif