#ifndef RTP_H264_NALU_RTPS_ENDER_HPP
#define RTP_H264_NALU_RTPS_ENDER_HPP

#include <arpa/inet.h>
#include <netinet/in.h>

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "Error.hpp"
#include "H264Nalu.hpp"
#include "RtpSender.hpp"
#include "StringUtils.hpp"
#include "h264tortp.hpp"

class H264NaluRtpSender : public RtpSender {
public:
    H264NaluRtpSender(std::uint32_t fps, const std::string &local_ip) : RtpSender(fps, local_ip) {
        if (fps <= 0) {
            throw std::runtime_error("fps error");
        }
    }

    ~H264NaluRtpSender() = default;

    void send(const std::uint8_t *p, std::size_t s) override {
        auto nalus = H264Nalu::nalus(0, p, s);
        if (nalus.empty()) {
            throw ExceptionParseNalus("cannot find any nalu");
        }
        for (const auto &nalu : nalus) {
            send_nalu(nalu.is_sps() || nalu.is_pps(), nalu.data() + nalu.start_code_len(),
                      nalu.size() - nalu.start_code_len());
        }
    }

    void send_nalu(bool is_sps_pps, const std::uint8_t *p, std::size_t s) {
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
        rtp_hdr->payload_type = H264;
        rtp_hdr->timestamp = htonl(_timestamp);
        std::cout << "timestamp: " << _timestamp << std::endl;
        //    rtp_hdr->ssrc = htonl(ssrc); // 在后面设置 SSRC
        rtp_hdr->marker = 0;

        auto nalu_hdr = (nalu_header_t *)&_rtp_header_buff[12];
        auto fu_ind = (fu_indicator_t *)&_rtp_header_buff[12];
        auto fu_hdr = (fu_header_t *)&_rtp_header_buff[13];

        if (_s <= RTP_PAYLOAD_MAX_SIZE) { // single nal unit
            // 1. 设置 rtp 头
            rtp_hdr->seq_no = htons(++_seqnum % (UINT16_MAX + 1));
            // 2. 设置rtp荷载 single nal unit 头
            nalu_hdr->f = (_p[0] & 0x80U) >> 7;   /* bit0 */
            nalu_hdr->nri = (_p[0] & 0x60U) >> 5; /* bit1~2 */
            nalu_hdr->type = (_p[0] & 0x1fU);
            // 3. 发送 nalu 内容
            RtpSender::send(13, 1, _s - 1); // 不拷贝nalu头
        } else {
            // FU-A分割：计算分割的个数。除最后一个分片外，每一个分片消耗 RTP_PAYLOAD_MAX_SIZE BYLE
            auto fu_pack_num = (_s + RTP_PAYLOAD_MAX_SIZE - 1) / RTP_PAYLOAD_MAX_SIZE;
            auto last_fu_pack_size = _s % RTP_PAYLOAD_MAX_SIZE;
            last_fu_pack_size = (0 == last_fu_pack_size) ? RTP_PAYLOAD_MAX_SIZE : last_fu_pack_size;
            for (auto fu_seq = 0; fu_seq < fu_pack_num; fu_seq++) {
                rtp_hdr->seq_no = htons(++_seqnum % (UINT16_MAX + 1));
                fu_ind->f = (_p[0] & 0x80U) >> 7;
                fu_ind->nri = (_p[0] & 0x60U) >> 5;
                fu_ind->type = 28;
                fu_hdr->type = _p[0] & 0x1FU;
                fu_hdr->r = 0;
                // 根据FU-A的类型设置不同的rtp头和rtp荷载头
                if (fu_seq == 0) { /* 第一个FU-A */
                    fu_hdr->s = 1;
                    fu_hdr->e = 0;
                    RtpSender::send(14, 1, RTP_PAYLOAD_MAX_SIZE - 1);
                } else if (fu_seq < fu_pack_num - 1) { // 中间的FU-A
                    fu_hdr->s = 0;
                    fu_hdr->e = 0;
                    RtpSender::send(14, RTP_PAYLOAD_MAX_SIZE * fu_seq, RTP_PAYLOAD_MAX_SIZE);
                } else {                 // 最后一个FU-A
                    rtp_hdr->marker = 1; // 该包为一帧的结尾则置为1, 否则为0. rfc 1889 没有规定该位的用途
                    fu_hdr->s = 0;
                    fu_hdr->e = 1;
                    RtpSender::send(14, RTP_PAYLOAD_MAX_SIZE * fu_seq, last_fu_pack_size);
                }

                // 防止发送太快，导致丢包
                std::this_thread::sleep_for(std::chrono::microseconds(3));
            }
        }
    }
};

#endif
