#ifndef NET_TCP_SOCKET_OF_ACCEPTOR_HPP
#define NET_TCP_SOCKET_OF_ACCEPTOR_HPP

#include <unistd.h>

#include <cstdint>
#include <memory>
#include <stdexcept>

#include "InetAddress.hpp"
#include "StringUtils.hpp"
#include "TcpSocket.hpp"
#include "TcpSocketOfServerSide.hpp"

class TcpSocketOfAcceptor : public TcpSocket {
public:
    TcpSocketOfAcceptor(const std::string& ip, std::uint16_t port) : TcpSocket() {
        std::string error_desc;

        _sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
        if (_sock_fd < 0) {
            error_desc = "socket error with " + app_common::StringUtils::str_error(errno);
            goto error;
        }

        try {
            opt_reuse_addr(true);
        } catch (const std::runtime_error& e) {
            error_desc = e.what();
            goto error;
        }

        _addr_local = Ipv4Address(ip, port);
        {
            auto in_addr = _addr_local.to_sockaddr_in();
            if (bind(_sock_fd, reinterpret_cast<const sockaddr*>(&in_addr), sizeof(in_addr)) < 0) {
                error_desc = "bind error-" + app_common::StringUtils::str_error(errno);
                goto error;
            }
        }
        return;

    error:
        close();
        throw std::runtime_error(error_desc);
    }

    ~TcpSocketOfAcceptor() override = default;

public:
    void listen(std::int32_t backlog) const {
        if (::listen(_sock_fd, backlog) < 0) {
            throw std::runtime_error("listen error-" + app_common::StringUtils::str_error(errno));
        }
    }

    [[nodiscard]] std::shared_ptr<TcpSocketOfServerSide> accept() const {
        sockaddr_in addr{};
        socklen_t   addrlen = sizeof(addr);

        auto fd = ::accept(_sock_fd, (struct sockaddr*)&addr, &addrlen);
        if (fd < 0) {
            throw std::runtime_error("accept error with " + app_common::StringUtils::str_error(errno));
        }

        try {
            // clang-format off
            auto sock = std::make_shared<TcpSocketOfServerSide>(
                fd
                , _addr_local.ip()
                , _addr_local.port()
                , app_common::StringUtils::inet_ntoa(addr.sin_addr.s_addr)
                , ntohs(addr.sin_port));
            // clang-format on
            return sock;
        } catch (const std::runtime_error& e) {
            ::close(fd);
            std::rethrow_exception(std::current_exception());
        }
    }
};

#endif
