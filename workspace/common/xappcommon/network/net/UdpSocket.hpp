#ifndef ANCILLARY_DEV_BINOCULAR_UDPSOCKET_HPP
#define ANCILLARY_DEV_BINOCULAR_UDPSOCKET_HPP

#include "Socket.hpp"

class UdpSocket : public Socket {
public:
    UdpSocket(const std::string &local_ip, const std::string &peer_ip, std::uint16_t peer_port)
        : UdpSocket(local_ip, 0, peer_ip, peer_port) {
    }

    UdpSocket(const std::string &local_ip, std::uint16_t local_port, const std::string &peer_ip,
              std::uint16_t peer_port)
        : Socket() {
        std::string error_desc;

        _sock_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
        if (_sock_fd < 0) {
            error_desc = "socket error with " + app_common::StringUtils::str_error(errno);
            goto error;
        }

        try {
            opt_reuse_addr(true);
        } catch (const std::runtime_error &e) {
            error_desc = e.what();
            goto error;
        }

        _addr_peer = Ipv4Address(peer_ip, peer_port);

        _addr_local = Ipv4Address(local_ip, 0);
        {
            auto &in_addr = _addr_local.to_sockaddr_in();
            if (bind(_sock_fd, reinterpret_cast<const sockaddr *>(&in_addr), sizeof(in_addr)) < 0) {
                error_desc = "bind error-" + app_common::StringUtils::str_error(errno);
                goto error;
            }

            if (0 == local_port) {
                sockaddr_in c{};
                socklen_t   clen = sizeof(c);
                if (getsockname(_sock_fd, (sockaddr *)&c, &clen) < 0) {
                    error_desc = "getsockname error with " + app_common::StringUtils::str_error(errno);
                    goto error;
                }
                _addr_local.port(ntohs(c.sin_port));
            }
        }
        return;

    error:
        close();
        throw std::runtime_error(error_desc);
    }

    ~UdpSocket() override = default;
};

#endif // ANCILLARY_DEV_BINOCULAR_UDPSOCKET_HPP
