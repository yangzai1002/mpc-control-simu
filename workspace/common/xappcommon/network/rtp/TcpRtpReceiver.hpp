#ifndef NET_TCP_RTP_RECEIVER_HPP
#define NET_TCP_RTP_RECEIVER_HPP

#include <iostream>
#include <memory>
#include <thread>
#include <utility>

#include "../net/InetAddress.hpp"
#include "RtpReceiver.hpp"
#include "StringUtils.hpp"
#include "fmt/format.h"
#include "h264tortp.hpp"

class TcpRtpReceiver : public RtpReceiver {
public:
    TcpRtpReceiver(std::uint32_t ssrc, std::int32_t fd, std::uint8_t rtp_channel, std::uint8_t rtcp_channel)
        : RtpReceiver(ssrc), _fd(fd), _rtp_ch(rtp_channel), _rtcp_ch(rtcp_channel) {
        sockaddr_in c{};
        socklen_t   clen = sizeof(c);
        if (getpeername(_fd, (sockaddr *)&c, &clen) < 0) {
            throw std::runtime_error("getpeername error-" + app_common::StringUtils::str_error(errno));
        }
        _addr = Ipv4Address(c);
    }

    ~TcpRtpReceiver() override {
        std::cout << fmt::format("############# destroy tcp rtp receiver {}:{}, channel {}-{}", _addr.ip(),
                                 _addr.port(), _rtp_ch, _rtcp_ch)
                  << std::endl;
    }

protected:
    TcpRtpReceiver() : RtpReceiver(0), _fd(-1), _rtp_ch(0), _rtcp_ch(0) {
    }

public:
    [[nodiscard]] std::string string() const override {
        return fmt::format("tcp {}, rtp_channel {}, rtcp_channel {}", _addr.string(), _rtp_ch, _rtcp_ch);
    }

    std::string ip() override {
        return _addr.ip();
    }

    uint16_t rtp_port() override {
        return _addr.port();
    }

    uint16_t rtcp_port() override {
        return _addr.port();
    }

    void push(const std::uint8_t *p, std::size_t s, std::int32_t fd) noexcept(false) override {
        std::uint8_t buff[4];

        std::uint16_t total_len = s;

        buff[0] = '$';
        buff[1] = _rtp_ch;
        buff[2] = (uint8_t)((total_len & 0xFF00) >> 8);
        buff[3] = (uint8_t)(total_len & 0xFF);

        auto x = htonl(_ssrc);
        memcpy((void *)(p + RTP_OFFSET_SSRC), &x, sizeof(x));

        sendmsg_sync(buff, sizeof(buff), p, s);
    }

    void push(const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2, std::size_t s2,
              std::int32_t fd) noexcept(false) override {
        std::uint8_t buff[4];

        std::uint16_t total_len = s1 + s2;

        buff[0] = '$';
        buff[1] = _rtp_ch;
        buff[2] = (uint8_t)((total_len & 0xFF00) >> 8);
        buff[3] = (uint8_t)(total_len & 0xFF);

        auto x = htonl(_ssrc);
        memcpy((void *)(p1 + RTP_OFFSET_SSRC), &x, sizeof(x));

        sendmsg_sync(buff, sizeof(buff), p1, s1, p2, s2);
    }

    void push_rtcp(std::int32_t fd, const std::uint8_t *p, std::size_t s) noexcept(false) override {
        std::uint8_t buff[4];

        std::uint16_t total_len = s;

        buff[0] = '$';
        buff[1] = _rtcp_ch;
        buff[2] = (uint8_t)((total_len & 0xFF00) >> 8);
        buff[3] = (uint8_t)(total_len & 0xFF);

        sendmsg_sync(buff, sizeof(buff), p, s);
    }

    void push_rtcp(std::int32_t fd, const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2,
                   std::size_t s2) noexcept(false) override {
        std::uint8_t buff[4];

        std::uint16_t total_len = s1 + s2;

        buff[0] = '$';
        buff[1] = _rtcp_ch;
        buff[2] = (uint8_t)((total_len & 0xFF00) >> 8);
        buff[3] = (uint8_t)(total_len & 0xFF);

        sendmsg_sync(buff, sizeof(buff), p1, s1, p2, s2);
    }

protected:
    void sendmsg_sync(const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2, std::size_t s2) {
        iovec iov[2];
        iov[0].iov_base = (void *)p1;
        iov[0].iov_len = s1;
        iov[1].iov_base = (void *)p2;
        iov[1].iov_len = s2;

        msghdr msg{};
        msg.msg_iov = iov;
        msg.msg_iovlen = 2;

    retry:
        if (sendmsg(_fd, &msg, MSG_NOSIGNAL) < 0) {
            error(errno);
            if (EAGAIN == _error || EWOULDBLOCK == _error) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                goto retry;
            }

            throw std::runtime_error("sendmsg error-" + app_common::StringUtils::str_error(errno));
        }
    }

    void sendmsg_sync(const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2, std::size_t s2,
                      const std::uint8_t *p3, std::size_t s3) {
        iovec iov[3];
        iov[0].iov_base = (void *)p1;
        iov[0].iov_len = s1;
        iov[1].iov_base = (void *)p2;
        iov[1].iov_len = s2;
        iov[2].iov_base = (void *)p3;
        iov[2].iov_len = s3;

        msghdr msg{};
        msg.msg_iov = iov;
        msg.msg_iovlen = 3;

        if (sendmsg(_fd, &msg, MSG_NOSIGNAL) < 0) {
            error(errno);
            throw std::runtime_error("sendmsg error-" + app_common::StringUtils::str_error(_error));
        }
    }

protected:
    std::int32_t _fd;
    std::uint8_t _rtp_ch;
    std::uint8_t _rtcp_ch;
    Ipv4Address  _addr;
};

#endif
