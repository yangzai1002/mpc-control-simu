#ifndef _MANAGER_AUTO_H_
#define _MANAGER_AUTO_H_

#include <stdio.h>
#include <string.h>

#include <nlohmann/json.hpp>
#include <vector>

#include "protocol/Protocol.h"
#include "common.h"

namespace protocol_common {
#pragma.pack(1)

// 经纬高结构体（经度、纬度、高度）
struct manager2auto_geo_point {
  // 经度，精确到小数点后6位，-180e6~180e6，181000000为无效值
  int32_t longitude = 181000000;

  // 纬度，精确到小数点后6位，-90e6~90*e6，91000000为无效值
  int32_t latitude = 181000000;

  // 高程，精确到小数点后1位，-1000e1~30000e1，999999为无效值
  int32_t altitude = 999999;
};

// 姿态角结构体（航向角、俯仰角、倾斜角）
struct manager2auto_pose {
  // 航向角，单位度，精度小数点后2位，0e2~360e2，36100为无效值
  uint16_t heading = 36100;

  // 俯仰角，单位度，精度小数点后2位，-90e2~90e2，9100为无效值
  int16_t pitch = 9100;

  // 倾斜角，单位度，精度小数点后2位，-90e2~90e2，9100为无效值
  int16_t roll = 9100;
};

// 编队配置参数结构体（IP、横向偏移量、纵向偏移量）
struct manager2auto_formation_config {
  // 行动成员ip， 将ip str通过 inet_addr 转换为uint32_t
  uint32_t member_ip = 0;

  // 横向相对偏移量，单位米，面向行使方向，左为正，精度小数点后1位，1e1
  int16_t offset_x = 0;

  // 纵向相对偏移量，单位米，面向行使方向，前为正，精度小数点后1位，1e1
  int16_t offset_y = 0;
};

// 任务点参数结构体（坐标系、路点属性、经纬高、姿态角、接受半径误差范围、接受姿态误差范围、编队机动车辆数量、多车队形信息）
struct manager2auto_task_point {
  // 坐标系
  // 0-WGS84,1-墨卡托，2-LocalNED，3-LocalENU，4-CGCS2000
  uint8_t coordinate_system = 0;

  // 路点属性
  // 1-必经点（位置必达），2-必经点（位置、航向必达），3-途径点
  uint8_t point_type = 0;

  // 经纬高
  manager2auto_geo_point geo_point;

  // 姿态角
  manager2auto_pose pose;

  // 可接受到达半径误差范围，精度小数点后1位，1e1
  uint16_t acceptance_radius = 0;

  // 可接受到达姿态误差范围
  manager2auto_pose acceptance_pose;

  // 编队机动车辆数量
  uint32_t formation_member_num = 0;

  // 多车队形信息
  std::vector<manager2auto_formation_config> formation_config;

  // 预留位
  uint32_t reserved1 = 0;
  uint32_t reserved2 = 0;
  uint32_t reserved3 = 0;
};

// 轨迹点信息结构体（时间戳、经纬高、速度、曲率、航向）
struct manager2auto_trajectory_point {
  // 时间戳，轨迹预测的时间,utc时间，精度至少到毫秒级
  uint64_t timestamp = 0;

  // 经纬高
  manager2auto_geo_point geo_point;

  // 速度，m/s,精度小数点后1位，1e1
  int16_t speed = 0;

  // 曲率，左正右副，精度小数点后3位，1e3
  int16_t curvature = 0;

  // 航向，单位度，精度小数点后2位，0e2~360e2，36100为无效值
  uint16_t heading = 36100;
};

// 编队轨迹消息结构体（车ip、轨迹点数量、轨迹点列表）
struct manager2auto_formation_trajectory {
  // 车ip, 将ip str通过inet_addr转换为uint32_t
  uint32_t member_ip = 0;

  // 轨迹点数量
  uint32_t trajectory_points_num = 0;

  // 轨迹点列表
  std::vector<manager2auto_trajectory_point> trajectory_point;
};

// 编队定位数据结构体（行动成员ip、定位数据）
struct manager2auto_formation_localization {
  // 行动成员ip， 将ip str通过 inet_addr 转换为uint32_t
  uint32_t member_ip = 0;

  // 定位数据, 结构体内curvature无意义
  manager2auto_trajectory_point trajectory_points;
};

// 视频处理位置结构体（左上角x坐标、左上角y坐标、右下角x坐标、右下角y坐标）
struct manager2auto_video_point {
  // 左上角x坐标，单位像素
  int16_t top_left_x = 0;

  // 左上角y坐标，单位像素
  int16_t top_left_y = 0;

  // 右下角x坐标，单位像素
  int16_t bottom_right_x = 0;

  // 右下角y坐标，单位像素
  int16_t bottom_right_y = 0;
};

// 视频剪接结构体（视窗位置、视频通道、视频裁减位置）
struct manager2auto_video_splicing {
  // 视窗位置：视频在操控屏幕的显示位置（默认1920*1080）
  manager2auto_video_point video_position;

  // 视频通道
  // 0：无相机，1：前主相机，2：左前后视相机，3：右前后视相机，4：左前前视相机，5：右前前视相机，6：后视相机，7：环视相机
  // 8：前视夜视相机，9：后视夜视相机，10：栅格地图
  uint8_t video_channel = 0;

  // 视频裁减位置
  manager2auto_video_point video_crop_position;
};

// 感知目标结构体（目标ID、目标类型、置信度、经纬高、速度、航向角、相对位置X、相对位置Y、相对位置Z、相对速度VX、相对速度VY、绝对位置X、绝对位置Y、绝对速度VX、绝对速度VY、观测尺寸长度、观测尺寸宽度、观测尺寸高度）
struct manager2auto_perception_target {
  // 目标ID
  uint16_t target_id = 0;

  // 目标类型
  // 1-车辆，2-人员
  uint8_t target_type = 0;

  // 置信度，1~100
  uint8_t confidence = 0;

  // 经纬高
  manager2auto_geo_point geopoint;

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

// 行动配置响应服务（行动编号、行动模式、降级模式）
// （uint32）返回值0表示成功，返回值1表示失败
class manager2auto_aciton_config {
#pragma region 行动类型配置
 public:
  // 帧头
  InternalFrameHeader header;

  // 行动编号
  uint64_t action_id = 0;

  // 行动模式
  // 1-目标点机动，2-目标跟随，3-编队机动，4-返航
  uint8_t task_mode = 0;

  // 降级模式
  // 0- 正常模式（避障）；1- 降级模式（不避障）
  uint8_t degrade_mode = 0;
#pragma endregion
};

// 目标点机动行动响应服务（行动编号、目标点数量、目标点列表、是否使用路网）
// （uint32）返回值0表示成功，返回值800表示有行动执行中，返回值801表示未设置行动点
class manager2auto_target_points_action {
#pragma region 目标点机动行动
 public:
  // 帧头
  InternalFrameHeader header;

  // 行动编号
  uint64_t action_id = 0;

  // 目标点数量
  uint32_t target_points_num = 0;

  // 目标点列表
  std::vector<manager2auto_task_point> point_attributes;

  // 是否使用路网
  // 0-不使用路网，1-使用路网
  uint8_t use_roadnet = false;
#pragma endregion
};

// 姿态调整响应服务（目标姿态、可接受到达姿态误差范围）
// 152使用：先下发姿态调整，接收目标位置，再下发机动任务
// cxy使用：直接进行调整
class manager2auto_pose_adjust {
#pragma region 姿态调整
 public:
  // 帧头
  InternalFrameHeader header;

  // 目标姿态
  manager2auto_pose target_pose;

  // 可接受到达姿态误差范围
  manager2auto_pose acceptance_pose;
#pragma endregion
};

// 目标跟随行动响应服务（行动编号、目标像素位置x、目标像素位置y、最小安全距离、最大跟随距离）
// 目标像素位置确定跟随目标，需要在退流视频中哦评估年显示+在目标中心
// （uint32）返回值0表示成功，返回值800表示有行动执行中，返回值803表示跟踪目标ID无效
class manager2auto_follow_target_action {
#pragma region 目标跟随
 public:
  // 帧头
  InternalFrameHeader header;

  // 行动编号
  uint64_t action_id = 0;

  // 目标像素位置x
  int16_t pixel_x = 0;

  // 目标像素位置y
  int16_t pixel_y = 0;

  // 最小安全距离，单位米，精度小数点后1位，1e1
  uint16_t safe_distance = 0;

  // 最大跟随距离，单位米，精度小数点后1位，1e1
  uint16_t max_distance = 0;
#pragma endregion
};

// 返航点设置响应服务
// （uint32）返回值0表示成功，返回值1表示失败
class manager2auto_return_point_set {
#pragma region 返航点设置
 public:
  // 帧头
  InternalFrameHeader header;

  // 返航标识
  // 0：无效，1：有效
  uint8_t flag = 0;
#pragma endregion
};

// 返航行动响应服务（行动编号、返航模式）
// （uint32）返回值0表示成功，返回值800表示有行动执行中，返回值801表示未设置行动点
class manager2auto_return_action {
#pragma region 返航行动
 public:
  // 帧头
  InternalFrameHeader header;

  // 行动编号
  uint64_t action_id = 0;

  // 返航模式
  // 0：历史路径返航、1：重规划返航
  uint8_t return_type = 0;
#pragma endregion
};

// 编队机动行动响应服务（行动编号、编队模式、编队车辆数量、本相对于头车的队形信息、行动点数量、编队行动点）
// （uint32）返回值0表示成功，返回值800表示有行动执行中，返回值801表示未设置行动点
class manager2auto_formation_action {
#pragma region 编队机动
 public:
  // 帧头
  InternalFrameHeader header;

  // 行动编号
  uint64_t action_id = 0;

  // 编队模式，默认为1
  // 0=引导车模式；1=引导路径模式
  uint8_t formation_mode = 1;

  // 编队车辆数量
  uint32_t formation_member_num = 0;

  // 本相对于头车的队形信息
  // 当编队模式formation_mode为0时有效
  std::vector<manager2auto_formation_config> formation_config;

  // 行动点数量
  uint32_t formation_point_num = 0;

  // 编队行动点
  std::vector<manager2auto_task_point> formation_point;
#pragma endregion
};

// 编队轨迹订阅服务（编队车辆数量、多车轨迹信息）5Hz
class manager2auto_formation_trajectory_publish {
#pragma region 编队轨迹订阅
 public:
  // 帧头
  InternalFrameHeader header;

  // 编队车辆数量
  uint32_t formation_member_num = 0;

  // 多车轨迹信息
  std::vector<manager2auto_formation_trajectory> formation_trajectory;
#pragma endregion
};

// 编队定位订阅服务（编队车辆数量、多车定位信息）10Hz
class manager2auto_formation_position {
#pragma region 编队定位订阅
 public:
  // 帧头
  InternalFrameHeader header;

  // 编队车辆数量
  uint32_t formation_member_num = 0;

  // 多车定位信息
  std::vector<manager2auto_formation_localization> formation_positon;
#pragma endregion
};

// 行动指令控制响应服务（行动编号、行动控制指令）
// （uint32）返回值0表示成功，返回值1表示失败
class manger2auto_commond_control {
#pragma region 行动指令控制
 public:
  // 帧头
  InternalFrameHeader header;

  // 行动编号
  uint64_t action_id = 0;

  // 行动控制指令
  // 1-开始，2-暂停，3-恢复，4-停止
  uint8_t commond = 0;

#pragma endregion
};

// 车辆速度上限配置响应服务（速度上限）
// （uint32）返回值0表示成功，返回值1表示失败
class manager2auto_speed_limit_config {
#pragma region 车辆速度上限配置
 public:
  // 帧头
  InternalFrameHeader header;

  // 速度上限，单位米/秒，精度小数点后1位，1e1
  uint16_t speed_limit = 0;
#pragma endregion
};

// 人机共驾模式订阅服务（心跳、人机共驾模式）
// （uint32）返回值0表示成功，返回值1表示失败
class manager2auto_human_driving_mode {
#pragma region 人机共驾模式
 public:
  // 帧头
  InternalFrameHeader header;

  // 心跳
  uint8_t heart = 0;

  // 人机共驾模式
  // 0-纯自主，1-遥控干预，2-主动应急避险
  uint8_t mode = 0;
#pragma endregion
};

// 摇杆控制订阅服务，10Hz（纵向摇杆量、横向摇杆量、横摆角速度）
class manager2auto_joystick_control {
#pragma region 摇杆控制
 public:
  // 帧头
  InternalFrameHeader header;

  // 纵向摇杆量/油门制动，[-100,100]
  int8_t joystick_x = 0;

  // 横向摇杆量/方向盘，[-100,100]，左正右负
  int8_t joystick_y = 0;

  // 横摆角速度，[-100,100]，单位°/s，预留，默认发0
  int8_t angular_speed = 0;

  // 预留
  int8_t reserved1 = 0;
  int8_t reserved2 = 0;
  int8_t reserved3 = 0;
  int8_t reserved4 = 0;
#pragma endregion
};

// 紧急制动响应服务（紧急制动标识）
// （uint32）返回值0表示成功，返回值1表示失败
class manager2auto_emergency_brake {
#pragma region 紧急制动
 public:
  // 帧头
  InternalFrameHeader header;

  // 紧急制动标识
  // 0-不制动，1-紧急制动
  uint8_t emergancy_stop = 0;
#pragma endregion
};

// 视频数据传输使能响应服务（视频使能标识）
// （uint32）返回值0表示成功，返回值1表示失败
class manager2auto_video_enable {
#pragma region 视频数据传输使能
 public:
  // 帧头
  InternalFrameHeader header;

  // 视频使能标识
  uint8_t video_enable = 1;
#pragma endregion
};

// 视频画面设置响应服务（分辨率、码率、帧率、分割裁减标志位、拼接视频数量、视频拼接设置）
// （uint32）返回值0表示成功，返回值1表示失败
class manager2auto_video_set {
#pragma region 视频画面设置
 public:
  // 帧头
  InternalFrameHeader header;

  // 分辨率
  // 0-1920*1080
  uint8_t resolution = 0;

  // 码率
  // 0-自适应，1-256k，2-512k，3-1M，4-2M
  uint8_t bps = 0;

  // 帧率
  uint8_t fps = 0;

  // 分割裁减标志位
  // 0-不调整拼接视频，1-调整拼接视频
  uint8_t splicing_flag = 0;

  // 拼接视频数量
  uint32_t video_num = 0;

  // 视频拼接设置
  std::vector<manager2auto_video_splicing> video_splicing;
#pragma endregion
};

// 视频辅助形式响应服务（遥控辅助线、遥控预警信息、自主辅助线）
// （uint32）返回值0表示成功，返回值1表示失败
class manager2auto_video_assist {
#pragma region 视频辅助形式
 public:
  // 帧头
  InternalFrameHeader header;

  // 遥控辅助线
  uint8_t remote_assist_line = 0;

  // 遥控预警信息
  uint8_t remote_warning_info = 0;

  // 自主辅助线
  uint8_t auto_assist_line = 0;
#pragma endregion
};

// manager2auto_geo_point
template <>
inline manager2auto_geo_point msg_parse<manager2auto_geo_point>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_geo_point obj;
  memcpy(&obj.longitude, buffer + offset, sizeof(obj.longitude));
  offset += sizeof(obj.longitude);
  memcpy(&obj.latitude, buffer + offset, sizeof(obj.latitude));
  offset += sizeof(obj.latitude);
  memcpy(&obj.altitude, buffer + offset, sizeof(obj.altitude));
  offset += sizeof(obj.altitude);
  return obj;
}

// manager2auto_pose
template <>
inline manager2auto_pose msg_parse<manager2auto_pose>(const uint8_t *buffer,
                                                      size_t &offset) {
  manager2auto_pose obj;
  memcpy(&obj.heading, buffer + offset, sizeof(obj.heading));
  offset += sizeof(obj.heading);
  memcpy(&obj.pitch, buffer + offset, sizeof(obj.pitch));
  offset += sizeof(obj.pitch);
  memcpy(&obj.roll, buffer + offset, sizeof(obj.roll));
  offset += sizeof(obj.roll);
  return obj;
}

// manager2auto_formation_config
template <>
inline manager2auto_formation_config msg_parse<manager2auto_formation_config>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_formation_config obj;
  memcpy(&obj.member_ip, buffer + offset, sizeof(obj.member_ip));
  offset += sizeof(obj.member_ip);
  memcpy(&obj.offset_x, buffer + offset, sizeof(obj.offset_x));
  offset += sizeof(obj.offset_x);
  memcpy(&obj.offset_y, buffer + offset, sizeof(obj.offset_y));
  offset += sizeof(obj.offset_y);
  return obj;
}

// manager2auto_task_point
template <>
inline manager2auto_task_point msg_parse<manager2auto_task_point>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_task_point obj;
  memcpy(&obj.coordinate_system, buffer + offset,
         sizeof(obj.coordinate_system));
  offset += sizeof(obj.coordinate_system);
  memcpy(&obj.point_type, buffer + offset, sizeof(obj.point_type));
  offset += sizeof(obj.point_type);
  obj.geo_point = msg_parse<manager2auto_geo_point>(buffer, offset);
  obj.pose = msg_parse<manager2auto_pose>(buffer, offset);
  memcpy(&obj.acceptance_radius, buffer + offset,
         sizeof(obj.acceptance_radius));
  offset += sizeof(obj.acceptance_radius);
  obj.acceptance_pose = msg_parse<manager2auto_pose>(buffer, offset);
  memcpy(&obj.formation_member_num, buffer + offset,
         sizeof(obj.formation_member_num));
  offset += sizeof(obj.formation_member_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.formation_config.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.formation_config[i] =
        msg_parse<manager2auto_formation_config>(buffer, offset);
  }

  memcpy(&obj.reserved1, buffer + offset, sizeof(obj.reserved1));
  offset += sizeof(obj.reserved1);
  memcpy(&obj.reserved2, buffer + offset, sizeof(obj.reserved2));
  offset += sizeof(obj.reserved2);
  memcpy(&obj.reserved3, buffer + offset, sizeof(obj.reserved3));
  offset += sizeof(obj.reserved3);
  return obj;
}

// manager2auto_trajectory_point
template <>
inline manager2auto_trajectory_point msg_parse<manager2auto_trajectory_point>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_trajectory_point obj;
  memcpy(&obj.timestamp, buffer + offset, sizeof(obj.timestamp));
  offset += sizeof(obj.timestamp);
  obj.geo_point = msg_parse<manager2auto_geo_point>(buffer, offset);
  memcpy(&obj.speed, buffer + offset, sizeof(obj.speed));
  offset += sizeof(obj.speed);
  memcpy(&obj.curvature, buffer + offset, sizeof(obj.curvature));
  offset += sizeof(obj.curvature);
  memcpy(&obj.heading, buffer + offset, sizeof(obj.heading));
  offset += sizeof(obj.heading);
  return obj;
}

// manager2auto_formation_trajectory
template <>
inline manager2auto_formation_trajectory
msg_parse<manager2auto_formation_trajectory>(const uint8_t *buffer,
                                             size_t &offset) {
  manager2auto_formation_trajectory obj;
  memcpy(&obj.member_ip, buffer + offset, sizeof(obj.member_ip));
  offset += sizeof(obj.member_ip);
  memcpy(&obj.trajectory_points_num, buffer + offset,
         sizeof(obj.trajectory_points_num));
  offset += sizeof(obj.trajectory_points_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.trajectory_point.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.trajectory_point[i] =
        msg_parse<manager2auto_trajectory_point>(buffer, offset);
  }
  return obj;
}

// manager2auto_formation_localization
template <>
inline manager2auto_formation_localization
msg_parse<manager2auto_formation_localization>(const uint8_t *buffer,
                                               size_t &offset) {
  manager2auto_formation_localization obj;
  memcpy(&obj.member_ip, buffer + offset, sizeof(obj.member_ip));
  offset += sizeof(obj.member_ip);
  obj.trajectory_points =
      msg_parse<manager2auto_trajectory_point>(buffer, offset);
  return obj;
}

// manager2auto_video_point
template <>
inline manager2auto_video_point msg_parse<manager2auto_video_point>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_video_point obj;
  memcpy(&obj.top_left_x, buffer + offset, sizeof(obj.top_left_x));
  offset += sizeof(obj.top_left_x);
  memcpy(&obj.top_left_y, buffer + offset, sizeof(obj.top_left_y));
  offset += sizeof(obj.top_left_y);
  memcpy(&obj.bottom_right_x, buffer + offset, sizeof(obj.bottom_right_x));
  offset += sizeof(obj.bottom_right_x);
  memcpy(&obj.bottom_right_y, buffer + offset, sizeof(obj.bottom_right_y));
  offset += sizeof(obj.bottom_right_y);
  return obj;
}

// manager2auto_video_splicing
template <>
inline manager2auto_video_splicing msg_parse<manager2auto_video_splicing>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_video_splicing obj;
  obj.video_position = msg_parse<manager2auto_video_point>(buffer, offset);
  memcpy(&obj.video_channel, buffer + offset, sizeof(obj.video_channel));
  offset += sizeof(obj.video_channel);
  obj.video_crop_position = msg_parse<manager2auto_video_point>(buffer, offset);
  return obj;
}

// manager2auto_perception_target
template <>
inline manager2auto_perception_target msg_parse<manager2auto_perception_target>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_perception_target obj;
  memcpy(&obj.target_id, buffer + offset, sizeof(obj.target_id));
  offset += sizeof(obj.target_id);
  memcpy(&obj.target_type, buffer + offset, sizeof(obj.target_type));
  offset += sizeof(obj.target_type);
  memcpy(&obj.confidence, buffer + offset, sizeof(obj.confidence));
  offset += sizeof(obj.confidence);
  obj.geopoint = msg_parse<manager2auto_geo_point>(buffer, offset);
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

// manager2auto_aciton_config
template <>
inline manager2auto_aciton_config msg_parse<manager2auto_aciton_config>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_aciton_config obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.action_id, buffer + offset, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(&obj.task_mode, buffer + offset, sizeof(obj.task_mode));
  offset += sizeof(obj.task_mode);
  memcpy(&obj.degrade_mode, buffer + offset, sizeof(obj.degrade_mode));
  offset += sizeof(obj.degrade_mode);
  return obj;
}

// manager2auto_target_points_action
template <>
inline manager2auto_target_points_action
msg_parse<manager2auto_target_points_action>(const uint8_t *buffer,
                                             size_t &offset) {
  manager2auto_target_points_action obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.action_id, buffer + offset, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(&obj.target_points_num, buffer + offset,
         sizeof(obj.target_points_num));
  offset += sizeof(obj.target_points_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.point_attributes.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.point_attributes[i] =
        msg_parse<manager2auto_task_point>(buffer, offset);
  }

  memcpy(&obj.use_roadnet, buffer + offset, sizeof(obj.use_roadnet));
  offset += sizeof(obj.use_roadnet);
  return obj;
}

// manager2auto_pose_adjust
template <>
inline manager2auto_pose_adjust msg_parse<manager2auto_pose_adjust>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_pose_adjust obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  obj.target_pose = msg_parse<manager2auto_pose>(buffer, offset);
  obj.acceptance_pose = msg_parse<manager2auto_pose>(buffer, offset);
  return obj;
}

// manager2auto_follow_target_action
template <>
inline manager2auto_follow_target_action
msg_parse<manager2auto_follow_target_action>(const uint8_t *buffer,
                                             size_t &offset) {
  manager2auto_follow_target_action obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.action_id, buffer + offset, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(&obj.pixel_x, buffer + offset, sizeof(obj.pixel_x));
  offset += sizeof(obj.pixel_x);
  memcpy(&obj.pixel_y, buffer + offset, sizeof(obj.pixel_y));
  offset += sizeof(obj.pixel_y);
  memcpy(&obj.safe_distance, buffer + offset, sizeof(obj.safe_distance));
  offset += sizeof(obj.safe_distance);
  memcpy(&obj.max_distance, buffer + offset, sizeof(obj.max_distance));
  offset += sizeof(obj.max_distance);
  return obj;
}

// manager2auto_return_point_set
template <>
inline manager2auto_return_point_set msg_parse<manager2auto_return_point_set>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_return_point_set obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.flag, buffer + offset, sizeof(obj.flag));
  offset += sizeof(obj.flag);
  return obj;
}

// manager2auto_return_action
template <>
inline manager2auto_return_action msg_parse<manager2auto_return_action>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_return_action obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.action_id, buffer + offset, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(&obj.return_type, buffer + offset, sizeof(obj.return_type));
  offset += sizeof(obj.return_type);
  return obj;
}

// manager2auto_formation_action
template <>
inline manager2auto_formation_action msg_parse<manager2auto_formation_action>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_formation_action obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.action_id, buffer + offset, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(&obj.formation_mode, buffer + offset, sizeof(obj.formation_mode));
  offset += sizeof(obj.formation_mode);
  memcpy(&obj.formation_member_num, buffer + offset,
         sizeof(obj.formation_member_num));
  offset += sizeof(obj.formation_member_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.formation_config.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.formation_config[i] =
        msg_parse<manager2auto_formation_config>(buffer, offset);
  }

  memcpy(&obj.formation_point_num, buffer + offset,
         sizeof(obj.formation_point_num));
  offset += sizeof(obj.formation_point_num);

  uint32_t vec2_size = 0;
  memcpy(&vec2_size, buffer + offset, sizeof(vec2_size));
  offset += sizeof(vec2_size);
  obj.formation_point.resize(vec2_size);
  for (uint32_t i = 0; i < vec2_size; ++i) {
    obj.formation_point[i] = msg_parse<manager2auto_task_point>(buffer, offset);
  }
  return obj;
}

// manager2auto_formation_trajectory_publish
template <>
inline manager2auto_formation_trajectory_publish
msg_parse<manager2auto_formation_trajectory_publish>(const uint8_t *buffer,
                                                     size_t &offset) {
  manager2auto_formation_trajectory_publish obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.formation_member_num, buffer + offset,
         sizeof(obj.formation_member_num));
  offset += sizeof(obj.formation_member_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.formation_trajectory.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.formation_trajectory[i] =
        msg_parse<manager2auto_formation_trajectory>(buffer, offset);
  }
  return obj;
}

// manager2auto_formation_position
template <>
inline manager2auto_formation_position
msg_parse<manager2auto_formation_position>(const uint8_t *buffer,
                                           size_t &offset) {
  manager2auto_formation_position obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.formation_member_num, buffer + offset,
         sizeof(obj.formation_member_num));
  offset += sizeof(obj.formation_member_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.formation_positon.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.formation_positon[i] =
        msg_parse<manager2auto_formation_localization>(buffer, offset);
  }
  return obj;
}

// manger2auto_commond_control
template <>
inline manger2auto_commond_control msg_parse<manger2auto_commond_control>(
    const uint8_t *buffer, size_t &offset) {
  manger2auto_commond_control obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.action_id, buffer + offset, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(&obj.commond, buffer + offset, sizeof(obj.commond));
  offset += sizeof(obj.commond);
  return obj;
}

// manager2auto_speed_limit_config
template <>
inline manager2auto_speed_limit_config
msg_parse<manager2auto_speed_limit_config>(const uint8_t *buffer,
                                           size_t &offset) {
  manager2auto_speed_limit_config obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.speed_limit, buffer + offset, sizeof(obj.speed_limit));
  offset += sizeof(obj.speed_limit);
  return obj;
}

// manager2auto_human_driving_mode
template <>
inline manager2auto_human_driving_mode
msg_parse<manager2auto_human_driving_mode>(const uint8_t *buffer,
                                           size_t &offset) {
  manager2auto_human_driving_mode obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.heart, buffer + offset, sizeof(obj.heart));
  offset += sizeof(obj.heart);
  memcpy(&obj.mode, buffer + offset, sizeof(obj.mode));
  offset += sizeof(obj.mode);
  return obj;
}

// manager2auto_joystick_control
template <>
inline manager2auto_joystick_control msg_parse<manager2auto_joystick_control>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_joystick_control obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.joystick_x, buffer + offset, sizeof(obj.joystick_x));
  offset += sizeof(obj.joystick_x);
  memcpy(&obj.joystick_y, buffer + offset, sizeof(obj.joystick_y));
  offset += sizeof(obj.joystick_y);
  memcpy(&obj.angular_speed, buffer + offset, sizeof(obj.angular_speed));
  offset += sizeof(obj.angular_speed);
  memcpy(&obj.reserved1, buffer + offset, sizeof(obj.reserved1));
  offset += sizeof(obj.reserved1);
  memcpy(&obj.reserved2, buffer + offset, sizeof(obj.reserved2));
  offset += sizeof(obj.reserved2);
  memcpy(&obj.reserved3, buffer + offset, sizeof(obj.reserved3));
  offset += sizeof(obj.reserved3);
  memcpy(&obj.reserved4, buffer + offset, sizeof(obj.reserved4));
  offset += sizeof(obj.reserved4);
  return obj;
}

// manager2auto_emergency_brake
template <>
inline manager2auto_emergency_brake msg_parse<manager2auto_emergency_brake>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_emergency_brake obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.emergancy_stop, buffer + offset, sizeof(obj.emergancy_stop));
  offset += sizeof(obj.emergancy_stop);
  return obj;
}

// manager2auto_video_enable
template <>
inline manager2auto_video_enable msg_parse<manager2auto_video_enable>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_video_enable obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.video_enable, buffer + offset, sizeof(obj.video_enable));
  offset += sizeof(obj.video_enable);
  return obj;
}

// manager2auto_video_set
template <>
inline manager2auto_video_set msg_parse<manager2auto_video_set>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_video_set obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.resolution, buffer + offset, sizeof(obj.resolution));
  offset += sizeof(obj.resolution);
  memcpy(&obj.bps, buffer + offset, sizeof(obj.bps));
  offset += sizeof(obj.bps);
  memcpy(&obj.fps, buffer + offset, sizeof(obj.fps));
  offset += sizeof(obj.fps);
  memcpy(&obj.splicing_flag, buffer + offset, sizeof(obj.splicing_flag));
  offset += sizeof(obj.splicing_flag);
  memcpy(&obj.video_num, buffer + offset, sizeof(obj.video_num));
  offset += sizeof(obj.video_num);

  uint32_t vec_size = 0;
  memcpy(&vec_size, buffer + offset, sizeof(vec_size));
  offset += sizeof(vec_size);
  obj.video_splicing.resize(vec_size);
  for (uint32_t i = 0; i < vec_size; ++i) {
    obj.video_splicing[i] =
        msg_parse<manager2auto_video_splicing>(buffer, offset);
  }
  return obj;
}

// manager2auto_video_assist
template <>
inline manager2auto_video_assist msg_parse<manager2auto_video_assist>(
    const uint8_t *buffer, size_t &offset) {
  manager2auto_video_assist obj;
  obj.header = msg_parse<InternalFrameHeader>(buffer, offset);
  memcpy(&obj.remote_assist_line, buffer + offset,
         sizeof(obj.remote_assist_line));
  offset += sizeof(obj.remote_assist_line);
  memcpy(&obj.remote_warning_info, buffer + offset,
         sizeof(obj.remote_warning_info));
  offset += sizeof(obj.remote_warning_info);
  memcpy(&obj.auto_assist_line, buffer + offset, sizeof(obj.auto_assist_line));
  offset += sizeof(obj.auto_assist_line);
  return obj;
}

// manager2auto_geo_point
template <>
inline void msg_pack<manager2auto_geo_point>(const manager2auto_geo_point &obj,
                                             uint8_t *buffer, size_t &offset) {
  memcpy(buffer + offset, &obj.longitude, sizeof(obj.longitude));
  offset += sizeof(obj.longitude);
  memcpy(buffer + offset, &obj.latitude, sizeof(obj.latitude));
  offset += sizeof(obj.latitude);
  memcpy(buffer + offset, &obj.altitude, sizeof(obj.altitude));
  offset += sizeof(obj.altitude);
}

// manager2auto_pose
template <>
inline void msg_pack<manager2auto_pose>(const manager2auto_pose &obj,
                                        uint8_t *buffer, size_t &offset) {
  memcpy(buffer + offset, &obj.heading, sizeof(obj.heading));
  offset += sizeof(obj.heading);
  memcpy(buffer + offset, &obj.pitch, sizeof(obj.pitch));
  offset += sizeof(obj.pitch);
  memcpy(buffer + offset, &obj.roll, sizeof(obj.roll));
  offset += sizeof(obj.roll);
}

// manager2auto_formation_config
template <>
inline void msg_pack<manager2auto_formation_config>(
    const manager2auto_formation_config &obj, uint8_t *buffer, size_t &offset) {
  memcpy(buffer + offset, &obj.member_ip, sizeof(obj.member_ip));
  offset += sizeof(obj.member_ip);
  memcpy(buffer + offset, &obj.offset_x, sizeof(obj.offset_x));
  offset += sizeof(obj.offset_x);
  memcpy(buffer + offset, &obj.offset_y, sizeof(obj.offset_y));
  offset += sizeof(obj.offset_y);
}

// manager2auto_task_point
template <>
inline void msg_pack<manager2auto_task_point>(
    const manager2auto_task_point &obj, uint8_t *buffer, size_t &offset) {
  memcpy(buffer + offset, &obj.coordinate_system,
         sizeof(obj.coordinate_system));
  offset += sizeof(obj.coordinate_system);
  memcpy(buffer + offset, &obj.point_type, sizeof(obj.point_type));
  offset += sizeof(obj.point_type);
  msg_pack(obj.geo_point, buffer, offset);
  msg_pack(obj.pose, buffer, offset);
  memcpy(buffer + offset, &obj.acceptance_radius,
         sizeof(obj.acceptance_radius));
  offset += sizeof(obj.acceptance_radius);
  msg_pack(obj.acceptance_pose, buffer, offset);
  memcpy(buffer + offset, &obj.formation_member_num,
         sizeof(obj.formation_member_num));
  offset += sizeof(obj.formation_member_num);

  uint32_t vec_size = obj.formation_config.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &fc : obj.formation_config) {
    msg_pack(fc, buffer, offset);
  }

  memcpy(buffer + offset, &obj.reserved1, sizeof(obj.reserved1));
  offset += sizeof(obj.reserved1);
  memcpy(buffer + offset, &obj.reserved2, sizeof(obj.reserved2));
  offset += sizeof(obj.reserved2);
  memcpy(buffer + offset, &obj.reserved3, sizeof(obj.reserved3));
  offset += sizeof(obj.reserved3);
}

// manager2auto_trajectory_point
template <>
inline void msg_pack<manager2auto_trajectory_point>(
    const manager2auto_trajectory_point &obj, uint8_t *buffer, size_t &offset) {
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

// manager2auto_formation_trajectory
template <>
inline void msg_pack<manager2auto_formation_trajectory>(
    const manager2auto_formation_trajectory &obj, uint8_t *buffer,
    size_t &offset) {
  memcpy(buffer + offset, &obj.member_ip, sizeof(obj.member_ip));
  offset += sizeof(obj.member_ip);
  memcpy(buffer + offset, &obj.trajectory_points_num,
         sizeof(obj.trajectory_points_num));
  offset += sizeof(obj.trajectory_points_num);

  uint32_t vec_size = obj.trajectory_point.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &pt : obj.trajectory_point) {
    msg_pack(pt, buffer, offset);
  }
}

// manager2auto_formation_localization
template <>
inline void msg_pack<manager2auto_formation_localization>(
    const manager2auto_formation_localization &obj, uint8_t *buffer,
    size_t &offset) {
  memcpy(buffer + offset, &obj.member_ip, sizeof(obj.member_ip));
  offset += sizeof(obj.member_ip);
  msg_pack(obj.trajectory_points, buffer, offset);
}

// manager2auto_video_point
template <>
inline void msg_pack<manager2auto_video_point>(
    const manager2auto_video_point &obj, uint8_t *buffer, size_t &offset) {
  memcpy(buffer + offset, &obj.top_left_x, sizeof(obj.top_left_x));
  offset += sizeof(obj.top_left_x);
  memcpy(buffer + offset, &obj.top_left_y, sizeof(obj.top_left_y));
  offset += sizeof(obj.top_left_y);
  memcpy(buffer + offset, &obj.bottom_right_x, sizeof(obj.bottom_right_x));
  offset += sizeof(obj.bottom_right_x);
  memcpy(buffer + offset, &obj.bottom_right_y, sizeof(obj.bottom_right_y));
  offset += sizeof(obj.bottom_right_y);
}

// manager2auto_video_splicing
template <>
inline void msg_pack<manager2auto_video_splicing>(
    const manager2auto_video_splicing &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.video_position, buffer, offset);
  memcpy(buffer + offset, &obj.video_channel, sizeof(obj.video_channel));
  offset += sizeof(obj.video_channel);
  msg_pack(obj.video_crop_position, buffer, offset);
}

// manager2auto_perception_target
template <>
inline void msg_pack<manager2auto_perception_target>(
    const manager2auto_perception_target &obj, uint8_t *buffer,
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

// manager2auto_aciton_config
template <>
inline void msg_pack<manager2auto_aciton_config>(
    const manager2auto_aciton_config &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.action_id, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(buffer + offset, &obj.task_mode, sizeof(obj.task_mode));
  offset += sizeof(obj.task_mode);
  memcpy(buffer + offset, &obj.degrade_mode, sizeof(obj.degrade_mode));
  offset += sizeof(obj.degrade_mode);
}

// manager2auto_target_points_action
template <>
inline void msg_pack<manager2auto_target_points_action>(
    const manager2auto_target_points_action &obj, uint8_t *buffer,
    size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.action_id, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(buffer + offset, &obj.target_points_num,
         sizeof(obj.target_points_num));
  offset += sizeof(obj.target_points_num);

  uint32_t vec_size = obj.point_attributes.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &pt : obj.point_attributes) {
    msg_pack(pt, buffer, offset);
  }

  memcpy(buffer + offset, &obj.use_roadnet, sizeof(obj.use_roadnet));
  offset += sizeof(obj.use_roadnet);
}

// manager2auto_pose_adjust
template <>
inline void msg_pack<manager2auto_pose_adjust>(
    const manager2auto_pose_adjust &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  msg_pack(obj.target_pose, buffer, offset);
  msg_pack(obj.acceptance_pose, buffer, offset);
}

// manager2auto_follow_target_action
template <>
inline void msg_pack<manager2auto_follow_target_action>(
    const manager2auto_follow_target_action &obj, uint8_t *buffer,
    size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.action_id, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(buffer + offset, &obj.pixel_x, sizeof(obj.pixel_x));
  offset += sizeof(obj.pixel_x);
  memcpy(buffer + offset, &obj.pixel_y, sizeof(obj.pixel_y));
  offset += sizeof(obj.pixel_y);
  memcpy(buffer + offset, &obj.safe_distance, sizeof(obj.safe_distance));
  offset += sizeof(obj.safe_distance);
  memcpy(buffer + offset, &obj.max_distance, sizeof(obj.max_distance));
  offset += sizeof(obj.max_distance);
}

// manager2auto_return_point_set
template <>
inline void msg_pack<manager2auto_return_point_set>(
    const manager2auto_return_point_set &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.flag, sizeof(obj.flag));
  offset += sizeof(obj.flag);
}

// manager2auto_return_action
template <>
inline void msg_pack<manager2auto_return_action>(
    const manager2auto_return_action &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.action_id, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(buffer + offset, &obj.return_type, sizeof(obj.return_type));
  offset += sizeof(obj.return_type);
}

// manager2auto_formation_action
template <>
inline void msg_pack<manager2auto_formation_action>(
    const manager2auto_formation_action &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.action_id, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(buffer + offset, &obj.formation_mode, sizeof(obj.formation_mode));
  offset += sizeof(obj.formation_mode);
  memcpy(buffer + offset, &obj.formation_member_num,
         sizeof(obj.formation_member_num));
  offset += sizeof(obj.formation_member_num);

  uint32_t vec_size = obj.formation_config.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &fc : obj.formation_config) {
    msg_pack(fc, buffer, offset);
  }

  memcpy(buffer + offset, &obj.formation_point_num,
         sizeof(obj.formation_point_num));
  offset += sizeof(obj.formation_point_num);

  uint32_t vec2_size = obj.formation_point.size();
  memcpy(buffer + offset, &vec2_size, sizeof(vec2_size));
  offset += sizeof(vec2_size);
  for (const auto &pt : obj.formation_point) {
    msg_pack(pt, buffer, offset);
  }
}

// manager2auto_formation_trajectory_publish
template <>
inline void msg_pack<manager2auto_formation_trajectory_publish>(
    const manager2auto_formation_trajectory_publish &obj, uint8_t *buffer,
    size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.formation_member_num,
         sizeof(obj.formation_member_num));
  offset += sizeof(obj.formation_member_num);

  uint32_t vec_size = obj.formation_trajectory.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &traj : obj.formation_trajectory) {
    msg_pack(traj, buffer, offset);
  }
}

// manager2auto_formation_position
template <>
inline void msg_pack<manager2auto_formation_position>(
    const manager2auto_formation_position &obj, uint8_t *buffer,
    size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.formation_member_num,
         sizeof(obj.formation_member_num));
  offset += sizeof(obj.formation_member_num);

  uint32_t vec_size = obj.formation_positon.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &pos : obj.formation_positon) {
    msg_pack(pos, buffer, offset);
  }
}

// manger2auto_commond_control
template <>
inline void msg_pack<manger2auto_commond_control>(
    const manger2auto_commond_control &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.action_id, sizeof(obj.action_id));
  offset += sizeof(obj.action_id);
  memcpy(buffer + offset, &obj.commond, sizeof(obj.commond));
  offset += sizeof(obj.commond);
}

// manager2auto_speed_limit_config
template <>
inline void msg_pack<manager2auto_speed_limit_config>(
    const manager2auto_speed_limit_config &obj, uint8_t *buffer,
    size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.speed_limit, sizeof(obj.speed_limit));
  offset += sizeof(obj.speed_limit);
}

// manager2auto_human_driving_mode
template <>
inline void msg_pack<manager2auto_human_driving_mode>(
    const manager2auto_human_driving_mode &obj, uint8_t *buffer,
    size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.heart, sizeof(obj.heart));
  offset += sizeof(obj.heart);
  memcpy(buffer + offset, &obj.mode, sizeof(obj.mode));
  offset += sizeof(obj.mode);
}

// manager2auto_joystick_control
template <>
inline void msg_pack<manager2auto_joystick_control>(
    const manager2auto_joystick_control &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.joystick_x, sizeof(obj.joystick_x));
  offset += sizeof(obj.joystick_x);
  memcpy(buffer + offset, &obj.joystick_y, sizeof(obj.joystick_y));
  offset += sizeof(obj.joystick_y);
  memcpy(buffer + offset, &obj.angular_speed, sizeof(obj.angular_speed));
  offset += sizeof(obj.angular_speed);
  memcpy(buffer + offset, &obj.reserved1, sizeof(obj.reserved1));
  offset += sizeof(obj.reserved1);
  memcpy(buffer + offset, &obj.reserved2, sizeof(obj.reserved2));
  offset += sizeof(obj.reserved2);
  memcpy(buffer + offset, &obj.reserved3, sizeof(obj.reserved3));
  offset += sizeof(obj.reserved3);
  memcpy(buffer + offset, &obj.reserved4, sizeof(obj.reserved4));
  offset += sizeof(obj.reserved4);
}

// manager2auto_emergency_brake
template <>
inline void msg_pack<manager2auto_emergency_brake>(
    const manager2auto_emergency_brake &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.emergancy_stop, sizeof(obj.emergancy_stop));
  offset += sizeof(obj.emergancy_stop);
}

// manager2auto_video_enable
template <>
inline void msg_pack<manager2auto_video_enable>(
    const manager2auto_video_enable &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.video_enable, sizeof(obj.video_enable));
  offset += sizeof(obj.video_enable);
}

// manager2auto_video_set
template <>
inline void msg_pack<manager2auto_video_set>(const manager2auto_video_set &obj,
                                             uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.resolution, sizeof(obj.resolution));
  offset += sizeof(obj.resolution);
  memcpy(buffer + offset, &obj.bps, sizeof(obj.bps));
  offset += sizeof(obj.bps);
  memcpy(buffer + offset, &obj.fps, sizeof(obj.fps));
  offset += sizeof(obj.fps);
  memcpy(buffer + offset, &obj.splicing_flag, sizeof(obj.splicing_flag));
  offset += sizeof(obj.splicing_flag);
  memcpy(buffer + offset, &obj.video_num, sizeof(obj.video_num));
  offset += sizeof(obj.video_num);

  uint32_t vec_size = obj.video_splicing.size();
  memcpy(buffer + offset, &vec_size, sizeof(vec_size));
  offset += sizeof(vec_size);
  for (const auto &vs : obj.video_splicing) {
    msg_pack(vs, buffer, offset);
  }
}

// manager2auto_video_assist
template <>
inline void msg_pack<manager2auto_video_assist>(
    const manager2auto_video_assist &obj, uint8_t *buffer, size_t &offset) {
  msg_pack(obj.header, buffer, offset);
  memcpy(buffer + offset, &obj.remote_assist_line,
         sizeof(obj.remote_assist_line));
  offset += sizeof(obj.remote_assist_line);
  memcpy(buffer + offset, &obj.remote_warning_info,
         sizeof(obj.remote_warning_info));
  offset += sizeof(obj.remote_warning_info);
  memcpy(buffer + offset, &obj.auto_assist_line, sizeof(obj.auto_assist_line));
  offset += sizeof(obj.auto_assist_line);
}

#pragma.pack()
}  // namespace protocol_common

#endif