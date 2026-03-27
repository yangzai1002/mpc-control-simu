#ifndef NET_PS_CODED_H264_NALU_RTP_SENDER_HPP
#define NET_PS_CODED_H264_NALU_RTP_SENDER_HPP

#include <arpa/inet.h>
#include <netinet/in.h>

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "RtpSender.hpp"
#include "StringUtils.hpp"
#include "h264tortp.hpp"

class PSCodedH264NaluRtpSender : public RtpSender {
public:
    PSCodedH264NaluRtpSender(std::uint32_t fps, const std::string &local_ip) : RtpSender(fps, local_ip) {
    }

    ~PSCodedH264NaluRtpSender() = default;

    void content(const std::uint8_t *p, std::size_t s) override {
        if (s < 1) {
            throw std::runtime_error("length error");
        }
        _p = p;
        _s = s;

        if (_p[4] != 0xc0) {
            _timestamp += (90000 / _fps); /* 90000 / 25 = 3600 */
        }
    }

    void send(const std::uint8_t *p, std::size_t s) override {
        content(p, s);

        auto rtp_hdr = (rtp_header_t *)_rtp_header_buff;
        rtp_hdr->csrc_len = 0;
        rtp_hdr->extension = 0;
        rtp_hdr->padding = 0;
        rtp_hdr->version = 2;
        rtp_hdr->payload_type = H264;
        rtp_hdr->timestamp = htonl(_timestamp);
        rtp_hdr->marker = 0;

        // FU-A分割：计算分割的个数。除最后一个分片外，每一个分片消耗 RTP_PAYLOAD_MAX_SIZE BYLE
        auto num_packet = (_s + RTP_PAYLOAD_MAX_SIZE - 1) / RTP_PAYLOAD_MAX_SIZE;
        auto last_packet_size = _s % RTP_PAYLOAD_MAX_SIZE;
        last_packet_size = (0 == last_packet_size) ? RTP_PAYLOAD_MAX_SIZE : last_packet_size;
        for (auto i = 0; i < num_packet; i++) {
            rtp_hdr->seq_no = htons(++_seqnum % (UINT16_MAX + 1));
            if (i == 0) { /* 第一个FU-A */
                RtpSender::send(12, 0, RTP_PAYLOAD_MAX_SIZE);
            } else if (i < num_packet - 1) { // 中间的FU-A
                RtpSender::send(12, RTP_PAYLOAD_MAX_SIZE * i, RTP_PAYLOAD_MAX_SIZE);
            } else {
                RtpSender::send(12, RTP_PAYLOAD_MAX_SIZE * i, last_packet_size);
            }
        }
    }
};

#endif
