#pragma once

#include <cstdio>
#include <cstring>
#include <vector>

#include "Protocol.h"
#include "common.h"

namespace protocol_common {

    // 惯导控制
    class __attribute__((packed)) manager2loc_ins_control {
#pragma region params 惯导控制
    public:
        // 帧头
        CommunicationHead header;

        // 控制码
        // 0-状态自检，1-重新寻北
        // 北斗有效情况下：0-1和1效果相同
        // 北斗无效情况下：0-1采用flash初值对准，1采用上一帧定位对准
        // 建议：只发1
        uint8_t code = 1;
#pragma endregion
    };

    // 定位模式切换
    class __attribute__((packed)) manager2loc_loc_mode {
#pragma region params 定位模式切换
    public:
        // 帧头
        CommunicationHead header;

        // 定位模式设置
        // 0-自适应模式，1-惯导+卫星，2-视觉+惯导+卫星，3-激光+惯导+卫星，4-视觉+激光+惯导+卫星
        uint8_t mode = 1;
#pragma endregion
    };

    // manager2loc_ins_control
    template <>
    inline void msg_pack<manager2loc_ins_control>(const manager2loc_ins_control &info, std::vector<uint8_t> &buffer) {
        msg_pack(info.header, buffer);
        msg_pack(info.code, buffer);
    }

    // manager2loc_loc_mode
    template <>
    inline void msg_pack<manager2loc_loc_mode>(const manager2loc_loc_mode &info, std::vector<uint8_t> &buffer) {
        msg_pack(info.header, buffer);
        msg_pack(info.mode, buffer);
    }

    // manager2loc_ins_control
    template <>
    inline manager2loc_ins_control msg_parse<manager2loc_ins_control>(const uint8_t *buffer, size_t size,
                                                                      size_t &offset) {
        manager2loc_ins_control info;
        info.header = msg_parse<CommunicationHead>(buffer, size, offset);
        read_le(info.code, buffer, size, offset);
        return info;
    }

    // manager2loc_loc_mode
    template <>
    inline manager2loc_loc_mode msg_parse<manager2loc_loc_mode>(const uint8_t *buffer, size_t size, size_t &offset) {
        manager2loc_loc_mode info;
        info.header = msg_parse<CommunicationHead>(buffer, size, offset);
        read_le(info.mode, buffer, size, offset);
        offset += sizeof(info.mode);
        return info;
    }

} // namespace protocol_common
