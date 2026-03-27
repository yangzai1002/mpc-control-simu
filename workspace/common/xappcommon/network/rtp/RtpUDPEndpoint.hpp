#ifndef ANCILLARY_DEV_BINOCULAR_RTPUDPENDPOINT_HPP
#define ANCILLARY_DEV_BINOCULAR_RTPUDPENDPOINT_HPP

#include "../net/InetAddress.hpp"
#include "../net/poller/PollerDesc.hpp"
#include "NetUtils.hpp"

class RtpUDPEndpoint {
public:
    RtpUDPEndpoint(const std::string& local_ip) : _rtp_udp_fd(-1), _rtcp_udp_fd(-1) {
        try {
            std::uint16_t port1 = 0;
            auto          fd1 = app_common::NetUtils::udp_listen(local_ip, port1);

            std::uint16_t port2 = port1 + 1;
            auto          fd2 = app_common::NetUtils::udp_listen(local_ip, port2);
            _rtp_udp_fd = fd1;
            _rtcp_udp_fd = fd2;
            _rtp_udp_addr = {local_ip, port1};
            _rtcp_udp_addr = {local_ip, port2};
        } catch (const std::exception& e) {
            if (_rtp_udp_fd >= 0) {
                close(_rtp_udp_fd);
            }
            if (_rtcp_udp_fd >= 0) {
                close(_rtcp_udp_fd);
            }
            std::rethrow_exception(std::current_exception());
        }
    }

    ~RtpUDPEndpoint() {
        close(_rtp_udp_fd);
        close(_rtcp_udp_fd);
    }

    [[nodiscard]] std::int32_t rtp_fd() const {
        return _rtp_udp_fd;
    }

    [[nodiscard]] std::int32_t rtcp_fd() const {
        return _rtcp_udp_fd;
    }

    [[nodiscard]] std::uint16_t rtp_port() const {
        return _rtp_udp_addr.port();
    };

    [[nodiscard]] std::uint16_t rtcp_port() const {
        return _rtcp_udp_addr.port();
    }

    [[nodiscard]] std::string rtp_ip() const {
        return _rtp_udp_addr.ip();
    }

    [[nodiscard]] std::string rtcp_ip() const {
        return _rtcp_udp_addr.ip();
    }

    int set_dscp(int dscp) {
        int tos = dscp << 2; // DSCP 值左移 2 位填充 TOS 字段
        return setsockopt(_rtp_udp_fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
    }

private:
    std::int32_t _rtp_udp_fd;
    std::int32_t _rtcp_udp_fd;
    Ipv4Address  _rtp_udp_addr;
    Ipv4Address  _rtcp_udp_addr;
};

#endif // ANCILLARY_DEV_BINOCULAR_RTPUDPENDPOINT_HPP
