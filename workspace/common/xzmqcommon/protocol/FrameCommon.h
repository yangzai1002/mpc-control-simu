#ifndef _FRAME_COMMON_H_
#define _FRAME_COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <cstdint>

#include "common.h"
#include "fmt/format.h"
#include "nlohmann/json.hpp"

namespace protocol_common {
#pragma pack(1)
    // 跨系统通信帧头
    struct CommunicationHead {
        // 消息ID
        // bit32->bit24：信息流向标志位
        // 	0x01：操控车->通信系统；0x02：操控车->自主系统；0x03：操控车->通用底盘；0x04：操控车->任务载荷；0x05：操控车->挂载件；
        // 	0x21：通信系统->操控车；0x22：自主系统->操控车；0x23：通用底盘->操控车；0x24：任务载荷->操控车；0x25：挂载件->操控车；
        // 	0x41：其他的跨车通信
        // 	0x51：各车内部通信
        // bit23->bit16：发送端设备
        // 	无人车端：
        // 	0x01：XL载荷；0x02：ZD载荷；0x03：HL载荷；0x04：TX载荷；0x05：DC载荷；
        // 	0x21：中型底盘；0x22：轻型底盘；0x23：自主系统；
        // 	0x41：侦察飞行器；0x42：系留飞行器；0x43：清剿机器人；
        // 	0b011xxxxx：其他
        // 	操控车端：
        // 	0xA1：服务器；0xA2：操控席；0xA3：协同席；0xA4：车长终端；0xA5：驾驶员终端；
        // 	0b101xxxxx：其他
        // 	上级指挥单元：
        // 	0xC1：营指；0xC2：连指；
        // bit15->bit0：Method ID/Event ID
        // 	Method
        // ID：首位为0。0x7FFF：应急指令下发；0x7FFE：应急指令取消；0x7FFD：权限连接建立；0x7FFC：接触权限链接；其余：自定义。
        // 	Event
        // ID：首位为1。0xFFFF：权限状态发布；0xFFFE：网控IP发布；0xFFFD：控制源IP发布；0xFFFC：整车模式发布；0xFFFB：故障信息发布；0xFFFA：通信状态发布；其余：自定义。
        uint32_t message_id = 0;

        // 消息帧长度
        // 消息帧长度（不含）之后的全部字节长数
        uint32_t message_length = 0;

        // 客户端ID
        // 由通信系统确定
        uint16_t client_id = 0;

        // 会话ID
        // 区分同一客户端请求同一服务Service的次数，默认为0
        uint16_t session_id = 0;

        // 协议版本
        // 默认为0x01
        uint8_t protocol_version = 0;

        // 接口版本
        // 默认为0x01
        uint8_t interface_version = 0;

        // 消息类型
        // 0x00：REQUEST，需要响应的请求
        // 0x01：REQUEST_WITHOUT_RESPONSE，不需要响应的请求
        // 0x02：NOTIFICATION，通知
        // 0x80：RESPONSE，响应请求
        // 0x81：ERROR，错误响应请求
        uint8_t message_type = 0;

        // 返回码
        // 0x00：E_OK，成功
        // 0x01：E_NOT_OK，未知错误
        // 0x02：UNKNOWN_SERVICE，未知的Service ID
        // 0x03：UNKNOWN_METHOD，未知的Method ID
        // 0x04：NOT_READY，应用程序未运行
        // 0x05：NOT_REACHABLE，内部错误
        // 0x06：TIMEOUT，超时
        // 0x07：WRONG_PROTOCOL_VERSION，协议版本错误
        // 0x08：WRONG_INTERFACE_VERSION，接口版本错误
        // 0x09：MALFORMED_MESSAGE，反序列化错误
        // 0x0A：WRONG_MESSAGE_TYPE，报文类型错误
        uint8_t return_code = 0;
    };

    // 跨系统通信帧尾
    struct CommunicationTail {
        // 校验和
        uint32_t payload_checksum = 0;
    };
    // 系统内部通信帧头
    struct InternalFrameHeader {
        // 时间戳，精确到毫秒
        // Byte0：年-1970，Byte1：（1-12）月，Byte2：（1-31）日
        // Byte3：（0-23）时，Byte4：（0-59）分，Byte5：（0-59）秒，Byte6：（0-999）毫秒
        uint64_t timestamp = 0;

        // 消息源系统ID
        // 0-默认；1-综合决策；2-自主机动；3-编队机动；4-融合定位；5-大模型
        uint32_t source_systemid = 0;

        // 消息目标系统ID
        // 0-默认；1-综合决策；2-自主机动；3-编队机动；4-融合定位；5-大模型
        uint32_t target_systemid = 0;
    };

    template <typename T> T msg_parse(const uint8_t *buffer, size_t size, size_t &offset);

    template <typename T>
    inline void read_le(T &v, const uint8_t *data, std::size_t size, size_t &offset) noexcept(false) {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                      "Integral or floating point trivial type required");
        if ((offset + sizeof(T)) > size) {
            throw std::runtime_error(
                fmt::format("buffer overflow when reading data: offset {}, need {}", offset, size));
        }
        memcpy(&v, data + offset, sizeof(T));
        offset += sizeof(T);
    }

    template <typename T>
    inline void read_be(T &v, const uint8_t *buf, std::size_t size, size_t &offset) noexcept(false) {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                      "Integral or floating point trivial type required");

        if ((offset + sizeof(T)) > size) {
            throw std::runtime_error(
                fmt::format("buffer overflow when reading data: offset {}, need {}", offset, size));
        }
        for (size_t i = 0; i < sizeof(T); ++i) {
            // 每读一字节，就左移再或进 v，保证第一个字节是最高位
            v = (v << 8) | buf[offset++];
        }
    }

    // 简单累加校验函数（可按需替换成 CRC）
    static uint32_t CalcChecksum(const uint8_t *data, size_t len) {
        uint32_t sum = 0;
        for (size_t i = 0; i < len; ++i) {
            sum += data[i];
        }
        return sum;
    }

    // CommunicationHead
    template <>
    inline CommunicationHead msg_parse<CommunicationHead>(const uint8_t *buffer, size_t size, size_t &offset) {
        CommunicationHead obj;
        read_le(obj.message_id, buffer, size, offset);
        read_le(obj.message_length, buffer, size, offset);
        read_le(obj.client_id, buffer, size, offset);
        read_le(obj.session_id, buffer, size, offset);
        read_le(obj.protocol_version, buffer, size, offset);
        read_le(obj.interface_version, buffer, size, offset);
        read_le(obj.message_type, buffer, size, offset);
        read_le(obj.return_code, buffer, size, offset);
        return obj;
    }

    // CommunicationTail
    template <>
    inline CommunicationTail msg_parse<CommunicationTail>(const uint8_t *buffer, size_t size, size_t &offset) {
        CommunicationTail obj;
        read_le(obj.payload_checksum, buffer, size, offset);
        return obj;
    }

    // InternalFrameHeader
    template <>
    inline InternalFrameHeader msg_parse<InternalFrameHeader>(const uint8_t *buffer, size_t size, size_t &offset) {
        InternalFrameHeader obj;
        read_le(obj.timestamp, buffer, size, offset);
        read_le(obj.source_systemid, buffer, size, offset);
        read_le(obj.target_systemid, buffer, size, offset);
        return obj;
    }

    // 通用msg_pack模板声明
    template <typename T> void msg_pack(const T &obj, uint8_t *buffer, size_t &offset);

    // template <typename T>
    // void msg_pack(const T &obj, std::vector<uint8_t> &buffer) {
    //   const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&obj);
    //   buffer.insert(buffer.end(), ptr, ptr + sizeof(T));
    // }

    // string 类型
    inline void msg_pack(const std::string &str, std::vector<uint8_t> &buffer) {
        buffer.insert(buffer.end(), str.begin(), str.end());
    }

    template <> inline void msg_pack<CommunicationHead>(const CommunicationHead &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.message_id, buffer);
        msg_pack(obj.message_length, buffer);
        msg_pack(obj.client_id, buffer);
        msg_pack(obj.session_id, buffer);
        msg_pack(obj.protocol_version, buffer);
        msg_pack(obj.interface_version, buffer);
        msg_pack(obj.message_type, buffer);
        msg_pack(obj.return_code, buffer);
    }

    // CommunicationTail
    template <> inline void msg_pack<CommunicationTail>(const CommunicationTail &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.payload_checksum, buffer);
    }

    template <>
    inline void msg_pack<InternalFrameHeader>(const InternalFrameHeader &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.timestamp, buffer);
        msg_pack(obj.source_systemid, buffer);
        msg_pack(obj.target_systemid, buffer);
    }

#pragma pack()
} // namespace protocol_common

#endif
