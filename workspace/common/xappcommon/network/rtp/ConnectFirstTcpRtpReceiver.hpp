#ifndef RTP_CONNECT_FIRST_TCP_RTP_RECEIVER_HPP
#define RTP_CONNECT_FIRST_TCP_RTP_RECEIVER_HPP

#include <unistd.h>

#include <iostream>

#include "../net/InetAddress.hpp"
#include "NetUtils.hpp"
#include "TcpRtpReceiver.hpp"
#include "fmt/format.h"

class ConnectFirstTcpRtpReceiver : public TcpRtpReceiver {
public:
    // clang-format off
    ConnectFirstTcpRtpReceiver(
        const std::string &ip
        , std::uint16_t port
        , std::uint32_t ssrc
        , std::uint8_t rtp_channel
        , std::uint8_t rtcp_channel)
        : TcpRtpReceiver() {
        // clang-format on
        _ssrc = ssrc;
        _rtp_ch = rtp_channel;
        _rtcp_ch = rtcp_channel;
        _addr = Ipv4Address(ip, port);
    }

    ~ConnectFirstTcpRtpReceiver() override {
        std::cout << fmt::format("############# destroy connect first tcp rtp receiver {}:{}, channel {}-{}",
                                 _addr.ip(), _addr.port(), _rtp_ch, _rtcp_ch)
                  << std::endl;
        if (_fd >= 0) {
            close(_fd);
            _fd = -1;
        }
    }

    void prepare(std::int32_t timeout_in_ms) {
        try {
            _fd = app_common::NetUtils::tcp_connect(_addr.ip().c_str(), _addr.port(), 0, timeout_in_ms);
        } catch (const std::runtime_error& e) {
            std::cout << fmt::format("tcp connect to {}:{} error: {}", _addr.ip(), _addr.port(), e.what()) << std::endl;
        }
    }

    bool ready() {
        return _fd >= 0;
    }
};

#endif
