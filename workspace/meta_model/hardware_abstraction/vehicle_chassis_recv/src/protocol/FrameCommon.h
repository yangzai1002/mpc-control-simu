#ifndef _FRAME_COMMON_H_
#define _FRAME_COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
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

    template <typename T> T msg_parse(const uint8_t *buffer, size_t &offset);

    // CommunicationHead
    template <> inline CommunicationHead msg_parse<CommunicationHead>(const uint8_t *buffer, size_t &offset) {
        CommunicationHead obj;
        memcpy(&obj.message_id, buffer + offset, sizeof(obj.message_id));
        offset += sizeof(obj.message_id);
        memcpy(&obj.message_length, buffer + offset, sizeof(obj.message_length));
        offset += sizeof(obj.message_length);
        memcpy(&obj.client_id, buffer + offset, sizeof(obj.client_id));
        offset += sizeof(obj.client_id);
        memcpy(&obj.session_id, buffer + offset, sizeof(obj.session_id));
        offset += sizeof(obj.session_id);
        memcpy(&obj.protocol_version, buffer + offset, sizeof(obj.protocol_version));
        offset += sizeof(obj.protocol_version);
        memcpy(&obj.interface_version, buffer + offset, sizeof(obj.interface_version));
        offset += sizeof(obj.interface_version);
        memcpy(&obj.message_type, buffer + offset, sizeof(obj.message_type));
        offset += sizeof(obj.message_type);
        memcpy(&obj.return_code, buffer + offset, sizeof(obj.return_code));
        offset += sizeof(obj.return_code);
        return obj;
    }

    // CommunicationTail
    template <> inline CommunicationTail msg_parse<CommunicationTail>(const uint8_t *buffer, size_t &offset) {
        CommunicationTail obj;
        memcpy(&obj.payload_checksum, buffer + offset, sizeof(obj.payload_checksum));
        offset += sizeof(obj.payload_checksum);
        return obj;
    }

    // InternalFrameHeader
    template <> inline InternalFrameHeader msg_parse<InternalFrameHeader>(const uint8_t *buffer, size_t &offset) {
        InternalFrameHeader obj;
        memcpy(&obj.timestamp, buffer + offset, sizeof(obj.timestamp));
        offset += sizeof(obj.timestamp);
        memcpy(&obj.source_systemid, buffer + offset, sizeof(obj.source_systemid));
        offset += sizeof(obj.source_systemid);
        memcpy(&obj.target_systemid, buffer + offset, sizeof(obj.target_systemid));
        offset += sizeof(obj.target_systemid);
        return obj;
    }

    // 通用msg_pack模板声明
    template <typename T> void msg_pack(const T &obj, uint8_t *buffer, size_t &offset);

    // CommunicationHead
    template <> inline void msg_pack<CommunicationHead>(const CommunicationHead &obj, uint8_t *buffer, size_t &offset) {
        memcpy(buffer + offset, &obj.message_id, sizeof(obj.message_id));
        offset += sizeof(obj.message_id);
        memcpy(buffer + offset, &obj.message_length, sizeof(obj.message_length));
        offset += sizeof(obj.message_length);
        memcpy(buffer + offset, &obj.client_id, sizeof(obj.client_id));
        offset += sizeof(obj.client_id);
        memcpy(buffer + offset, &obj.session_id, sizeof(obj.session_id));
        offset += sizeof(obj.session_id);
        memcpy(buffer + offset, &obj.protocol_version, sizeof(obj.protocol_version));
        offset += sizeof(obj.protocol_version);
        memcpy(buffer + offset, &obj.interface_version, sizeof(obj.interface_version));
        offset += sizeof(obj.interface_version);
        memcpy(buffer + offset, &obj.message_type, sizeof(obj.message_type));
        offset += sizeof(obj.message_type);
        memcpy(buffer + offset, &obj.return_code, sizeof(obj.return_code));
        offset += sizeof(obj.return_code);
    }

    // CommunicationTail
    template <> inline void msg_pack<CommunicationTail>(const CommunicationTail &obj, uint8_t *buffer, size_t &offset) {
        memcpy(buffer + offset, &obj.payload_checksum, sizeof(obj.payload_checksum));
        offset += sizeof(obj.payload_checksum);
    }

    // InternalFrameHeader
    template <>
    inline void msg_pack<InternalFrameHeader>(const InternalFrameHeader &obj, uint8_t *buffer, size_t &offset) {
        memcpy(buffer + offset, &obj.timestamp, sizeof(obj.timestamp));
        offset += sizeof(obj.timestamp);
        memcpy(buffer + offset, &obj.source_systemid, sizeof(obj.source_systemid));
        offset += sizeof(obj.source_systemid);
        memcpy(buffer + offset, &obj.target_systemid, sizeof(obj.target_systemid));
        offset += sizeof(obj.target_systemid);
    }

#pragma pack()
} // namespace protocol_common

#endif