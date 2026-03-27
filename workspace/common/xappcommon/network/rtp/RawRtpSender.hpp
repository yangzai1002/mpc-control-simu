#ifndef NET_RAW_RTP_SENDER_HPP
#define NET_RAW_RTP_SENDER_HPP

#include <netinet/in.h>

#include <map>
#include <memory>
#include <string>

#include "RtpSender.hpp"

class RawRtpSender : public RtpSender {
public:
    RawRtpSender(const std::shared_ptr<UsageEnvironment> &env, std::uint32_t fps, const std::string &local_ip)
        : RtpSender(fps, local_ip), _env(env) {
        if (fps <= 0) {
            throw std::runtime_error("fps error");
        }
    }

    ~RawRtpSender() = default;

    void content(const std::uint8_t *p, std::size_t s) override {
        if (s < 1) {
            throw std::runtime_error("length error");
        }
        _p = p;
        _s = s;

        _timestamp = (_env->current_milliseconds() - _env->start_time());
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

        auto num_packet = _s / RTP_PAYLOAD_MAX_SIZE;
        auto last_packet_size = _s % RTP_PAYLOAD_MAX_SIZE;
        if (0 == last_packet_size) {
            last_packet_size = RTP_PAYLOAD_MAX_SIZE;
        } else {
            ++num_packet;
        }

        raw_header_t rh;
        for (auto i = 0; i < num_packet; i++) {
            rtp_hdr->seq_no = htons(++_seqnum % (UINT16_MAX + 1));
            if (i < num_packet - 1) {
                rtp_hdr->marker = 0;
                rh.packet_len = htons(RTP_PAYLOAD_MAX_SIZE);
                rh.ralative_packet_ms = htonl(_env->current_milliseconds() - _env->start_time());
                memcpy(_rtp_header_buff + sizeof(*rtp_hdr), &rh, sizeof(rh));
                RtpSender::send(sizeof(rtp_header_t) + sizeof(raw_header_t), RTP_PAYLOAD_MAX_SIZE * i,
                                RTP_PAYLOAD_MAX_SIZE);
            } else {
                rtp_hdr->marker = 1;
                rh.packet_len = htons(last_packet_size);
                rh.ralative_packet_ms = htonl(_env->current_milliseconds() - _env->start_time());
                memcpy(_rtp_header_buff + sizeof(*rtp_hdr), &rh, sizeof(rh));
                RtpSender::send(sizeof(rtp_header_t) + sizeof(raw_header_t), RTP_PAYLOAD_MAX_SIZE * i,
                                last_packet_size);
            }
            // 防止发送太快，导致丢包
            std::this_thread::sleep_for(std::chrono::microseconds(3));
        }
    }

private:
    std::shared_ptr<UsageEnvironment> _env;
};

#endif
