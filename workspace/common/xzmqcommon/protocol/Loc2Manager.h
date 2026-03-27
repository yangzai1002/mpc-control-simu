#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <nlohmann/json.hpp>

#include "Protocol.h"
#include "common.h"

namespace protocol_common {
    // 故障码结构体（故障码、故障等级）
    struct __attribute__(()) loc2manager_fault_data {
        // 故障码,参见具体故障码定义
        uint32_t fault_code;

        // 故障等级,1-严重（停车）；2-警告（限速）；3-提示
        uint8_t fault_level;
    };

    // 经纬高结构体（经度、纬度、高度）
    struct __attribute__(()) loc2manager_geo_point {
        // 经度，精确到小数点后6位，-180e6~180e6，181000000为无效值
        int32_t longitude = 181000000;

        // 纬度，精确到小数点后6位，-90e6~90*e6，91000000为无效值
        int32_t latitude = 91000000;

        // 高程，精确到小数点后1位，-1000e1~30000e1，999999为无效值
        int32_t altitude = 999999;
    };

    // 姿态角结构体（航向角、俯仰角、倾斜角）
    struct __attribute__(()) loc2manager_pose {
        // 航向角，单位度，精度小数点后2位，0e2~360e2，36100为无效值
        uint16_t heading = 36100;

        // 俯仰角，单位度，精度小数点后2位，-90e2~90e2，9100为无效值
        int16_t pitch = 9100;

        // 倾斜角，单位度，精度小数点后2位，-90e2~90e2，9100为无效值
        int16_t roll = 9100;
    };

    // 融合定位软件接口，100Hz
    class __attribute__(()) loc2manager_basic_data {
#pragma region 融合定位数据
    public:
        // 帧头
        InternalFrameHeader header;

        // 融合定位
        loc2manager_geo_point fuse_geo;

        // 组合定位
        loc2manager_geo_point nav_geo;

        // 卫星定位
        loc2manager_geo_point sate_geo;

        // 融合结果-姿态角
        loc2manager_pose fuse_pose;

        // 融合结果-全局X轴向速度，米/秒，10^1
        int16_t fuse_vx = 0;

        // 融合结果-全局Y轴向速度，米/秒，10^1
        int16_t fuse_vy = 0;

        // 融合结果-全局Z轴向速度，米/秒，10^1
        int16_t fuse_vz = 0;

        // 融合结果-航向角速度，度/秒，10^2
        int16_t fuse_wx = 0;

        // 融合结果-俯仰角速度，度/秒，10^2
        int16_t fuse_wy = 0;

        // 融合结果-倾斜角速度，度/秒，10^2
        int16_t fuse_wz = 0;

        // 递推坐标位置X，米，10^1
        int32_t dr_x = 0;

        // 递推坐标位置Y，米，10^1
        int32_t dr_y = 0;

        // 递推坐标位置Z，米，10^1
        int32_t dr_z = 0;

        // 递推速度Vx，米/秒，10^1
        int16_t dr_vx = 0;

        // 递推速度Vy，米/秒，10^1
        int16_t dr_vy = 0;

        // 递推速度Vz，米/秒，10^1
        int16_t dr_vz = 0;

        // 递推姿态角结果
        loc2manager_pose dr_pose;

        // 递推滚转角速度，度/秒，10^2
        int16_t dr_rollrate = 0;

        // 递推俯仰角速度，度/秒，10^2
        int16_t dr_pitchrate = 0;

        // 递推航向角速度，度/秒，10^2
        int16_t dr_yawrate = 0;

        // 导航模式
        /*
        导航模式
        Bit0：纯惯性（0：关、1 开）
        Bit1：轮速计（0：关、1 开）
        Bit2：卫星（0：关、1 开）
        Bit3：激光（0：关、1 开）
        Bit4：视觉（0：关、1 开）
        Bit5：毫米波 （0：关、1 开）
        Bit6：预留
        Bit7：预留（不得与北斗惯导状态冲突）
        */
        uint8_t navstate = 0;

        // 北斗定位状态
        // 0-定位数据无效，1-单点定位，2-伪距差分定位，3-双频定位
        // 4-RTK定位固定解,5-RTK定位浮点解，6-SBAS定位，7-PPP定位（动态差分）
        // 精度：（4/7）>5>(2,6)>3>1
        uint8_t gnssstate = 0;

        // 惯导工作状态
        // 0-惯导正在启动；1-惯导启动完成；2-正在对准，卫星辅助；3-正在对准，装订模式；
        // 4-导航状态，惯导/卫星/轮速组合模式；5-导航状态，惯导/卫星组合模式；6-导航状态，惯导/轮速组合模式；
        // 7-导航状态，纯惯导模式
        uint8_t insstate = 0;

        // 惯导错误上报
        // bit0 = 1：x轴陀螺故障
        // bit1 = 1：y轴陀螺故障
        // bit2：z轴陀螺故障（俯仰角、横滚角、航向角、俯仰角速度、横滚角速度、航向角速度不可用）
        // bit3 = 1：x轴加速度计故障
        // bit4 = 1：y轴加速度计故障
        // bit5 = 1：z轴加速度计故障 车速x、车速y、车速z、x向加速度、y向加速度、z向加速度不可用）
        // bit6
        // = 1：卫星辅助对准，未收到卫星定位定向数据（无卫星数据，无法进行卫星辅助对准，无导航信息，角速度、加速度可用）
        // bit7 = 1：装订模式对准，装订数据超差(装订数据错误，无法进行对准无导航信息，角速度、加速度可用）
        // bit8 = 1：轮速计故障(如果卫星有效，数据均可保证精度，如果卫星无效，精度会下降）
        uint16_t ins_error_report = 0;
#pragma endregion
    };

    // 融合定位软件状态接口
    class loc2manager_status {
#pragma region 融合定位软件状态
    public:
        // 帧头
        InternalFrameHeader header;

        // 软件版本
        /*
        软件版本：Vx.yy.zz/aaaabbcc
        Byte0：x（研制阶段）；Byte1：yy（外部评审）；
        Byte2：zz（内部管理）；Byte3：aaaa（年-1970）；Byte4：bb（月）；Byte5：cc（日）；Byte6～7：预留
        */
        uint64_t loc_software_version = 0;

        // 故障数量
        uint32_t error_num = 0;

        // 故障码列表
        std::vector<loc2manager_fault_data> fault_list;
#pragma endregion
    };

    // loc2manager_geo_point
    template <>
    inline loc2manager_geo_point msg_parse<loc2manager_geo_point>(const uint8_t *buffer, size_t size, size_t &offset) {
        loc2manager_geo_point obj;
        read_le(obj.longitude, buffer, size, offset);
        read_le(obj.latitude, buffer, size, offset);
        read_le(obj.altitude, buffer, size, offset);
        return obj;
    }

    // loc2manager_pose
    template <>
    inline loc2manager_pose msg_parse<loc2manager_pose>(const uint8_t *buffer, size_t size, size_t &offset) {
        loc2manager_pose obj;
        read_le(obj.heading, buffer, size, offset);
        read_le(obj.pitch, buffer, size, offset);
        read_le(obj.roll, buffer, size, offset);
        return obj;
    }

    // loc2manager_basic_data
    template <>
    inline loc2manager_basic_data msg_parse<loc2manager_basic_data>(const uint8_t *buffer, size_t size,
                                                                    size_t &offset) {
        loc2manager_basic_data obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        obj.fuse_geo = msg_parse<loc2manager_geo_point>(buffer, size, offset);
        obj.nav_geo = msg_parse<loc2manager_geo_point>(buffer, size, offset);
        obj.sate_geo = msg_parse<loc2manager_geo_point>(buffer, size, offset);
        obj.fuse_pose = msg_parse<loc2manager_pose>(buffer, size, offset);

        read_le(obj.fuse_vx, buffer, size, offset);
        read_le(obj.fuse_vy, buffer, size, offset);
        read_le(obj.fuse_vz, buffer, size, offset);
        read_le(obj.fuse_wx, buffer, size, offset);
        read_le(obj.fuse_wy, buffer, size, offset);
        read_le(obj.fuse_wz, buffer, size, offset);
        read_le(obj.dr_x, buffer, size, offset);
        read_le(obj.dr_y, buffer, size, offset);
        read_le(obj.dr_z, buffer, size, offset);
        read_le(obj.dr_vx, buffer, size, offset);
        read_le(obj.dr_vy, buffer, size, offset);
        read_le(obj.dr_vz, buffer, size, offset);
        obj.dr_pose = msg_parse<loc2manager_pose>(buffer, size, offset);
        read_le(obj.dr_rollrate, buffer, size, offset);
        read_le(obj.dr_pitchrate, buffer, size, offset);
        read_le(obj.dr_yawrate, buffer, size, offset);
        read_le(obj.navstate, buffer, size, offset);
        read_le(obj.gnssstate, buffer, size, offset);
        read_le(obj.insstate, buffer, size, offset);
        read_le(obj.ins_error_report, buffer, size, offset);

        return obj;
    }

    // loc2manager_fault_data
    template <>
    inline void msg_pack<loc2manager_fault_data>(const loc2manager_fault_data &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.fault_code, buffer);
        msg_pack(obj.fault_level, buffer);
    }

    // loc2manager_geo_point
    template <>
    inline void msg_pack<loc2manager_geo_point>(const loc2manager_geo_point &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.longitude, buffer);
        msg_pack(obj.latitude, buffer);
        msg_pack(obj.altitude, buffer);
    }

    // loc2manager_pose
    template <> inline void msg_pack<loc2manager_pose>(const loc2manager_pose &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.heading, buffer);
        msg_pack(obj.pitch, buffer);
        msg_pack(obj.roll, buffer);
    }

    // loc2manager_basic_data
    template <>
    inline void msg_pack<loc2manager_basic_data>(const loc2manager_basic_data &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.fuse_geo, buffer);
        msg_pack(obj.nav_geo, buffer);
        msg_pack(obj.sate_geo, buffer);
        msg_pack(obj.fuse_pose, buffer);

        msg_pack(obj.fuse_vx, buffer);
        msg_pack(obj.fuse_vy, buffer);
        msg_pack(obj.fuse_vz, buffer);
        msg_pack(obj.fuse_wx, buffer);
        msg_pack(obj.fuse_wy, buffer);
        msg_pack(obj.fuse_wz, buffer);
        msg_pack(obj.dr_x, buffer);
        msg_pack(obj.dr_y, buffer);
        msg_pack(obj.dr_z, buffer);
        msg_pack(obj.dr_vx, buffer);
        msg_pack(obj.dr_vy, buffer);
        msg_pack(obj.dr_vz, buffer);
        msg_pack(obj.dr_pose, buffer);
        msg_pack(obj.dr_rollrate, buffer);
        msg_pack(obj.dr_pitchrate, buffer);
        msg_pack(obj.dr_yawrate, buffer);
        msg_pack(obj.navstate, buffer);
        msg_pack(obj.gnssstate, buffer);
        msg_pack(obj.insstate, buffer);
        msg_pack(obj.ins_error_report, buffer);
    }

    // loc2manager_status
    template <> inline void msg_pack<loc2manager_status>(const loc2manager_status &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.loc_software_version, buffer);
        msg_pack(obj.error_num, buffer);
        for (const auto &f : obj.fault_list) {
            msg_pack(f, buffer);
        }
    }

} // namespace protocol_common
