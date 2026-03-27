#ifndef NET_TCP_SOCKET_OF_SERVER_SIDE_HPP
#define NET_TCP_SOCKET_OF_SERVER_SIDE_HPP

#include <cstdint>

#include "InetAddress.hpp"
#include "TcpSocket.hpp"

class TcpSocketOfServerSide : public TcpSocket {
public:
    using TcpSocket::TcpSocket;
};

#endif
