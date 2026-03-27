#ifndef NET_TCP_SERVER_HPP
#define NET_TCP_SERVER_HPP
#include <map>

#include "../base/UsageEnvironment.hpp"
#include "Acceptor.hpp"
#include "InetAddress.hpp"
#include "TcpConnOfServerSide.hpp"

class TcpServer {
public:
    TcpServer(const std::shared_ptr<UsageEnvironment>& env)
        : _environment(env)
        , _acceptor(std::unique_ptr<Acceptor>(
              new Acceptor(env, [this](const std::shared_ptr<TcpSocketOfServerSide>& c) { on_new_connection(c); }))) {
    }

    virtual ~TcpServer() = default;

    virtual void start() {
        _acceptor->listen();
    }

    virtual void stop() {
        _acceptor.reset();
    }

    std::string ip() {
        return _acceptor->ip();
    }

    std::uint16_t port() {
        return _acceptor->port();
    }

protected:
    virtual void on_new_connection(const std::shared_ptr<TcpSocketOfServerSide>&) = 0;
    // virtual void on_end_connection(int sockfd);

protected:
    std::shared_ptr<UsageEnvironment> _environment;
    std::unique_ptr<Acceptor>         _acceptor;
};

#endif
