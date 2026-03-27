#ifndef _MANAGER_CHASSIS_H_
#define _MANAGER_CHASSIS_H_
#include "proto/FrameCommon.h"

namespace protocol_common {
#pragma pack(1)
// 底盘控制信号
class manager2chassis_control {
// #pragma region 底盘控制信号
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
  uint16_t throttle_braking= 0;

  // 转向模式请求（控制模式为1）
  // 0：行驶转向（默认），1：中心转向
  //必须
  uint8_t steering_mode_req= 0;

  // 中心转向请求值（控制模式为1）
  // 中型：-1-右转，0-默认值， 1左转
  // 轻型：-100～100%, 左正右负
  //必须
  int8_t centrsteer_req= 0;

  // 目标曲率请求（控制模式为1），经度0.001，左正右负
    //必须
  int16_t steering_req= 0;

  // 目标车速请求（控制模式为1）：-25~25(米/秒）*10，精度0.1
    //必须
  int16_t speed_req= 0;

  // 紧急停车
  // 0：未急停，1：急停
    //必须
  uint8_t emergency_breaking= 0;

  // 底盘使能
  // 0：不使能，1：使能
  uint8_t chassis_enable= 0;

  // 操控模式（底盘不执行）
  // 0：待机，1：纯遥控，2：主动应急避险，3：纯自主，4：遥控干预，5：静默值守
  uint8_t control_mode_req= 0;

  // 发动机起停
  // 0：正常停机，1：启动发动机，2：快速停机
  uint8_t engine_control = 0;

  // 发电功率
  // 0：怠速发电，1：低功率发电，2：中功率发电，3：高功率发电，4：功率跟随模式
  uint8_t engine_power_mode= 0;

  // 底盘限速，绝对值，单位1千米/小时
  //必须
  uint8_t speed_limit= 0;

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
  uint8_t low_beams= 0;

  // 远光灯
  // 0：保留，1：接通，2：关闭，3：保护复位
  uint8_t high_beams= 0;

  // 左转向灯：【在自主模式下，底盘转向灯控制由自主命令下发。】
  // 0：保留，1：接通，2：关闭，3：保护复位
  uint8_t leftturn_signals= 0;

  // 右转向灯：【在自主模式下，底盘转向灯控制由自主命令下发。】
  // 0：保留，1：接通，2：关闭，3：保护复位
  uint8_t rightturn_signals= 0;

  // 示廓灯
  // 0：保留，1：接通，2：关闭，3：保护复位
  uint8_t hazard_lights= 0;

  // 双闪
  // 0：保留，1：接通，2：关闭，3：保护复位
  uint8_t double_flash= 0;

  // 喇叭
  // 0：保留，1：接通，2：关闭，3：保护复位
  uint8_t horn= 0;

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
// #pragma endregion
};

// #pragma pack(1)
// template <>
// inline manager2chassis_control msg_parse<manager2chassis_control>(
//     const uint8_t* buffer, size_t& offset) {
//   manager2chassis_control control;
  
//   // 解析帧头
//   control.header = msg_parse<CommunicationHead>(buffer, offset);
  
//   // 解析各个成员变量
//   memcpy(&control.heart, buffer + offset, sizeof(control.heart));
//   offset += sizeof(control.heart);
  
//   memcpy(&control.control_mode, buffer + offset, sizeof(control.control_mode));
//   offset += sizeof(control.control_mode);
  
//   memcpy(&control.gear, buffer + offset, sizeof(control.gear));
//   offset += sizeof(control.gear);
  
//   memcpy(&control.steering, buffer + offset, sizeof(control.steering));
//   offset += sizeof(control.steering);
  
//   memcpy(&control.throttle_braking, buffer + offset, sizeof(control.throttle_braking));
//   offset += sizeof(control.throttle_braking);
  
//   memcpy(&control.steering_mode_req, buffer + offset, sizeof(control.steering_mode_req));
//   offset += sizeof(control.steering_mode_req);
  
//   memcpy(&control.centrsteer_req, buffer + offset, sizeof(control.centrsteer_req));
//   offset += sizeof(control.centrsteer_req);
  
//   memcpy(&control.steering_req, buffer + offset, sizeof(control.steering_req));
//   offset += sizeof(control.steering_req);
  
//   memcpy(&control.speed_req, buffer + offset, sizeof(control.speed_req));
//   offset += sizeof(control.speed_req);
  
//   memcpy(&control.emergency_breaking, buffer + offset, sizeof(control.emergency_breaking));
//   offset += sizeof(control.emergency_breaking);
  
//   memcpy(&control.chassis_enable, buffer + offset, sizeof(control.chassis_enable));
//   offset += sizeof(control.chassis_enable);
  
//   memcpy(&control.control_mode_req, buffer + offset, sizeof(control.control_mode_req));
//   offset += sizeof(control.control_mode_req);
  
//   memcpy(&control.engine_control, buffer + offset, sizeof(control.engine_control));
//   offset += sizeof(control.engine_control);
  
//   memcpy(&control.engine_power_mode, buffer + offset, sizeof(control.engine_power_mode));
//   offset += sizeof(control.engine_power_mode);
  
//   memcpy(&control.speed_limit, buffer + offset, sizeof(control.speed_limit));
//   offset += sizeof(control.speed_limit);
  
//   memcpy(&control.high_voltage_power, buffer + offset, sizeof(control.high_voltage_power));
//   offset += sizeof(control.high_voltage_power);
  
//   memcpy(&control.motor_mode, buffer + offset, sizeof(control.motor_mode));
//   offset += sizeof(control.motor_mode);
  
//   memcpy(&control.run_mode, buffer + offset, sizeof(control.run_mode));
//   offset += sizeof(control.run_mode);
  
//   memcpy(&control.parking_mode, buffer + offset, sizeof(control.parking_mode));
//   offset += sizeof(control.parking_mode);
  
//   memcpy(&control.parking_brake, buffer + offset, sizeof(control.parking_brake));
//   offset += sizeof(control.parking_brake);
  
//   memcpy(&control.water_pump_control1, buffer + offset, sizeof(control.water_pump_control1));
//   offset += sizeof(control.water_pump_control1);
  
//   memcpy(&control.water_pump_control2, buffer + offset, sizeof(control.water_pump_control2));
//   offset += sizeof(control.water_pump_control2);
  
//   memcpy(&control.fan_control, buffer + offset, sizeof(control.fan_control));
//   offset += sizeof(control.fan_control);
  
//   memcpy(&control.low_beams, buffer + offset, sizeof(control.low_beams));
//   offset += sizeof(control.low_beams);
  
//   memcpy(&control.high_beams, buffer + offset, sizeof(control.high_beams));
//   offset += sizeof(control.high_beams);
  
//   memcpy(&control.leftturn_signals, buffer + offset, sizeof(control.leftturn_signals));
//   offset += sizeof(control.leftturn_signals);
  
//   memcpy(&control.rightturn_signals, buffer + offset, sizeof(control.rightturn_signals));
//   offset += sizeof(control.rightturn_signals);
  
//   memcpy(&control.hazard_lights, buffer + offset, sizeof(control.hazard_lights));
//   offset += sizeof(control.hazard_lights);
  
//   memcpy(&control.double_flash, buffer + offset, sizeof(control.double_flash));
//   offset += sizeof(control.double_flash);
  
//   memcpy(&control.horn, buffer + offset, sizeof(control.horn));
//   offset += sizeof(control.horn);
  
//   memcpy(&control.drainage_pump, buffer + offset, sizeof(control.drainage_pump));
//   offset += sizeof(control.drainage_pump);
  
//   memcpy(&control.radar_power, buffer + offset, sizeof(control.radar_power));
//   offset += sizeof(control.radar_power);
  
//   memcpy(&control.lidar_power, buffer + offset, sizeof(control.lidar_power));
//   offset += sizeof(control.lidar_power);
  
//   memcpy(&control.night_vision_power, buffer + offset, sizeof(control.night_vision_power));
//   offset += sizeof(control.night_vision_power);
  
//   memcpy(&control.load_power, buffer + offset, sizeof(control.load_power));
//   offset += sizeof(control.load_power);
  
//   memcpy(&control.debug_equipment, buffer + offset, sizeof(control.debug_equipment));
//   offset += sizeof(control.debug_equipment);
  
//   memcpy(&control.protection, buffer + offset, sizeof(control.protection));
//   offset += sizeof(control.protection);
  
//   memcpy(&control.error_reset, buffer + offset, sizeof(control.error_reset));
//   offset += sizeof(control.error_reset);
  
//   memcpy(&control.powerbattery_heating, buffer + offset, sizeof(control.powerbattery_heating));
//   offset += sizeof(control.powerbattery_heating);
  
//   memcpy(&control.mains_output, buffer + offset, sizeof(control.mains_output));
//   offset += sizeof(control.mains_output);
  
//   memcpy(&control.battery_output, buffer + offset, sizeof(control.battery_output));
//   offset += sizeof(control.battery_output);
  
//   // 解析帧尾
//   control.tail = msg_parse<CommunicationTail>(buffer, offset);
  
//   return control;
// }

// template <>
// inline void msg_pack<manager2chassis_control>(
//     const manager2chassis_control& control, uint8_t* buffer,
//     size_t& offset) {
//   // 打包帧头
//   msg_pack(control.header, buffer, offset);
  
//   // 打包各个成员变量
//   memcpy(buffer + offset, &control.heart, sizeof(control.heart));
//   offset += sizeof(control.heart);
  
//   memcpy(buffer + offset, &control.control_mode, sizeof(control.control_mode));
//   offset += sizeof(control.control_mode);
  
//   memcpy(buffer + offset, &control.gear, sizeof(control.gear));
//   offset += sizeof(control.gear);
  
//   memcpy(buffer + offset, &control.steering, sizeof(control.steering));
//   offset += sizeof(control.steering);
  
//   memcpy(buffer + offset, &control.throttle_braking, sizeof(control.throttle_braking));
//   offset += sizeof(control.throttle_braking);
  
//   memcpy(buffer + offset, &control.steering_mode_req, sizeof(control.steering_mode_req));
//   offset += sizeof(control.steering_mode_req);
  
//   memcpy(buffer + offset, &control.centrsteer_req, sizeof(control.centrsteer_req));
//   offset += sizeof(control.centrsteer_req);
  
//   memcpy(buffer + offset, &control.steering_req, sizeof(control.steering_req));
//   offset += sizeof(control.steering_req);
  
//   memcpy(buffer + offset, &control.speed_req, sizeof(control.speed_req));
//   offset += sizeof(control.speed_req);
  
//   memcpy(buffer + offset, &control.emergency_breaking, sizeof(control.emergency_breaking));
//   offset += sizeof(control.emergency_breaking);
  
//   memcpy(buffer + offset, &control.chassis_enable, sizeof(control.chassis_enable));
//   offset += sizeof(control.chassis_enable);
  
//   memcpy(buffer + offset, &control.control_mode_req, sizeof(control.control_mode_req));
//   offset += sizeof(control.control_mode_req);
  
//   memcpy(buffer + offset, &control.engine_control, sizeof(control.engine_control));
//   offset += sizeof(control.engine_control);
  
//   memcpy(buffer + offset, &control.engine_power_mode, sizeof(control.engine_power_mode));
//   offset += sizeof(control.engine_power_mode);
  
//   memcpy(buffer + offset, &control.speed_limit, sizeof(control.speed_limit));
//   offset += sizeof(control.speed_limit);
  
//   memcpy(buffer + offset, &control.high_voltage_power, sizeof(control.high_voltage_power));
//   offset += sizeof(control.high_voltage_power);
  
//   memcpy(buffer + offset, &control.motor_mode, sizeof(control.motor_mode));
//   offset += sizeof(control.motor_mode);
  
//   memcpy(buffer + offset, &control.run_mode, sizeof(control.run_mode));
//   offset += sizeof(control.run_mode);
  
//   memcpy(buffer + offset, &control.parking_mode, sizeof(control.parking_mode));
//   offset += sizeof(control.parking_mode);
  
//   memcpy(buffer + offset, &control.parking_brake, sizeof(control.parking_brake));
//   offset += sizeof(control.parking_brake);
  
//   memcpy(buffer + offset, &control.water_pump_control1, sizeof(control.water_pump_control1));
//   offset += sizeof(control.water_pump_control1);
  
//   memcpy(buffer + offset, &control.water_pump_control2, sizeof(control.water_pump_control2));
//   offset += sizeof(control.water_pump_control2);
  
//   memcpy(buffer + offset, &control.fan_control, sizeof(control.fan_control));
//   offset += sizeof(control.fan_control);
  
//   memcpy(buffer + offset, &control.low_beams, sizeof(control.low_beams));
//   offset += sizeof(control.low_beams);
  
//   memcpy(buffer + offset, &control.high_beams, sizeof(control.high_beams));
//   offset += sizeof(control.high_beams);
  
//   memcpy(buffer + offset, &control.leftturn_signals, sizeof(control.leftturn_signals));
//   offset += sizeof(control.leftturn_signals);
  
//   memcpy(buffer + offset, &control.rightturn_signals, sizeof(control.rightturn_signals));
//   offset += sizeof(control.rightturn_signals);
  
//   memcpy(buffer + offset, &control.hazard_lights, sizeof(control.hazard_lights));
//   offset += sizeof(control.hazard_lights);
  
//   memcpy(buffer + offset, &control.double_flash, sizeof(control.double_flash));
//   offset += sizeof(control.double_flash);
  
//   memcpy(buffer + offset, &control.horn, sizeof(control.horn));
//   offset += sizeof(control.horn);
  
//   memcpy(buffer + offset, &control.drainage_pump, sizeof(control.drainage_pump));
//   offset += sizeof(control.drainage_pump);
  
//   memcpy(buffer + offset, &control.radar_power, sizeof(control.radar_power));
//   offset += sizeof(control.radar_power);
  
//   memcpy(buffer + offset, &control.lidar_power, sizeof(control.lidar_power));
//   offset += sizeof(control.lidar_power);
  
//   memcpy(buffer + offset, &control.night_vision_power, sizeof(control.night_vision_power));
//   offset += sizeof(control.night_vision_power);
  
//   memcpy(buffer + offset, &control.load_power, sizeof(control.load_power));
//   offset += sizeof(control.load_power);
  
//   memcpy(buffer + offset, &control.debug_equipment, sizeof(control.debug_equipment));
//   offset += sizeof(control.debug_equipment);
  
//   memcpy(buffer + offset, &control.protection, sizeof(control.protection));
//   offset += sizeof(control.protection);
  
//   memcpy(buffer + offset, &control.error_reset, sizeof(control.error_reset));
//   offset += sizeof(control.error_reset);
  
//   memcpy(buffer + offset, &control.powerbattery_heating, sizeof(control.powerbattery_heating));
//   offset += sizeof(control.powerbattery_heating);
  
//   memcpy(buffer + offset, &control.mains_output, sizeof(control.mains_output));
//   offset += sizeof(control.mains_output);
  
//   memcpy(buffer + offset, &control.battery_output, sizeof(control.battery_output));
//   offset += sizeof(control.battery_output);
  
//   // 打包帧尾
//   msg_pack(control.tail, buffer, offset);
// }
#pragma pack()
}  // namespace protocol_common

#endif
