#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "Protocol.h" // 假设 InternalFrameHeader 定义在此
#include "common.h"

namespace protocol_common {

    // auto2infosec_fault_upload 协议，1Hz 上传传感器故障状态
    class __attribute__((packed)) auto2infosec_fault_upload {
#pragma region 传感器故障上传数据
    public:
        // 帧头
        InternalFrameHeader header;

        // 正前激光雷达 (0-无故障，1-有故障)
        uint8_t front_lidar = 0;

        // 左前激光雷达
        uint8_t left_front_lidar = 0;

        // 右前激光雷达
        uint8_t right_front_lidar = 0;

        // 正前毫米波雷达
        uint8_t front_mmw_radar = 0;

        // 左前毫米波雷达
        uint8_t left_front_mmw_radar = 0;

        // 右前毫米波雷达
        uint8_t right_front_mmw_radar = 0;

        // 前中长焦相机60
        uint8_t front_center_tele_camera_60 = 0;

        // 前中周视相机120
        uint8_t front_center_wide_camera_120 = 0;

        // 前左周视相机100
        uint8_t front_left_wide_camera_100 = 0;

        // 前右周视相机100
        uint8_t front_right_wide_camera_100 = 0;

        // 左侧前视相机60
        uint8_t left_front_camera_60 = 0;

        // 左侧后视相机100
        uint8_t left_rear_camera_100 = 0;

        // 右侧前视相机60
        uint8_t right_front_camera_60 = 0;

        // 右侧后视相机100
        uint8_t right_rear_camera_100 = 0;

        // 后中周视相机120
        uint8_t rear_center_wide_camera_120 = 0;

        // 前鱼眼相机200
        uint8_t front_fisheye_camera_200 = 0;

        // 左鱼眼相机200
        uint8_t left_fisheye_camera_200 = 0;

        // 右鱼眼相机200
        uint8_t right_fisheye_camera_200 = 0;

        // 后鱼眼相机200
        uint8_t rear_fisheye_camera_200 = 0;

        // 正前微光相机
        uint8_t front_lowlight_camera = 0;

        // 正后微光相机
        uint8_t rear_lowlight_camera = 0;

        // 惯性测量单元 (IMU)
        uint8_t imu = 0;

#pragma endregion
    };

    // msg_parse 特化
    template <>
    inline auto2infosec_fault_upload msg_parse<auto2infosec_fault_upload>(const uint8_t *buffer, size_t size,
                                                                          size_t &offset) {
        auto2infosec_fault_upload obj;

        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.front_lidar, buffer, size, offset);
        read_le(obj.left_front_lidar, buffer, size, offset);
        read_le(obj.right_front_lidar, buffer, size, offset);
        read_le(obj.front_mmw_radar, buffer, size, offset);
        read_le(obj.left_front_mmw_radar, buffer, size, offset);
        read_le(obj.right_front_mmw_radar, buffer, size, offset);
        read_le(obj.front_center_tele_camera_60, buffer, size, offset);
        read_le(obj.front_center_wide_camera_120, buffer, size, offset);
        read_le(obj.front_left_wide_camera_100, buffer, size, offset);
        read_le(obj.front_right_wide_camera_100, buffer, size, offset);
        read_le(obj.left_front_camera_60, buffer, size, offset);
        read_le(obj.left_rear_camera_100, buffer, size, offset);
        read_le(obj.right_front_camera_60, buffer, size, offset);
        read_le(obj.right_rear_camera_100, buffer, size, offset);
        read_le(obj.rear_center_wide_camera_120, buffer, size, offset);
        read_le(obj.front_fisheye_camera_200, buffer, size, offset);
        read_le(obj.left_fisheye_camera_200, buffer, size, offset);
        read_le(obj.right_fisheye_camera_200, buffer, size, offset);
        read_le(obj.rear_fisheye_camera_200, buffer, size, offset);
        read_le(obj.front_lowlight_camera, buffer, size, offset);
        read_le(obj.rear_lowlight_camera, buffer, size, offset);
        read_le(obj.imu, buffer, size, offset);
        return obj;
    }

    // msg_pack 特化
    template <>
    inline void msg_pack<auto2infosec_fault_upload>(const auto2infosec_fault_upload &obj,
                                                    std::vector<uint8_t> &           buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.front_lidar, buffer);
        msg_pack(obj.left_front_lidar, buffer);
        msg_pack(obj.right_front_lidar, buffer);
        msg_pack(obj.front_mmw_radar, buffer);
        msg_pack(obj.left_front_mmw_radar, buffer);
        msg_pack(obj.right_front_mmw_radar, buffer);
        msg_pack(obj.front_center_tele_camera_60, buffer);
        msg_pack(obj.front_center_wide_camera_120, buffer);
        msg_pack(obj.front_left_wide_camera_100, buffer);
        msg_pack(obj.front_right_wide_camera_100, buffer);
        msg_pack(obj.left_front_camera_60, buffer);
        msg_pack(obj.left_rear_camera_100, buffer);
        msg_pack(obj.right_front_camera_60, buffer);
        msg_pack(obj.right_rear_camera_100, buffer);
        msg_pack(obj.rear_center_wide_camera_120, buffer);
        msg_pack(obj.front_fisheye_camera_200, buffer);
        msg_pack(obj.left_fisheye_camera_200, buffer);
        msg_pack(obj.right_fisheye_camera_200, buffer);
        msg_pack(obj.rear_fisheye_camera_200, buffer);
        msg_pack(obj.front_lowlight_camera, buffer);
        msg_pack(obj.rear_lowlight_camera, buffer);
        msg_pack(obj.imu, buffer);
    }
} // namespace protocol_common