#ifndef RTP_MCAST_RTP_RECEIVER_HPP
#define RTP_MCAST_RTP_RECEIVER_HPP

#include <functional>
#include <iostream>

#include "../net/InetAddress.hpp"
#include "../net/poller/PollerDesc.hpp"
#include "RtpReceiver.hpp"
#include "StringUtils.hpp"
#include "fmt/format.h"
#include "h264tortp.hpp"

class McastRtpReceiver : public RtpReceiver {
public:
    // clang-format off
    McastRtpReceiver(
        std::uint32_t ssrc
        , const std::string &peer_ip
        , std::uint16_t peer_rtp_port
        , std::uint16_t peer_rtcp_port)
        : RtpReceiver(ssrc)
    , _rtp_peer(peer_ip, peer_rtp_port)
    , _rtcp_peer(peer_ip, peer_rtcp_port) {
        // clang-format on
    }

    ~McastRtpReceiver() override {
        std::cout << fmt::format("############# destroy udp rtp receiver {}:{}-{}", _rtp_peer.ip(), _rtp_peer.port(),
                                 _rtcp_peer.port())
                  << std::endl;
    }

    [[nodiscard]] std::string string() const override {
        return fmt::format("mcast_rtp {}, mcast_rtcp {}", _rtp_peer.string(), _rtcp_peer.string());
    }

    std::string ip() override {
        return _rtp_peer.ip();
    }

    uint16_t rtp_port() override {
        return _rtp_peer.port();
    }

    uint16_t rtcp_port() override {
        return _rtcp_peer.port();
    }

    void push(const std::uint8_t *p, std::size_t s, std::int32_t fd) noexcept(false) override {
        auto x = htonl(_ssrc);
        memcpy((void *)(p + RTP_OFFSET_SSRC), &x, sizeof(x));

        sendmesg(fd, _rtp_peer.to_sockaddr_in(), p, s);
    }

    void push(const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2, std::size_t s2,
              std::int32_t fd) noexcept(false) override {
        auto x = htonl(_ssrc);
        memcpy((void *)(p1 + RTP_OFFSET_SSRC), &x, sizeof(x));
        sendmesg(fd, _rtp_peer.to_sockaddr_in(), p1, s1, p2, s2);
    }

    void push_rtcp(std::int32_t fd, const std::uint8_t *p, std::size_t s) noexcept(false) override {
        sendmesg(fd, _rtcp_peer.to_sockaddr_in(), p, s);
    }

    void push_rtcp(std::int32_t fd, const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2,
                   std::size_t s2) noexcept(false) override {
        sendmesg(fd, _rtcp_peer.to_sockaddr_in(), p1, s1, p2, s2);
    };

private:
    void sendmesg(std::int32_t fd, const sockaddr_in &peer, const std::uint8_t *p, std::size_t s) noexcept(false) {
        std::size_t off = 0;
        while (off < s) {
            auto n = sendto(fd, p, s, 0, (sockaddr *)&peer, sizeof(peer));
            if (n < 0) {
                error(errno);
                throw std::runtime_error("sendto error-" + app_common::StringUtils::str_error(errno));
            }

            off += n;
        }
    }

    void sendmesg(std::int32_t fd, const sockaddr_in &peer, const std::uint8_t *p1, std::size_t s1,
                  const std::uint8_t *p2, std::size_t s2) noexcept(false) {
        iovec v[2]{};

        v[0].iov_base = (void *)p1;
        v[0].iov_len = s1;
        v[1].iov_base = (void *)(p2);
        v[1].iov_len = s2;

        msghdr msg{};
        msg.msg_name = (void *)&peer;
        msg.msg_namelen = sizeof(peer);
        msg.msg_iov = v;
        msg.msg_iovlen = 2;

        auto r = sendmsg(fd, &msg, 0);
        if (r != s1 + s2) {
            error(errno);
            throw std::runtime_error("sendmsg error with " + app_common::StringUtils::str_error(errno));
        }
    }

private:
    Ipv4Address _rtp_peer;
    Ipv4Address _rtcp_peer;
};

#endif
