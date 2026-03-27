#ifndef _AUTO_MANAGER_H_
#define _AUTO_MANAGER_H_

#include <stdio.h>
#include <string.h>

#include <nlohmann/json.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include <nlohmann/json.hpp>

#include "FrameCommon.h"
#include "Protocol.h"
#include "common.h"
//to do 添加编队队形信息协议，同时
namespace protocol_common
{
#pragma pack(1)

// 故障码结构体（故障码ID、故障等级）
struct auto2manager_faultdata {
  // 故障码ID【详见故障码定义】
  uint32_t fault_id;

  // 故障等级
  // 1-严重（停车），2-警告（限速），3-信息（提示）
  uint8_t fault_level = 0;
};

// 经纬高结构体（经度、纬度、高度）
struct auto2manager_geo_point {
  // 经度，精确到小数点后6位，-180e6~180e6，181000000为无效值
  int32_t longitude = 181000000;

  // 纬度，精确到小数点后6位，-90e6~90*e6，91000000为无效值
  int32_t latitude = 181000000;

  // 高程，精确到小数点后1位，-1000e1~30000e1，999999为无效值
  int32_t altitude = 999999;
};

// 轨迹点信息结构体（时间戳、经纬高、速度、曲率、航向）
struct auto2manager_trajectory_point {
  // 时间戳，轨迹预测的时间,utc时间，精度至少到毫秒级
  uint64_t timestamp = 0;

  // 经纬高
  auto2manager_geo_point geo_point;

  // 速度，m/s,精度小数点后1位，1e1
  int16_t speed = 0;

  // 曲率，左正右副，精度小数点后3位，1e3
  int16_t curvature = 0;

  // 航向，单位度，精度小数点后2位，0e2~360e2，36100为无效值
  uint16_t heading = 36100;
};

// 感知目标结构体（目标ID、目标类型、置信度、经纬高、速度、航向角、相对位置X、相对位置Y、相对位置Z、相对速度VX、相对速度VY、绝对位置X、绝对位置Y、绝对速度VX、绝对速度VY、观测尺寸长度、观测尺寸宽度、观测尺寸高度）
struct auto2manager_perception_target {
  // 目标ID
  uint16_t target_id = 0;

  // 目标类型
  // 1-车辆，2-人员
  uint8_t target_type = 0;

  // 置信度，1~100
  uint8_t confidence = 0;

  // 经纬高
  auto2manager_geo_point geopoint;

  // 速度，单位米/秒，精度小数点后1位，1e1
  uint16_t speed = 0;

  // 航向角，单位度，精度小数点后2位，0e2~360e2，36100为无效值
  uint16_t heading = 36100;

  // 相对位置X，以xxx为原点，正前方向，单位米，精确到小数点后1位，1e1
  int16_t relative_x = 0;

  // 相对位置Y，以xxx为原点，正左方向，单位米，精确到小数点后1位，1e1
  int16_t relative_y = 0;

  // 相对位置Z，以xxx为原点，正上方向，单位米，精确到小数点后1位，1e1
  int16_t relative_z = 0;

  // 相对速度VX，以xxx为原点，正前方向，单位米/秒，精度小数点后1位，1e1
  int16_t relative_speed_x = 0;

  // 相对速度VY，以xxx为原点，正左方向，单位米/秒，精度小数点后1位，1e1
  int16_t relative_speed_y = 0;

  // 绝对位置X，以组合导航设备上电位置为原点，正右方向，单位米，精确到小数点后1位，1e1
  int32_t absolute_x = 0;

  // 绝对位置Y，以组合导航设备上电位置为原点，正前方向，单位米，精确到小数点后1位，1e1
  int32_t absolute_y = 0;

  // 绝对速度VX，以组合导航设备上电位置为原点，正右方向，单位米/秒，精度小数点后1位，1e1
  int16_t absolute_speed_x = 0;

  // 绝对速度VY，以组合导航设备上电位置为原点，正前方向，单位米/秒，精度小数点后1位，1e1
  int16_t absolute_speed_y = 0;

  // 观测尺寸长度，单位米，精确到小数点后1位，1e1
  uint16_t length = 0;

  // 观测尺寸宽度，单位米，精确到小数点后1位，1e1
  uint16_t width = 0;

  // 观测尺寸高度，单位米，精确到小数点后1位，1e1
  uint16_t height = 0;
};

// 车辆软件状态发布服务，1Hz（软件版本、感知软件状态、规划软件状态、自主机动软件状态、人机共驾模式、降级机动开关状态、紧急制动状态、故障数量、故障码列表）
class auto2manager_vehicle_status {
#pragma region 车辆软件状态发布服务
 public:
  // 帧头
  InternalFrameHeader header;

  // 软件版本
  // xx.yy.zz.year.month.day
  // Byte0：xx，Byte1：yy，Byte2：zz，Byte3：year-1970，Byte4：month，Byte5：day，Byte6～7：预留，默认为0
  uint64_t software_version = 0;

  // 感知软件状态
  // 1-正常、2-异常
  uint8_t perception_status = 0;

  // 规划软件状态
  // 1-正常、2-异常（软件异常）、3-超时(无法自主通行)
  uint8_t planning_status = 0;

  // 自主机动软件状态
  // 1-正常、2-异常
  uint8_t auto_status = 0;

  // 人机共驾模式
  // 1-纯自主，2-遥控干预，3-主动应急避险
  uint8_t human_driving_mode = 0;

  // 降级机动开关状态
  // 0-非降级机动；1-降级机动；
  uint8_t degrade_mode = 0;

  // 紧急制动状态
  // 0-非紧急制动、1-紧急制动
  uint8_t emergency_brake = 0;

  // 故障数量
  uint32_t fault_num = 0;

  // 故障码列表
  std::vector<auto2manager_faultdata> fault_list;
#pragma endregion
    };

// 车辆行动状态发布服务，1Hz（行动编号、行动类型、行动状态、进度）
class auto2manager_task_status {
#pragma region 车辆行动状态发布服务
 public:
  // 帧头
  InternalFrameHeader header;

  // 行动编号
  uint64_t action_id = 0;

  // 行动类型
  // 1-目标点机动，2-目标跟随，3-编队机动，4-返航
  uint8_t task_type = 0;

  // 行动状态
  // 0-未开始，1-运行中，2-暂停，3-中止，4-完成，5-失败
  uint8_t task_status = 0;

  // 行进进度
  // [0，100]，仅定点/返航机动类行动涉及
  uint8_t progress = 0;
#pragma endregion
    };

// 姿态调整响应服务，返回数据（推荐位置）
// 152使用
class auto2manager_adjust_attitude_response {
#pragma region 姿态调整响应服务
 public:
  // 返回值，0表示失败，1表示成功
  uint32_t ad_return = 0;

  // 推荐位置
  auto2manager_geo_point recommended_position;
#pragma endregion
};

// 全局路径规划发布服务（总里程、全局路径点数量、全局路径点列表）
class auto2manager_global_path {
#pragma region 全局路径规划发布服务
 public:
  // 返回值
  // 0- failure 1- success
  uint32_t ad_return = 0;

  // 总里程
  // 预估总里程，单位米
  uint32_t total_mileage = 0;

  // 全局路径点数量
  uint32_t pathpoint_num = 0;

        // 全局路径点列表
        std::vector<auto2manager_geo_point> pathpoint_list;
#pragma endregion
    };

// 车辆编队机动行动局部轨迹路径规划发布服务，5Hz（行动编号、编队车辆数量、编队机动行动点数量、编队机动行动点列表）
// 编队机动行动执行期间周期发布，无编队机动行动不发布
class auto2manager_formation_path {
#pragma region 编队机动局部路径
 public:
  // 帧头
  InternalFrameHeader header;

  // 轨迹点数
  uint32_t pathpoint_num = 0;

        // 轨迹点列表
        std::vector<auto2manager_trajectory_point> trajectory_points_list;
#pragma endregion
    };

// 地形感知信息发布服务，10Hz【未完成】
class auto2manager_terrain_info {
#pragma region 地形感知信息
 public:
  // 帧头
  InternalFrameHeader header;

        // 地形感知信息，BGR888，3通道
        // B:地表SurfaceFeature
        // G:地形TerrainFeature
        // R:障碍ObstacleType
        // cv::Mat terrain_info;
#pragma endregion
    };

// 视频状态上报，1Hz（分辨率、帧率、码率）
class auto2manager_video_status {
#pragma region 视频状态上报
 public:
  // 帧头
  InternalFrameHeader header;

  // 分辨率
  // 1-1920*1080
  uint8_t resolution = 0;

  // 码率
  // 1-256k，2-512k，3-1M，4-2M
  uint32_t bps = 0;

  // 帧率，fps
  uint8_t fps = 0;
#pragma endregion
};

// 感知目标发布，10Hz（目标个数、目标属性列表）
class auto2manager_perception_target_publish {
#pragma region 感知目标发布
 public:
  // 帧头
  InternalFrameHeader header;

  // 目标个数
  uint32_t target_num = 0;

  // 目标属性
  std::vector<auto2manager_perception_target> target_attributes;
#pragma endregion
};

// 触发控制量上传（Message ID最后一位、PayLoad值）
class auto2manager_control_data_upload {
#pragma region 触发控制量上传
 public:
  // 帧头
  InternalFrameHeader header;

  // Message ID 最后一位
  uint8_t message_id_data = 0;

  // PayLoad值
  uint8_t payload_data = 0;
#pragma endregion
    };

// auto2manager_faultdata
template <>
inline auto2manager_faultdata msg_parse<auto2manager_faultdata>(
    const uint8_t *buffer, size_t &offset) {
  auto2manager_faultdata obj;
  memcpy(&obj.fault_id, buffer + offset, sizeof(obj.fault_id));
  offset += sizeof(obj.fault_id);
  memcpy(&obj.fault_level, buffer + offset, sizeof(obj.fault_level));
  offset += sizeof(obj.fault_level);
  return obj;
}

// auto2manager_geo_point
template <>
inline auto2manager_geo_point msg_parse<auto2manager_geo_point>(
    const uint8_t *buffer, size_t &offset) {
  auto2manager_geo_point obj;
  memcpy(&obj.longitude, buffer + offset, sizeof(obj.longitude));
  offset += sizeof(obj.longitude);
  memcpy(&obj.latitude, buffer + offset, sizeof(obj.latitude));
  offset += sizeof(obj.latitude);
  memcpy(&obj.altitude, buffer + offset, sizeof(obj.altitude));
  offset += sizeof(obj.altitude);
  return obj;
}

// auto2manager_trajectory_point
template <>
inline auto2manager_trajectory_point msg_parse<auto2manager_trajectory_point>(
    const uint8_t *buffer, size_t &offset) {
  auto2manager_trajectory_point obj;
  memcpy(&obj.timestamp, buffer + offset, sizeof(obj.timestamp));
  offset += sizeof(obj.timestamp);
  obj.geo_point = msg_parse<auto2manager_geo_point>(buffer, offset);
  memcpy(&obj.speed, buffer + offset, sizeof(obj.speed));
  offset += sizeof(obj.speed);
  memcpy(&obj.curvature, buffer + offset, sizeof(obj.curvature));
  offset += sizeof(obj.curvature);
  memcpy(&obj.heading, buffer + offset, sizeof(obj.heading));
  offset += sizeof(obj.heading);
  return obj;
}

// auto2manager_perception_target
template <>
inline auto2manager_perception_target msg_parse<auto2manager_perception_target>(
    const uint8_t *buffer, size_t &offset) {
  auto2manager_perception_target obj;
  memcpy(&obj.target_id, buffer + offset, sizeof(obj.target_id));
  offset += sizeof(obj.target_id);
  memcpy(&obj.target_type, buffer + offset, sizeof(obj.target_type));
  offset += sizeof(obj.target_type);
  memcpy(&obj.confidence, buffer + offset, sizeof(obj.confidence));
  offset += sizeof(obj.confidence);
  obj.geopoint = msg_parse<auto2manager_geo_point>(buffer, offset);
  memcpy(&obj.speed, buffer + offset, sizeof(obj.speed));
  offset += sizeof(obj.speed);
  memcpy(&obj.heading, buffer + offset, sizeof(obj.heading));
  offset += sizeof(obj.heading);
  memcpy(&obj.relative_x, buffer + offset, sizeof(obj.relative_x));
  offset += sizeof(obj.relative_x);
  memcpy(&obj.relative_y, buffer + offset, sizeof(obj.relative_y));
  offset += sizeof(obj.relative_y);
  memcpy(&obj.relative_z, buffer + offset, sizeof(obj.relative_z));
  offset += sizeof(obj.relative_z);
  memcpy(&obj.relative_speed_x, buffer + offset, sizeof(obj.relative_speed_x));
  offset += sizeof(obj.relative_speed_x);
  memcpy(&obj.relative_speed_y, buffer + offset, sizeof(obj.relative_speed_y));
  offset += sizeof(obj.relative_speed_y);
  memcpy(&obj.absolute_x, buffer + offset, sizeof(obj.absolute_x));
  offset += sizeof(obj.absolute_x);
  memcpy(&obj.absolute_y, buffer + offset, sizeof(obj.absolute_y));
  offset += sizeof(obj.absolute_y);
  memcpy(&obj.absolute_speed_x, buffer + offset, sizeof(obj.absolute_speed_x));
  offset += sizeof(obj.absolute_speed_x);
  memcpy(&obj.absolute_speed_y, buffer + offset, sizeof(obj.absolute_speed_y));
  offset += sizeof(obj.absolute_speed_y);
  memcpy(&obj.length, buffer + offset, sizeof(obj.length));
  offset += sizeof(obj.length);
  memcpy(&obj.width, buffer + offset, sizeof(obj.width));
  offset += sizeof(obj.width);
  memcpy(&obj.height, buffer + offset, sizeof(obj.height));
  offset += sizeof(obj.height);
  return obj;
}

// auto2manager_vehicle_status
template <>
inline auto2manager_vehicle_status msg_parse<auto2manager_vehicle_status>(
    const uint8_t *buffer, size_t &offset) {
  auto2manager_vehicle_status obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.software_version, buffer + offset, sizeof(obj.software_version));
  offset += sizeof(obj.software_version);
  memcpy(&obj.perception_status, buffer + offset,
         sizeof(obj.perception_status));
  offset += sizeof(obj.perception_status);
  memcpy(&obj.planning_status, buffer + offset, sizeof(obj.planning_status));
  offset += sizeof(obj.planning_status);
  memcpy(&obj.auto_status, buffer + offset, sizeof(obj.auto_status));
  offset += sizeof(obj.auto_status);
  memcpy(&obj.human_driving_mode, buffer + offset,
         sizeof(obj.human_driving_mode));
  offset += sizeof(obj.human_driving_mode);
  memcpy(&obj.degrade_mode, buffer + offset, sizeof(obj.degrade_mode));
  offset += sizeof(obj.degrade_mode);
  memcpy(&obj.emergency_brake, buffer + offset, sizeof(obj.emergency_brake));
  offset += sizeof(obj.emergency_brake);
  memcpy(&obj.fault_num, buffer + offset, sizeof(obj.fault_num));
  offset += sizeof(obj.fault_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.fault_list.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.fault_list[i] = msg_parse<auto2manager_faultdata>(buffer, offset);
  }
  return obj;
}

// auto2manager_task_status
template <>
inline auto2manager_task_status msg_parse<auto2manager_task_status>(
    const uint8_t *buffer, size_t &offset) {
  auto2manager_task_status obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.action_id, buffer + offset, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(&obj.task_type, buffer + offset, sizeof(obj.task_type));
  offset += sizeof(obj.task_type);
  memcpy(&obj.task_status, buffer + offset, sizeof(obj.task_status));
  offset += sizeof(obj.task_status);
  memcpy(&obj.progress, buffer + offset, sizeof(obj.progress));
  offset += sizeof(obj.progress);
  return obj;
}

// auto2manager_adjust_attitude_response
template <>
inline auto2manager_adjust_attitude_response
msg_parse<auto2manager_adjust_attitude_response>(const uint8_t *buffer,
                                                 size_t &offset) {
  auto2manager_adjust_attitude_response obj;
  memcpy(&obj.ad_return, buffer + offset, sizeof(obj.ad_return));
  offset += sizeof(obj.ad_return);
  obj.recommended_position = msg_parse<auto2manager_geo_point>(buffer, offset);
  return obj;
}

// auto2manager_global_path
template <>
inline auto2manager_global_path msg_parse<auto2manager_global_path>(
    const uint8_t *buffer, size_t &offset) {
  auto2manager_global_path obj;
  // obj.ad_return = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.ad_return, buffer + offset, sizeof(obj.ad_return));
  offset += sizeof(obj.ad_return);
  memcpy(&obj.total_mileage, buffer + offset, sizeof(obj.total_mileage));
  offset += sizeof(obj.total_mileage);
  memcpy(&obj.pathpoint_num, buffer + offset, sizeof(obj.pathpoint_num));
  offset += sizeof(obj.pathpoint_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.pathpoint_list.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.pathpoint_list[i] = msg_parse<auto2manager_geo_point>(buffer, offset);
  }
  return obj;
}

// auto2manager_formation_path
template <>
inline auto2manager_formation_path msg_parse<auto2manager_formation_path>(
    const uint8_t *buffer, size_t &offset) {
  auto2manager_formation_path obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.pathpoint_num, buffer + offset, sizeof(obj.pathpoint_num));
  offset += sizeof(obj.pathpoint_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.trajectory_points_list.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.trajectory_points_list[i] =
        msg_parse<auto2manager_trajectory_point>(buffer, offset);
  }
  return obj;
}

// auto2manager_video_status
template <>
inline auto2manager_video_status msg_parse<auto2manager_video_status>(
    const uint8_t *buffer, size_t &offset) {
  auto2manager_video_status obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.resolution, buffer + offset, sizeof(obj.resolution));
  offset += sizeof(obj.resolution);
  memcpy(&obj.bps, buffer + offset, sizeof(obj.bps));
  offset += sizeof(obj.bps);
  memcpy(&obj.fps, buffer + offset, sizeof(obj.fps));
  offset += sizeof(obj.fps);
  return obj;
}

// auto2manager_perception_target_publish
template <>
inline auto2manager_perception_target_publish
msg_parse<auto2manager_perception_target_publish>(const uint8_t *buffer,
                                                  size_t &offset) {
  auto2manager_perception_target_publish obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.target_num, buffer + offset, sizeof(obj.target_num));
  offset += sizeof(obj.target_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.target_attributes.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.target_attributes[i] =
        msg_parse<auto2manager_perception_target>(buffer, offset);
  }
  return obj;
}

// auto2manager_control_data_upload
template <>
inline auto2manager_control_data_upload
msg_parse<auto2manager_control_data_upload>(const uint8_t *buffer,
                                            size_t &offset) {
  auto2manager_control_data_upload obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.message_id_data, buffer + offset, sizeof(obj.message_id_data));
  offset += sizeof(obj.message_id_data);
  memcpy(&obj.payload_data, buffer + offset, sizeof(obj.payload_data));
  offset += sizeof(obj.payload_data);
  return obj;
}

// auto2manager_faultdata
template <>
inline void msg_pack<auto2manager_faultdata>(const auto2manager_faultdata &obj,
                                             uint8_t *buffer, size_t &offset) {
  memcpy(buffer + offset, &obj.fault_id, sizeof(obj.fault_id));
  offset += sizeof(obj.fault_id);
  memcpy(buffer + offset, &obj.fault_level, sizeof(obj.fault_level));
  offset += sizeof(obj.fault_level);
}

// auto2manager_geo_point
template <>
inline void msg_pack<auto2manager_geo_point>(const auto2manager_geo_point &obj,
                                             uint8_t *buffer, size_t &offset) {
  memcpy(buffer + offset, &obj.longitude, sizeof(obj.longitude));
  offset += sizeof(obj.longitude);
  memcpy(buffer + offset, &obj.latitude, sizeof(obj.latitude));
  offset += sizeof(obj.latitude);
  memcpy(buffer + offset, &obj.altitude, sizeof(obj.altitude));
  offset += sizeof(obj.altitude);
}

// auto2manager_trajectory_point
template <>
inline void msg_pack<auto2manager_trajectory_point>(
    const auto2manager_trajectory_point &obj, uint8_t *buffer, size_t &offset) {
  memcpy(buffer + offset, &obj.timestamp, sizeof(obj.timestamp));
  offset += sizeof(obj.timestamp);
  msg_pack(obj.geo_point, buffer, offset);
  memcpy(buffer + offset, &obj.speed, sizeof(obj.speed));
  offset += sizeof(obj.speed);
  memcpy(buffer + offset, &obj.curvature, sizeof(obj.curvature));
  offset += sizeof(obj.curvature);
  memcpy(buffer + offset, &obj.heading, sizeof(obj.heading));
  offset += sizeof(obj.heading);
}

// auto2manager_perception_target
template <>
inline void msg_pack<auto2manager_perception_target>(
    const auto2manager_perception_target &obj, uint8_t *buffer,
    size_t &offset) {
  memcpy(buffer + offset, &obj.target_id, sizeof(obj.target_id));
  offset += sizeof(obj.target_id);
  memcpy(buffer + offset, &obj.target_type, sizeof(obj.target_type));
  offset += sizeof(obj.target_type);
  memcpy(buffer + offset, &obj.confidence, sizeof(obj.confidence));
  offset += sizeof(obj.confidence);
  msg_pack(obj.geopoint, buffer, offset);
  memcpy(buffer + offset, &obj.speed, sizeof(obj.speed));
  offset += sizeof(obj.speed);
  memcpy(buffer + offset, &obj.heading, sizeof(obj.heading));
  offset += sizeof(obj.heading);
  memcpy(buffer + offset, &obj.relative_x, sizeof(obj.relative_x));
  offset += sizeof(obj.relative_x);
  memcpy(buffer + offset, &obj.relative_y, sizeof(obj.relative_y));
  offset += sizeof(obj.relative_y);
  memcpy(buffer + offset, &obj.relative_z, sizeof(obj.relative_z));
  offset += sizeof(obj.relative_z);
  memcpy(buffer + offset, &obj.relative_speed_x, sizeof(obj.relative_speed_x));
  offset += sizeof(obj.relative_speed_x);
  memcpy(buffer + offset, &obj.relative_speed_y, sizeof(obj.relative_speed_y));
  offset += sizeof(obj.relative_speed_y);
  memcpy(buffer + offset, &obj.absolute_x, sizeof(obj.absolute_x));
  offset += sizeof(obj.absolute_x);
  memcpy(buffer + offset, &obj.absolute_y, sizeof(obj.absolute_y));
  offset += sizeof(obj.absolute_y);
  memcpy(buffer + offset, &obj.absolute_speed_x, sizeof(obj.absolute_speed_x));
  offset += sizeof(obj.absolute_speed_x);
  memcpy(buffer + offset, &obj.absolute_speed_y, sizeof(obj.absolute_speed_y));
  offset += sizeof(obj.absolute_speed_y);
  memcpy(buffer + offset, &obj.length, sizeof(obj.length));
  offset += sizeof(obj.length);
  memcpy(buffer + offset, &obj.width, sizeof(obj.width));
  offset += sizeof(obj.width);
  memcpy(buffer + offset, &obj.height, sizeof(obj.height));
  offset += sizeof(obj.height);
}

// auto2manager_vehicle_status
template <>
inline void msg_pack<auto2manager_vehicle_status>(
    const auto2manager_vehicle_status &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.software_version, sizeof(obj.software_version));
  offset += sizeof(obj.software_version);
  memcpy(buffer + offset, &obj.perception_status,
         sizeof(obj.perception_status));
  offset += sizeof(obj.perception_status);
  memcpy(buffer + offset, &obj.planning_status, sizeof(obj.planning_status));
  offset += sizeof(obj.planning_status);
  memcpy(buffer + offset, &obj.auto_status, sizeof(obj.auto_status));
  offset += sizeof(obj.auto_status);
  memcpy(buffer + offset, &obj.human_driving_mode,
         sizeof(obj.human_driving_mode));
  offset += sizeof(obj.human_driving_mode);
  memcpy(buffer + offset, &obj.degrade_mode, sizeof(obj.degrade_mode));
  offset += sizeof(obj.degrade_mode);
  memcpy(buffer + offset, &obj.emergency_brake, sizeof(obj.emergency_brake));
  offset += sizeof(obj.emergency_brake);
  memcpy(buffer + offset, &obj.fault_num, sizeof(obj.fault_num));
  offset += sizeof(obj.fault_num);

  uint32_t vec_size = obj.fault_list.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &f : obj.fault_list) {
    msg_pack(f, buffer, offset);
  }
}

// auto2manager_task_status
template <>
inline void msg_pack<auto2manager_task_status>(
    const auto2manager_task_status &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.action_id, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(buffer + offset, &obj.task_type, sizeof(obj.task_type));
  offset += sizeof(obj.task_type);
  memcpy(buffer + offset, &obj.task_status, sizeof(obj.task_status));
  offset += sizeof(obj.task_status);
  memcpy(buffer + offset, &obj.progress, sizeof(obj.progress));
  offset += sizeof(obj.progress);
}

// auto2manager_adjust_attitude_response
template <>
inline void msg_pack<auto2manager_adjust_attitude_response>(
    const auto2manager_adjust_attitude_response &obj, uint8_t *buffer,
    size_t &offset) {
  memcpy(buffer + offset, &obj.ad_return, sizeof(obj.ad_return));
  offset += sizeof(obj.ad_return);
  msg_pack(obj.recommended_position, buffer, offset);
}

// auto2manager_global_path
template <>
inline void msg_pack<auto2manager_global_path>(
    const auto2manager_global_path &obj, uint8_t *buffer, size_t &offset) {
    memcpy(buffer + offset, &obj.ad_return, sizeof(obj.ad_return));
    offset += sizeof(obj.ad_return);
  memcpy(buffer + offset, &obj.total_mileage, sizeof(obj.total_mileage));
  offset += sizeof(obj.total_mileage);
  memcpy(buffer + offset, &obj.pathpoint_num, sizeof(obj.pathpoint_num));
  offset += sizeof(obj.pathpoint_num);

  uint32_t vec_size = obj.pathpoint_list.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &pt : obj.pathpoint_list) {
    msg_pack(pt, buffer, offset);
  }
}

// auto2manager_formation_path
template <>
inline void msg_pack<auto2manager_formation_path>(
    const auto2manager_formation_path &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.pathpoint_num, sizeof(obj.pathpoint_num));
  offset += sizeof(obj.pathpoint_num);

  uint32_t vec_size = obj.trajectory_points_list.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &pt : obj.trajectory_points_list) {
    msg_pack(pt, buffer, offset);
  }
}

// auto2manager_video_status
template <>
inline void msg_pack<auto2manager_video_status>(
    const auto2manager_video_status &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.resolution, sizeof(obj.resolution));
  offset += sizeof(obj.resolution);
  memcpy(buffer + offset, &obj.bps, sizeof(obj.bps));
  offset += sizeof(obj.bps);
  memcpy(buffer + offset, &obj.fps, sizeof(obj.fps));
  offset += sizeof(obj.fps);
}

// auto2manager_perception_target_publish
template <>
inline void msg_pack<auto2manager_perception_target_publish>(
    const auto2manager_perception_target_publish &obj, uint8_t *buffer,
    size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.target_num, sizeof(obj.target_num));
  offset += sizeof(obj.target_num);

  uint32_t vec_size = obj.target_attributes.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &t : obj.target_attributes) {
    msg_pack(t, buffer, offset);
  }
}

// auto2manager_control_data_upload
template <>
inline void msg_pack<auto2manager_control_data_upload>(
    const auto2manager_control_data_upload &obj, uint8_t *buffer,
    size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.message_id_data, sizeof(obj.message_id_data));
  offset += sizeof(obj.message_id_data);
  memcpy(buffer + offset, &obj.payload_data, sizeof(obj.payload_data));
  offset += sizeof(obj.payload_data);
}

#pragma.pack()
}  // namespace protocal_common

#endif