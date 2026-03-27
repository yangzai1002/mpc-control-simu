#ifndef NET_TCP_SOCKET_OF_CLIENT_SIDE_HPP
#define NET_TCP_SOCKET_OF_CLIENT_SIDE_HPP

#include <cstdint>

#include "InetAddress.hpp"
#include "TcpSocket.hpp"

class TcpSocketOfClientSide : public TcpSocket {
public:
    TcpSocketOfClientSide(const std::string &peer_ip, std::uint16_t peer_port) : TcpSocket() {
        std::string error_desc;

        _sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (_sock_fd < 0) {
            error_desc = "socket error with " + app_common::StringUtils::str_error(errno);
            goto error1;
        }

        try {
            opt_linger(true, 0);
        } catch (const std::runtime_error &e) {
            error_desc = e.what();
            goto error1;
        }

        _addr_peer = Ipv4Address(peer_ip, peer_port);

        return;

    error1:
        close();
        throw std::runtime_error(error_desc);
    }

    TcpSocketOfClientSide(const std::string &local_ip, std::uint16_t local_port, const std::string &peer_ip,
                          std::uint16_t peer_port)
        : TcpSocketOfClientSide(peer_ip, peer_port) {
        _addr_local = Ipv4Address(local_ip, local_port);
        auto &      in_addr = _addr_local.to_sockaddr_in();
        std::string error_desc;

        if (bind(_sock_fd, reinterpret_cast<const sockaddr *>(&in_addr), sizeof(in_addr)) < 0) {
            error_desc = "bind error with " + app_common::StringUtils::str_error(errno);
            goto error1;
        }

        if (0 == _addr_local.port()) {
            sockaddr_in c{};
            socklen_t   clen = sizeof(c);
            if (getsockname(_sock_fd, (sockaddr *)&c, &clen) < 0) {
                error_desc = "getsockname error with " + app_common::StringUtils::str_error(errno);
                goto error1;
            }
            _addr_local.port(ntohs(c.sin_port));
        }

        return;

    error1:
        close();
        throw std::runtime_error(error_desc);
    }

    ~TcpSocketOfClientSide() override = default;

    void connect(std::int32_t timeout_in_ms = 1000) {
        auto &in_addr = _addr_peer.to_sockaddr_in();

        auto result = ::connect(_sock_fd, reinterpret_cast<const sockaddr *>(&in_addr), sizeof(in_addr));
        if (result < 0 && errno != EINPROGRESS) {
            throw std::runtime_error("connect error-" + app_common::StringUtils::str_error(errno));
        }

        if (0 == result) {
            return;
        }

        fd_set rdevents{};
        FD_SET(_sock_fd, &rdevents);
        auto    wrevents = rdevents;
        auto    exevents = rdevents;
        timeval tv{.tv_sec = 0, .tv_usec = timeout_in_ms * 1000};
        result = select(_sock_fd + 1, &rdevents, &wrevents, &exevents, &tv);
        if (result <= 0) {
            throw std::runtime_error("connect(select) error with " +
                                     app_common::StringUtils::str_error(errno));
        } else {
            int       error;
            socklen_t len = sizeof(error);
            if (getsockopt(_sock_fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                throw std::runtime_error("getsockopt(SO_ERROR) error with" +
                                         app_common::StringUtils::str_error(errno));
            }
            if (error) {
                throw std::runtime_error("connect error with" + app_common::StringUtils::str_error(error));
            }
        }
    }

    void connect_async(){};
};

#endif
