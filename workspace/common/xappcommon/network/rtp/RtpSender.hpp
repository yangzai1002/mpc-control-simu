#ifndef RTP_NALU_RTP_SENDER_HPP
#define RTP_NALU_RTP_SENDER_HPP

#include <netinet/in.h>

#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "../media/Sender.hpp"
#include "../net/InetAddress.hpp"
#include "McastRtpReceiver.hpp"
#include "RtpReceiver.hpp"
#include "RtpUDPEndpoint.hpp"
#include "fmt/format.h"
#include "h264tortp.hpp"

class RtpSender : public Sender {
public:
    // clang-format off
    RtpSender(std::uint32_t fps
        , const std::string &local_ip)
        : Sender()
        , _multicast(false)
        , _rtp_local_endpoint(local_ip)
        , _rtp_header_buff{0}
        , _fps(fps)
        , _seqnum(0)
        , _timestamp(0) {
        // clang-format on

        if (_rtp_local_endpoint.set_dscp(0x2E) < 0) { // 设置 DSCP 为 AF41 (010111)
            std::cout << "set dscp error" << std::endl;
        }
    };

    // clang-format off
    RtpSender(std::uint32_t fps
        , const std::string &local_ip
        , const std::string &mcast_ip
        , std::uint16_t mcast_rtp_port
        , std::uint16_t mcast_rtcp_port)
        : RtpSender(fps, local_ip) {
        // clang-format on
        _multicast = true;
        _rtp_mcast_receiver =
            std::make_shared<McastRtpReceiver>(0xFFFFFFFFU, mcast_ip, mcast_rtp_port, mcast_rtcp_port);
    }
    // clang-format on

    ~RtpSender() = default;

    void content(const std::uint8_t *p, std::size_t s) override {
        if (s < 1) {
            throw std::runtime_error("length error");
        }
        if (s > 4 && 0 == memcmp(p, "\x00\x00\x00\x01", 4)) {
            _p = p + 4;
            _s = s - 4;
        } else {
            _p = p + 4;
            _s = s - 4;
        }
        _timestamp += (90000 / _fps); /* 90000 / 25 = 3600 */
    }

    std::uint16_t receiver_count() override {
        return _multicast ? 1 : Sender::receiver_count();
    }

    void dump_receivers() override {
        if (_multicast) {
            std::cout << fmt::format(_rtp_mcast_receiver->string()) << std::endl;
            return;
        }

        Sender::dump_receivers();
    }

    void add_receiver(const std::string &receiver_name, const std::shared_ptr<Receiver> &s) override {
        if (_multicast) {
            throw std::runtime_error("multicast enabled");
        }

        Sender::add_receiver(receiver_name, s);
    }

    void del_receiver(const std::string &receiver_name) override {
        if (_multicast) {
            throw std::runtime_error("multicast enabled");
        }

        Sender::del_receiver(receiver_name);
    }

    virtual void content(const std::uint8_t *p, std::size_t s, std::uint32_t timestamp) {
        if (s < 1) {
            throw std::runtime_error("length error");
        }

        //    printf("##NALU: ");
        //    for (std::size_t i = 0; i < 16; ++i) {
        //        printf("%02X ", p[i]);
        //    }
        //    printf("\n");

        _p = p;
        _s = s;

        _timestamp = timestamp;
    }

    virtual void send(std::size_t header_len, std::size_t payload_offset, std::size_t payload_len) noexcept(true) {
        std::lock_guard<std::mutex> g(_receivers_mutex);

        for (auto &r : _receivers) {
            if (0 == r.second->error()) { //
                try {
                    r.second->push(_rtp_header_buff, header_len, _p + payload_offset, payload_len,
                                   _rtp_local_endpoint.rtp_fd());
                } catch (const std::runtime_error &e) {
                    std::cout << fmt::format("push to {} error: {}", r.first, e.what()) << std::endl;
                }
            }
        }
    }

    virtual void send(const std::uint8_t *p, std::size_t s, std::uint32_t timestamp) noexcept(true) {
        std::cout << fmt::format("un-implemented method") << std::endl;
    }

    [[maybe_unused]] [[nodiscard]] bool is_multicast() const {
        return _multicast;
    }

    [[maybe_unused]] std::string local_ip() {
        return _rtp_local_endpoint.rtp_ip();
    }

    [[maybe_unused]] std::uint16_t local_rtp_port() {
        return _rtp_local_endpoint.rtp_port();
    }

    [[maybe_unused]] std::uint16_t local_rtcp_port() {
        return _rtp_local_endpoint.rtcp_port();
    }

    [[maybe_unused]] std::string mcast_ip() {
        if (!_multicast) {
            throw std::runtime_error("multicast disabled");
        }

        return _rtp_mcast_receiver->ip();
    }

    [[maybe_unused]] std::uint16_t mcast_rtp_port() {
        if (!_multicast) {
            throw std::runtime_error("multicast disabled");
        }

        return _rtp_mcast_receiver->rtp_port();
    }

    [[maybe_unused]] std::uint16_t mcast_rtcp_port() {
        if (!_multicast) {
            throw std::runtime_error("multicast disabled");
        }

        return _rtp_mcast_receiver->rtcp_port();
    }

protected:
#ifndef EASY_NANO
    static const std::size_t RTP_PAYLOAD_MAX_SIZE = 1400;
#else
    static const std::size_t RTP_PAYLOAD_MAX_SIZE = 32768;
#endif
    bool                         _multicast;
    RtpUDPEndpoint               _rtp_local_endpoint;
    std::shared_ptr<RtpReceiver> _rtp_mcast_receiver;

    std::uint8_t _rtp_header_buff[sizeof(rtp_header_t) + sizeof(raw_header_t)]; // 多余的两个字节是用于 FU/或其它扩展的

    std::uint32_t _fps;
    std::uint16_t _seqnum;
    std::uint32_t _timestamp;
};

#endif
