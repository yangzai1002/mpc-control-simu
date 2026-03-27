#ifndef NET_TCP_SOCKET_HPP
#define NET_TCP_SOCKET_HPP

#include <netinet/tcp.h>

#include <stdexcept>

#include "Socket.hpp"
#include "StringUtils.hpp"

class TcpSocket : public Socket {
public:
    using Socket::Socket;

    void opt_nodealy(bool no_delay) const {
        std::int32_t on = no_delay ? 1 : 0;
        if (setsockopt(_sock_fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) < 0) {
            throw std::runtime_error("setsockopt(TCP_NODELAY) error with " +
                                     app_common::StringUtils::str_error(errno));
        }
    }
};

#endif
