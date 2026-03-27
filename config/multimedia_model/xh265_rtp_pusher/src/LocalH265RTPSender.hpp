#ifndef LOCAL_H265_RTP_SENDER_HPP
#define LOCAL_H265_RTP_SENDER_HPP

#include <arpa/inet.h>
#include <netinet/in.h>

#include <map>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include "HexUtils.hpp"
#include "StringUtils.hpp"
#include "h265_bitstream_parser.h"
#include "h265_bitstream_parser_state.h"
#include "log/log.h"
#include "network/net/InetAddress.hpp"
#include "network/rtp/Error.hpp"
#include "network/rtp/RtpUDPEndpoint.hpp"
#include "network/rtp/h264tortp.hpp"

class LocalH265RTPSender {
private:
    struct h265_nalu_header {
        uint16_t forbidden_zero_bit : 1;    // 必须为0
        uint16_t nal_unit_type : 6;         // NALU 类型
        uint16_t nuh_layer_id : 6;          // 层ID（用于可伸缩扩展）
        uint16_t nuh_temporal_id_plus1 : 3; // 时间ID
    };

    struct h265_fu_header {
        uint8_t fu_type : 6;
        uint8_t e : 1;
        uint8_t s : 1;
    };

public:
    LocalH265RTPSender(std::uint32_t ssrc, std::uint32_t fps, const std::string &local_ip, const std::string &peer_ip,
                       std::int32_t peer_port)
        : _fps(fps)
        , _ssrc(ssrc)
        , _endpoint(local_ip)
        , _peer(peer_ip, peer_port)
        , _peer_addr(_peer.to_sockaddr_in())
        , _timestamp(0)
        , _seqnum(0) {
        if (_fps <= 0) {
            throw std::runtime_error("fps error");
        }
    }

    ~LocalH265RTPSender() {
    }

    void send(const std::uint8_t *p, std::size_t s) {
        h265nal::ParsingOptions parsing_options;
        parsing_options.add_checksum = false;
        auto nalus = h265nal::H265BitstreamParser::ParseBitstream(p, s, parsing_options);
        if (nullptr == nalus) {
            AERROR << "cannot find any nalu";
            throw ExceptionParseNalus();
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
        rtp_hdr->ssrc = htonl(_ssrc);
        rtp_hdr->timestamp = htonl(_timestamp);
        std::cout << "timestamp: " << _timestamp << std::endl;
        //    rtp_hdr->ssrc = htonl(ssrc); // 在后面设置 SSRC
        rtp_hdr->marker = 0;

        if (s <= RTP_PAYLOAD_MAX_SIZE) { // single nal unit
            rtp_hdr->seq_no = htons(++_seqnum % (UINT16_MAX + 1));
            memcpy(_rtp_header_buff + sizeof(rtp_header_t), p, s);
            do_send(_rtp_header_buff, s + sizeof(rtp_header_t));
        } else {
            s -= 2;
            p += 2;
            auto fu_pack_num = (s + RTP_PAYLOAD_MAX_SIZE - 1) / RTP_PAYLOAD_MAX_SIZE;
            auto last_fu_pack_size = s % RTP_PAYLOAD_MAX_SIZE;
            last_fu_pack_size = (0 == last_fu_pack_size) ? RTP_PAYLOAD_MAX_SIZE : last_fu_pack_size;
            for (auto fu_seq = 0; fu_seq < fu_pack_num; fu_seq++) {
                auto offset = sizeof(rtp_header_t);
                _rtp_header_buff[offset++] = (49U << 1);
                _rtp_header_buff[offset++] = 1;

                rtp_hdr->seq_no = htons(++_seqnum % (UINT16_MAX + 1));
                if (fu_seq == 0) {
                    _rtp_header_buff[offset++] = (0x80U | nalu_type);
                    memcpy(_rtp_header_buff + offset, p, RTP_PAYLOAD_MAX_SIZE);
                    do_send(_rtp_header_buff, RTP_PAYLOAD_MAX_SIZE + offset);
                } else if (fu_seq < fu_pack_num - 1) {
                    _rtp_header_buff[offset++] = (0x00U | nalu_type);
                    memcpy(_rtp_header_buff + offset, p + RTP_PAYLOAD_MAX_SIZE * fu_seq, RTP_PAYLOAD_MAX_SIZE);
                    do_send(_rtp_header_buff, RTP_PAYLOAD_MAX_SIZE + offset);
                } else {
                    rtp_hdr->marker = 1;
                    _rtp_header_buff[offset++] = (0x40U | nalu_type);
                    memcpy(_rtp_header_buff + offset, p + RTP_PAYLOAD_MAX_SIZE * fu_seq, last_fu_pack_size);
                    do_send(_rtp_header_buff, last_fu_pack_size + offset);
                }
                // 防止发送太快，导致丢包
                std::this_thread::sleep_for(std::chrono::microseconds(3));
            }
        }
    }

    void do_send(std::uint8_t *p, std::int32_t s) {
        auto off = 0;
        while (off < s) {
        retry:
            auto n = sendto(_endpoint.rtp_fd(), p + off, s - off, 0, (sockaddr *)&_peer_addr, sizeof(_peer_addr));
            if (n < 0) {
                if (EAGAIN == errno || EWOULDBLOCK == errno) {
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    goto retry;
                }
                AERROR << "ssrc " << _ssrc << " sendto error " << app_common::StringUtils::str_error(errno);
                throw std::runtime_error("sendto error: " + app_common::StringUtils::str_error(errno));
            }

            off += n;
            ADEBUG << "ssrc " << _ssrc << " sendto " << n << " bytes"
                   << ", s: " << s << ", off: " << off;
        }
    }

private:
    const std::int32_t RTP_PAYLOAD_MAX_SIZE = 1350;

    const std::int32_t _fps;

    const std::int32_t _ssrc;

    const RtpUDPEndpoint _endpoint;

    const Ipv4Address _peer;

    const sockaddr_in _peer_addr;

    std::uint32_t _timestamp;

    std::uint32_t _seqnum;

    std::uint8_t _rtp_header_buff[12 + 1500];
};

#endif
