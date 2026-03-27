#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include <fcntl.h>
#include <fmt/format.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include "InetAddress.hpp"
#include "StringUtils.hpp"

class Socket {
public:
    Socket() : _sock_fd(-1) {
    }

    // clang-format off
    Socket(
        int sockfd
        , const std::string &local_ip
        , std::uint16_t local_port
        , const std::string &peer_ip
        , std::uint16_t peer_port): _sock_fd(sockfd)
    , _addr_local(local_ip, local_port)
    , _addr_peer(peer_ip, peer_port) {
        // clang-format on
        if (_sock_fd < 0) {
            throw std::runtime_error("socket error-" + std::to_string(_sock_fd));
        }
    }

    virtual ~Socket() {
        std::cout << fmt::format("###### destroy socket fd {} local_ip {} local port {} peer ip {} peer port {}",
                                 _sock_fd, _addr_local.ip(), _addr_local.port(), _addr_peer.ip(), _addr_peer.port())
                  << std::endl;
        close();
    }

    void close() {
        if (_sock_fd >= 0) {
            ::close(_sock_fd);
            _sock_fd = -1;
        }
    }

    [[nodiscard]] std::int32_t fd() const {
        return _sock_fd;
    }

    [[nodiscard]] std::string local_ip() const {
        return _addr_local.ip();
    }

    [[nodiscard]] std::uint16_t local_port() const {
        return _addr_local.port();
    }

    [[nodiscard]] std::string peer_ip() const {
        return _addr_peer.ip();
    }

    [[nodiscard]] std::uint16_t peer_port() const {
        return _addr_peer.port();
    }

    void set_flag_nonblock(bool on) const {
        set_flags(O_NONBLOCK, on);
    }

    void set_flag_closeonexec(bool on) const {
        set_flags(O_CLOEXEC, on);
    }

    void set_flags(std::int32_t flag, bool set) const {
        std::int32_t flags = fcntl(_sock_fd, F_GETFL, 0);
        if (flags < 0) {
            throw std::runtime_error("fcntl(F_GETFL) error with " + app_common::StringUtils::str_error(errno));
        }

        if (set) {
            flags |= flag;
        } else {
            flags &= ~flag;
        }

        if (fcntl(_sock_fd, F_SETFL, flags) < 0) {
            throw std::runtime_error("fcntl(F_SETFL) error with " + app_common::StringUtils::str_error(errno));
        }
    }

    void opt_reuse_addr(bool on) const {
        std::int32_t optval = on ? 1 : 0;
        if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
            throw std::runtime_error("setsockopt(SO_REUSEADDR) error with " +
                                     app_common::StringUtils::str_error(errno));
        }
    }

    void opt_reuse_port(bool on) const {
#ifdef SO_REUSEPORT
        std::int32_t v = on ? 1 : 0;
        if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0) {
            throw std::runtime_error("setsockopt(SO_REUSEPORT) error with " +
                                     app_common::StringUtils::str_error(errno));
        }
#endif
    }

    void opt_linger(bool on, std::int32_t linger_time) const {
        linger so_linger{.l_onoff = on ? 1 : 0, .l_linger = linger_time};
        if (setsockopt(_sock_fd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
            throw std::runtime_error("setsockopt(SO_LINGER) error with " +
                                     app_common::StringUtils::str_error(errno));
        }
    }

protected:
    std::int32_t _sock_fd;
    Ipv4Address  _addr_local;
    Ipv4Address  _addr_peer;
};

#endif
