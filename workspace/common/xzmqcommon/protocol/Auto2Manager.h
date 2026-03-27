#pragma once

#include <cstdio>
#include <cstring>
#include <nlohmann/json.hpp>
#include <vector>

#include "FrameCommon.h"
#include "Protocol.h"
#include "common.h"
// TODO 添加编队队形信息协议，同时
namespace protocol_common {
    // 故障码结构体（故障码ID、故障等级）
    struct __attribute__(()) auto2manager_faultdata {
        // 故障码ID【详见故障码定义】
        uint32_t fault_id;

        // 故障等级
        // 1-严重（停车），2-警告（限速），3-信息（提示）
        uint8_t fault_level = 0;
    };

    // 经纬高结构体（经度、纬度、高度）
    struct __attribute__(()) auto2manager_geo_point {
        // 经度，精确到小数点后6位，-180e6~180e6，181000000为无效值
        int32_t longitude = 181000000;

        // 纬度，精确到小数点后6位，-90e6~90*e6，91000000为无效值
        int32_t latitude = 91000000;

        // 高程，精确到小数点后1位，-1000e1~30000e1，999999为无效值
        int32_t altitude = 999999;
    };

    // 轨迹点信息结构体（时间戳、经纬高、速度、曲率、航向）
    struct __attribute__(()) auto2manager_trajectory_point {
        // 时间戳，轨迹预测的时间,utc时间，精度至少到毫秒级
        uint64_t timestamp = 0;

        // 经纬高
        auto2manager_geo_point geo_point;

        // 速度，m/s,精度小数点后1位，1e1
        int16_t speed = 0;

        // 曲率，左正右负，精度小数点后3位，1e3
        int16_t curvature = 0;

        // 航向，单位度，精度小数点后2位，0e2~360e2，36100为无效值
        uint16_t heading = 36100;
    };

    // 感知目标结构体（目标ID、目标类型、置信度、经纬高、速度、航向角、相对位置X、相对位置Y、相对位置Z、相对速度VX、相对速度VY、绝对位置X、绝对位置Y、绝对速度VX、绝对速度VY、观测尺寸长度、观测尺寸宽度、观测尺寸高度）
    struct __attribute__(()) auto2manager_perception_target {
        // 目标ID
        uint32_t target_id = 0;

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

        // 相对位置X，以几何中心为原点，正前方向，单位米，精确到小数点后1位，1e1
        int16_t relative_x = 0;

        // 相对位置Y，以几何中心为原点，正左方向，单位米，精确到小数点后1位，1e1
        int16_t relative_y = 0;

        // 相对位置Z，以几何中心为原点，正上方向，单位米，精确到小数点后1位，1e1
        int16_t relative_z = 0;

        // 相对速度VX，以几何中心为原点，正前方向，单位米/秒，精度小数点后1位，1e1
        int16_t relative_speed_x = 0;

        // 相对速度VY，以几何中心为原点，正左方向，单位米/秒，精度小数点后1位，1e1
        int16_t relative_speed_y = 0;

        // 绝对位置X，以组合导航设备上电时车辆位置为原点，正右方向，单位米，精确到小数点后1位，1e1
        int32_t absolute_x = 0;

        // 绝对位置Y，以组合导航设备上电时车辆位置为原点，正前方向，单位米，精确到小数点后1位，1e1
        int32_t absolute_y = 0;

        // 绝对速度VX，以组合导航设备上电时车辆位置为原点，正右方向，单位米/秒，精度小数点后1位，1e1
        int16_t absolute_speed_x = 0;

        // 绝对速度VY，以组合导航设备上电时车辆位置为原点，正前方向，单位米/秒，精度小数点后1位，1e1
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
        /*
        软件版本：Vx.yy.zz/aaaabbcc
        Byte0：x（研制阶段）；Byte1：yy（外部评审）；
        Byte2：zz（内部管理）；Byte3：aaaa（年-1970）；Byte4：bb（月）；Byte5：cc（日）；Byte6～7：预留
        */
        uint64_t software_version = 0;

        // 降级机动开关状态：0-正常模式，1-突击模式，2-停障模式
        uint8_t degrade_mode = 0;

        // 自主机动策略：0-循迹机动，1-自主通行
        uint8_t auto_mobility_strategy = 0;

        // 感知策略：1-铺装路模式，2-越野模式，3-草地模式，4-扬尘模式
        uint8_t perception_strategy = 2; // 默认可设为2

        // 定位使用策略：目前固定为2-融合定位
        uint8_t positioning_strategy = 2;

        // 人机共驾模式：1-纯自主，2-遥控干预，3-主动应急避险
        uint8_t human_driving_mode = 1;

        // 速度上限值（单位：0.1 m/s，即 0~300 表示 0~30.0 m/s）
        uint16_t speed_limit = 0;

        // 故障数量
        uint32_t fault_num = 0;

        // 故障代码列表
        std::vector<auto2manager_faultdata> fault_list;
#pragma endregion
    };

    // 偏移量发布
    class __attribute__(()) auto2manager_offset_publish {
#pragma region 偏移量发布
    public:
        InternalFrameHeader header;

        // 偏移量
        // 绝对值，单位米， 精确到小数点后1位
        uint8_t offset = 0;

        // 可通行率
        uint8_t passable_rate = 100; // REVIEW 默认值？ 0 - 不可自主通行， 100-一路畅通
    };

    // 车辆行动状态发布服务，1Hz（行动编号、行动类型、行动状态、进度）
    class __attribute__(()) auto2manager_task_status {
#pragma region 车辆行动状态发布服务
    public:
        // 帧头
        InternalFrameHeader header;

        // 行动编号
        uint8_t action_id = 0;

        // 行动类型
        // 1-目标点机动，2-目标跟随，3-编队机动，4-返航
        uint8_t task_type = 0;

        //行动状态
        // 0-行动准备中:收到行动，但不具备执行状态，可停止行动
        // 1.等待执行:行动准备完成，可启动/停止行司
        // 2-正在执行:行动正在执行，可暂停/停止行动
        // 3-行动暂停:行动暂停中，可继续/停止行动行动因外界要求结束，不可对该行动进行操作喨描
        // 4-行动终止:
        // 5-行动完成:完成行动要求，不可对该行动进行操作
        // 6-行动失败:行动因自身原因结束，不可对该行动进行操作
        uint8_t task_status = 0;

        // 行进进度
        // [0，100]，仅定点/返航机动类行动涉及
        uint8_t progress = 0;
#pragma endregion
    };

    // 姿态调整响应服务，返回数据（推荐位置）
    // 152使用
    class __attribute__(()) auto2manager_adjust_attitude_response {
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
        // 0- failure 1- success 800-有行动执行 801-未设置行动点
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

    // 地形感知信息发布服务，10Hz【未完成，上MDC后取消注释】
    class __attribute__(()) auto2manager_terrain_info {
#pragma region 地形感知信息
    public:
        // 帧头
        InternalFrameHeader header;

        uint16_t height = 500;    // 默认地图像素高
        uint16_t width = 500;     // 默认地图像素宽
        uint8_t  channel = 3;     // 默认地图通道数
        uint16_t resolution = 20; // 默认0.2 m， 需x100发送

        // 像素数据：按行优先(channel→width→height)排列
        std::vector<uint8_t> pixel_data; // 长度 = height*width*channel
#pragma endregion
    };

    // 视频状态上报，1Hz（分辨率、帧率、码率）
    class __attribute__(()) auto2manager_video_status {
#pragma region 视频状态上报
    public:
        // 帧头
        InternalFrameHeader header;

        // 分辨率
        // 1-1920*1080(默认) 2-1280*720
        uint8_t resolution = 1;

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
    class __attribute__(()) auto2manager_control_data_upload {
#pragma region 触发控制量上传
    public:
        // 帧头
        InternalFrameHeader header;

        // 控制量编号，底盘协议Message ID 最后一位
        uint8_t message_id_data = 0;

        // 值，底盘协议PayLoad值
        uint8_t payload_data = 0;
#pragma endregion
    };

    template <>
    inline auto2manager_faultdata msg_parse<auto2manager_faultdata>(const uint8_t *buffer, std::size_t size,
                                                                    size_t &offset) {
        auto2manager_faultdata obj;
        read_le(obj.fault_id, buffer, size, offset);
        read_le(obj.fault_level, buffer, size, offset);
        return obj;
    }

    // auto2manager_geo_point
    template <>
    inline auto2manager_geo_point msg_parse<auto2manager_geo_point>(const uint8_t *buffer, std::size_t size,
                                                                    size_t &offset) {
        auto2manager_geo_point obj;
        read_le(obj.longitude, buffer, size, offset);
        read_le(obj.latitude, buffer, size, offset);
        read_le(obj.altitude, buffer, size, offset);
        return obj;
    }

    // auto2manager_trajectory_point
    template <>
    inline auto2manager_trajectory_point msg_parse<auto2manager_trajectory_point>(const uint8_t *buffer,
                                                                                  std::size_t size, size_t &offset) {
        auto2manager_trajectory_point obj;
        read_le(obj.timestamp, buffer, size, offset);
        obj.geo_point = msg_parse<auto2manager_geo_point>(buffer, size, offset);
        read_le(obj.speed, buffer, size, offset);
        read_le(obj.curvature, buffer, size, offset);
        read_le(obj.heading, buffer, size, offset);
        return obj;
    }

    // auto2manager_perception_target
    template <>
    inline auto2manager_perception_target msg_parse<auto2manager_perception_target>(const uint8_t *buffer,
                                                                                    std::size_t size, size_t &offset) {
        auto2manager_perception_target obj;
        read_le(obj.target_id, buffer, size, offset);
        read_le(obj.target_type, buffer, size, offset);
        read_le(obj.confidence, buffer, size, offset);
        obj.geopoint = msg_parse<auto2manager_geo_point>(buffer, size, offset);
        read_le(obj.speed, buffer, size, offset);
        read_le(obj.heading, buffer, size, offset);
        read_le(obj.relative_x, buffer, size, offset);
        read_le(obj.relative_y, buffer, size, offset);
        read_le(obj.relative_z, buffer, size, offset);
        read_le(obj.relative_speed_x, buffer, size, offset);
        read_le(obj.relative_speed_y, buffer, size, offset);
        read_le(obj.absolute_x, buffer, size, offset);
        read_le(obj.absolute_y, buffer, size, offset);
        read_le(obj.absolute_speed_x, buffer, size, offset);
        read_le(obj.absolute_speed_y, buffer, size, offset);
        read_le(obj.length, buffer, size, offset);
        read_le(obj.width, buffer, size, offset);
        read_le(obj.height, buffer, size, offset);
        return obj;
    }

    // auto2manager_vehicle_status
    template <>
    inline auto2manager_vehicle_status msg_parse<auto2manager_vehicle_status>(const uint8_t *buffer, std::size_t size,
                                                                              size_t &offset) {
        auto2manager_vehicle_status obj;

        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);

        read_le(obj.software_version, buffer, size, offset);
        read_le(obj.degrade_mode, buffer, size, offset);
        read_le(obj.auto_mobility_strategy, buffer, size, offset);
        read_le(obj.perception_strategy, buffer, size, offset);
        read_le(obj.positioning_strategy, buffer, size, offset);
        read_le(obj.human_driving_mode, buffer, size, offset);
        read_le(obj.speed_limit, buffer, size, offset);
        read_le(obj.fault_num, buffer, size, offset);

        obj.fault_list.resize(obj.fault_num);
        for (uint32_t i = 0; i < obj.fault_num; ++i) {
            obj.fault_list[i] = msg_parse<auto2manager_faultdata>(buffer, size, offset);
        }

        return obj;
    }

    template <>
    inline auto2manager_offset_publish msg_parse<auto2manager_offset_publish>(const uint8_t *buffer, std::size_t size,
                                                                              size_t &offset) {
        auto2manager_offset_publish obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.offset, buffer, size, offset);
        read_le(obj.passable_rate, buffer, size, offset);
        return obj;
    }

    // auto2manager_task_status
    template <>
    inline auto2manager_task_status msg_parse<auto2manager_task_status>(const uint8_t *buffer, std::size_t size,
                                                                        size_t &offset) {
        auto2manager_task_status obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.action_id, buffer, size, offset);
        read_le(obj.task_type, buffer, size, offset);
        read_le(obj.task_status, buffer, size, offset);
        read_le(obj.progress, buffer, size, offset);
        return obj;
    }

    // auto2manager_adjust_attitude_response
    template <>
    inline auto2manager_adjust_attitude_response msg_parse<auto2manager_adjust_attitude_response>(const uint8_t *buffer,
                                                                                                  std::size_t    size,
                                                                                                  size_t &offset) {
        auto2manager_adjust_attitude_response obj;
        read_le(obj.ad_return, buffer, size, offset);
        obj.recommended_position = msg_parse<auto2manager_geo_point>(buffer, size, offset);
        return obj;
    }

    // auto2manager_global_path
    template <>
    inline auto2manager_global_path msg_parse<auto2manager_global_path>(const uint8_t *buffer, std::size_t size,
                                                                        size_t &offset) {
        auto2manager_global_path obj;
        read_le(obj.ad_return, buffer, size, offset);
        read_le(obj.total_mileage, buffer, size, offset);
        read_le(obj.pathpoint_num, buffer, size, offset);

        auto vec_size = obj.pathpoint_num;
        obj.pathpoint_list.resize(vec_size);
        for (uint32_t i = 0; i < vec_size; ++i) {
            obj.pathpoint_list[i] = msg_parse<auto2manager_geo_point>(buffer, size, offset);
        }
        return obj;
    }

    // auto2manager_formation_path
    template <>
    inline auto2manager_formation_path msg_parse<auto2manager_formation_path>(const uint8_t *buffer, std::size_t size,
                                                                              size_t &offset) {
        auto2manager_formation_path obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.pathpoint_num, buffer, size, offset);

        obj.trajectory_points_list.resize(obj.pathpoint_num);
        for (uint32_t i = 0; i < obj.pathpoint_num; ++i) {
            obj.trajectory_points_list[i] = msg_parse<auto2manager_trajectory_point>(buffer, size, offset);
        }
        return obj;
    }

    // auto2manager_terrain_info
    template <>
    inline auto2manager_terrain_info msg_parse<auto2manager_terrain_info>(const uint8_t *buffer, std::size_t size,
                                                                          size_t &offset) {
        auto2manager_terrain_info obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);

        read_le(obj.height, buffer, size, offset);
        read_le(obj.width, buffer, size, offset);
        read_le(obj.channel, buffer, size, offset);
        read_le(obj.resolution, buffer, size, offset);

        const size_t pixel_cnt = obj.height * obj.width * obj.channel;
        if ((offset + pixel_cnt) > size) {
            throw std::runtime_error(
                fmt::format("buffer overflow when reading data: offset {}, need {}", offset, (offset + pixel_cnt)));
        }

        obj.pixel_data.resize(pixel_cnt);
        memcpy(obj.pixel_data.data(), buffer + offset, pixel_cnt);
        offset += pixel_cnt;
        return obj;
    }

    // auto2manager_video_status
    template <>
    inline auto2manager_video_status msg_parse<auto2manager_video_status>(const uint8_t *buffer, std::size_t size,
                                                                          size_t &offset) {
        auto2manager_video_status obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.resolution, buffer, size, offset);
        read_le(obj.bps, buffer, size, offset);
        read_le(obj.fps, buffer, size, offset);
        return obj;
    }

    // auto2manager_perception_target_publish
    template <>
    inline auto2manager_perception_target_publish msg_parse<auto2manager_perception_target_publish>(
        const uint8_t *buffer, std::size_t size, size_t &offset) {
        auto2manager_perception_target_publish obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.target_num, buffer, size, offset);

        auto vec_size = obj.target_num;
        obj.target_attributes.resize(vec_size);
        for (uint32_t i = 0; i < vec_size; ++i) {
            obj.target_attributes[i] = msg_parse<auto2manager_perception_target>(buffer, size, offset);
        }
        return obj;
    }

    // auto2manager_control_data_upload
    template <>
    inline auto2manager_control_data_upload msg_parse<auto2manager_control_data_upload>(const uint8_t *buffer,
                                                                                        std::size_t    size,
                                                                                        size_t &       offset) {
        auto2manager_control_data_upload obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.message_id_data, buffer, size, offset);
        read_le(obj.payload_data, buffer, size, offset);
        return obj;
    }

    template <>
    inline void msg_pack<auto2manager_faultdata>(const auto2manager_faultdata &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.fault_id, buffer);
        msg_pack(obj.fault_level, buffer);
    }

    template <>
    inline void msg_pack<auto2manager_geo_point>(const auto2manager_geo_point &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.longitude, buffer);
        msg_pack(obj.latitude, buffer);
        msg_pack(obj.altitude, buffer);
    }

    // auto2manager_trajectory_point
    template <>
    inline void msg_pack<auto2manager_trajectory_point>(const auto2manager_trajectory_point &obj, uint8_t *buffer,
                                                        size_t &offset) {
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
    inline void msg_pack<auto2manager_perception_target>(const auto2manager_perception_target &obj, uint8_t *buffer,
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

    template <>
    inline void msg_pack<auto2manager_perception_target>(const auto2manager_perception_target &obj,
                                                         std::vector<std::uint8_t> &           buffer) {
        msg_pack(obj.target_id, buffer);
        msg_pack(obj.target_type, buffer);
        msg_pack(obj.confidence, buffer);
        msg_pack(obj.geopoint, buffer);
        msg_pack(obj.speed, buffer);
        msg_pack(obj.heading, buffer);
        msg_pack(obj.relative_x, buffer);
        msg_pack(obj.relative_y, buffer);
        msg_pack(obj.relative_z, buffer);
        msg_pack(obj.relative_speed_x, buffer);
        msg_pack(obj.relative_speed_y, buffer);
        msg_pack(obj.absolute_x, buffer);
        msg_pack(obj.absolute_y, buffer);
        msg_pack(obj.absolute_speed_x, buffer);
        msg_pack(obj.absolute_speed_y, buffer);
        msg_pack(obj.length, buffer);
        msg_pack(obj.width, buffer);
        msg_pack(obj.height, buffer);
    }

    // auto2manager_vehicle_status
    template <>
    inline void msg_pack<auto2manager_vehicle_status>(const auto2manager_vehicle_status &obj, uint8_t *buffer,
                                                      size_t &offset) {
        msg_pack(obj.header, buffer, offset);

        memcpy(buffer + offset, &obj.software_version, sizeof(obj.software_version));
        offset += sizeof(obj.software_version);
        memcpy(buffer + offset, &obj.degrade_mode, sizeof(obj.degrade_mode));
        offset += sizeof(obj.degrade_mode);
        memcpy(buffer + offset, &obj.auto_mobility_strategy, sizeof(obj.auto_mobility_strategy));
        offset += sizeof(obj.auto_mobility_strategy);
        memcpy(buffer + offset, &obj.perception_strategy, sizeof(obj.perception_strategy));
        offset += sizeof(obj.perception_strategy);
        memcpy(buffer + offset, &obj.positioning_strategy, sizeof(obj.positioning_strategy));
        offset += sizeof(obj.positioning_strategy);
        memcpy(buffer + offset, &obj.human_driving_mode, sizeof(obj.human_driving_mode));
        offset += sizeof(obj.human_driving_mode);
        memcpy(buffer + offset, &obj.speed_limit, sizeof(obj.speed_limit));
        offset += sizeof(obj.speed_limit);
        memcpy(buffer + offset, &obj.fault_num, sizeof(obj.fault_num));
        offset += sizeof(obj.fault_num);

        for (const auto &f : obj.fault_list) {
            msg_pack(f, buffer, offset);
        }
    }

    // 使用 std::vector<uint8_t> 动态序列化 auto2manager_vehicle_status
    template <>
    inline void msg_pack<auto2manager_vehicle_status>(const auto2manager_vehicle_status &obj,
                                                      std::vector<uint8_t> &buffer) { // 接收 vector 引用，动态扩容
        // 1. 序列化嵌套的 header（假设 header 的 msg_pack 已支持 vector）
        msg_pack(obj.header, buffer);

        // 2. 序列化固定大小字段（利用基础模板函数）
        msg_pack(obj.software_version, buffer);
        msg_pack(obj.degrade_mode, buffer);
        msg_pack(obj.auto_mobility_strategy, buffer);
        msg_pack(obj.perception_strategy, buffer);
        msg_pack(obj.positioning_strategy, buffer);
        msg_pack(obj.human_driving_mode, buffer);
        msg_pack(obj.speed_limit, buffer);
        msg_pack(obj.fault_num, buffer);

        // 逐个序列化 vector 中的元素（假设 fault 的 msg_pack 已支持 vector）
        for (const auto &f : obj.fault_list) {
            msg_pack(f, buffer);
        }
    }

    // auto2manager_task_status
    template <>
    inline void msg_pack<auto2manager_task_status>(const auto2manager_task_status &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.action_id, buffer);
        msg_pack(obj.task_type, buffer);
        msg_pack(obj.task_status, buffer);
        msg_pack(obj.progress, buffer);
    }

    template <>
    inline void msg_pack<auto2manager_offset_publish>(const auto2manager_offset_publish &obj,
                                                      std::vector<uint8_t> &             buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.offset, buffer);
        msg_pack(obj.passable_rate, buffer);
    }

    template <>
    inline void msg_pack<auto2manager_adjust_attitude_response>(const auto2manager_adjust_attitude_response &obj,
                                                                std::vector<uint8_t> &                       buffer) {
        msg_pack(obj.ad_return, buffer);
        msg_pack(obj.recommended_position, buffer);
    }

    // auto2manager_global_path
    template <>
    inline void msg_pack<auto2manager_global_path>(const auto2manager_global_path &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.ad_return, buffer);
        msg_pack(obj.total_mileage, buffer);
        msg_pack(obj.pathpoint_num, buffer);

        uint32_t vec_size = obj.pathpoint_list.size();
        msg_pack(vec_size, buffer);
        for (const auto &pt : obj.pathpoint_list) {
            msg_pack(pt, buffer);
        }
    }

    // auto2manager_formation_path
    template <>
    inline void msg_pack<auto2manager_formation_path>(const auto2manager_formation_path &obj,
                                                      std::vector<uint8_t> &             buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.pathpoint_num, buffer);
        uint32_t vec_size = obj.trajectory_points_list.size();
        msg_pack(vec_size, buffer);
        for (const auto &pt : obj.trajectory_points_list) {
            msg_pack(pt, buffer);
        }
    }

    /* 如果想支持 std::vector<uint8_t> 接口，再补一个重载即可 */
    template <>
    inline void msg_pack<auto2manager_terrain_info>(const auto2manager_terrain_info &obj,
                                                    std::vector<uint8_t> &           buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.height, buffer);
        msg_pack(obj.width, buffer);
        msg_pack(obj.channel, buffer);
        msg_pack(obj.resolution, buffer);
        buffer.insert(buffer.end(), obj.pixel_data.begin(), obj.pixel_data.end());
    }

    // auto2manager_video_status
    template <>
    inline void msg_pack<auto2manager_video_status>(const auto2manager_video_status &obj,
                                                    std::vector<uint8_t> &           buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.resolution, buffer);
        msg_pack(obj.bps, buffer);
        msg_pack(obj.fps, buffer);
    }

    // auto2manager_perception_target_publish
    template <>
    inline void msg_pack<auto2manager_perception_target_publish>(const auto2manager_perception_target_publish &obj,
                                                                 std::vector<uint8_t> &                        buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.target_num, buffer);
        for (const auto &t : obj.target_attributes) {
            msg_pack(t, buffer);
        }
    }

    template <>
    inline void msg_pack<auto2manager_control_data_upload>(const auto2manager_control_data_upload &obj,
                                                           std::vector<uint8_t> &                  buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.message_id_data, buffer);
        msg_pack(obj.payload_data, buffer);
    }
} // namespace protocol_common
