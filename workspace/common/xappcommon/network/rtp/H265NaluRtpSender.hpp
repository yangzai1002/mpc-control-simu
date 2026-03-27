#ifndef RTP_H265_NALU_RTPS_ENDER_HPP
#define RTP_H265_NALU_RTPS_ENDER_HPP

#include <arpa/inet.h>
#include <netinet/in.h>

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "Error.hpp"
#include "HexUtils.hpp"
#include "RtpSender.hpp"
#include "h264tortp.hpp"
#include "h265_bitstream_parser.h"
#include "h265_bitstream_parser_state.h"
#include "log/log.h"

class H265NaluRtpSender : public RtpSender {
private:
    struct h265_nalu_header {
        uint8_t nuh_layer_id_h : 1;
        uint8_t nal_unit_type : 6;         // NALU 类型
        uint8_t forbidden_zero_bit : 1;    // 必须为0
        uint8_t nuh_temporal_id_plus1 : 3; // 时间ID
        uint8_t nuh_layer_id_l : 5;        // 层ID（用于可伸缩扩展）
    };

    struct h265_fu_header {
        uint8_t fu_type : 6;
        uint8_t e : 1;
        uint8_t s : 1;
    };

public:
    H265NaluRtpSender(std::uint32_t fps, const std::string &local_ip) : RtpSender(fps, local_ip) {
        if (fps <= 0) {
            throw std::runtime_error("fps error");
        }
    }

    ~H265NaluRtpSender() = default;

    void send(const std::uint8_t *p, std::size_t s) override {
        h265nal::ParsingOptions parsing_options;
        parsing_options.add_checksum = false;
        auto nalus = h265nal::H265BitstreamParser::ParseBitstream(p, s, parsing_options);
        if (nullptr == nalus) {
            throw ExceptionParseNalus("cannot find any nalu");
        }

        for (auto &nalu : nalus->nal_units) {
            auto type = nalu->nal_unit_header->nal_unit_type;
            auto is_sps_pps = (h265nal::NalUnitType::SPS_NUT == type || h265nal::NalUnitType::PPS_NUT == type ||
                               h265nal::NalUnitType::VPS_NUT == type);
            AINFO << "nalu, type: " << type << ", offset " << nalu->offset << ", payload_length: " << nalu->length
                  << ", payload: "
                  << app_common::HexUtils::to_hex_string(p + nalu->offset, std::min(200, (std::int32_t)nalu->length));
            send_nalu(is_sps_pps, type, p + nalu->offset, nalu->length);
        }
    }

    void send_nalu(bool is_sps_pps, std::uint8_t nalu_type, const std::uint8_t *p, std::size_t s) {
        if (s < 2) {
            AERROR << "nalu less than 2";
            return;
        }
        _p = p;
        _s = s;
        if (!is_sps_pps) {
            _timestamp += 90000 / _fps;
        }

        // int nalu_len;   /* 不包括0x00000001起始码, 但包括nalu头部的长度 */
        auto rtp_hdr = (rtp_header_t *)_rtp_header_buff;
        rtp_hdr->csrc_len = 0;
        rtp_hdr->extension = 0;
        rtp_hdr->padding = 0;
        rtp_hdr->version = 2;
        rtp_hdr->payload_type = 98;
        rtp_hdr->timestamp = htonl(_timestamp);
        std::cout << "timestamp: " << _timestamp << std::endl;
        //    rtp_hdr->ssrc = htonl(ssrc); // 在后面设置 SSRC
        rtp_hdr->marker = 0;

        if (_s <= RTP_PAYLOAD_MAX_SIZE) {
            rtp_hdr->seq_no = htons(++_seqnum % (UINT16_MAX + 1));
            RtpSender::send(sizeof(rtp_header_t), 0, _s);
        } else {
            _s -= sizeof(h265_nalu_header);
            _p += sizeof(h265_nalu_header);

            auto fu_pack_num = (_s + RTP_PAYLOAD_MAX_SIZE - 1) / RTP_PAYLOAD_MAX_SIZE;
            auto last_fu_pack_size = _s % RTP_PAYLOAD_MAX_SIZE;
            last_fu_pack_size = (0 == last_fu_pack_size) ? RTP_PAYLOAD_MAX_SIZE : last_fu_pack_size;

            for (auto fu_seq = 0; fu_seq < fu_pack_num; fu_seq++) {
                auto offset = sizeof(rtp_header_t);
                _rtp_header_buff[offset++] = (49U << 1);
                _rtp_header_buff[offset++] = 1;

                rtp_hdr->seq_no = htons(++_seqnum % (UINT16_MAX + 1));
                if (fu_seq == 0) {
                    _rtp_header_buff[offset++] = (0x80U | nalu_type);
                    RtpSender::send(offset, 0, RTP_PAYLOAD_MAX_SIZE);
                } else if (fu_seq < fu_pack_num - 1) {
                    _rtp_header_buff[offset++] = (0x00U | nalu_type);
                    RtpSender::send(offset, RTP_PAYLOAD_MAX_SIZE * fu_seq, RTP_PAYLOAD_MAX_SIZE);
                } else {
                    rtp_hdr->marker = 1;
                    _rtp_header_buff[offset++] = (0x40U | nalu_type);
                    RtpSender::send(offset, RTP_PAYLOAD_MAX_SIZE * fu_seq, last_fu_pack_size);
                }

                // 防止发送太快，导致丢包
                std::this_thread::sleep_for(std::chrono::microseconds(3));
            }
        }
    }
};

#endif
