#ifndef NET_ACCEPTOR_H
#define NET_ACCEPTOR_H

#include <functional>
#include <iostream>
#include <utility>

#include "../base/UsageEnvironment.hpp"
#include "StringUtils.hpp"
#include "TcpSocketOfAcceptor.hpp"
#include "TcpSocketOfServerSide.hpp"
#include "fmt/format.h"

class Acceptor {
public:
    // clang-format off
    Acceptor(std::shared_ptr<UsageEnvironment> env
        , std::function<void(const std::shared_ptr<TcpSocketOfServerSide>&)> cb_newconn)
        : _environment(std::move(env))
        , _tcp_socket(_environment->listen_ip(), _environment->listen_port())
        , _poller_event(
            std::make_shared<PollerDesc>(_tcp_socket.fd(), (std::uint32_t)PollerDesc::PollerEventType::EVENT_READ,
              std::optional<std::function<void()>>([this]() { do_accept(); }), std::nullopt, std::nullopt))
        , _callback_newconn(std::move(cb_newconn)) {
        // clang-format on
        _tcp_socket.opt_reuse_addr(true);
    }

    // clang-format on

    ~Acceptor() {
        _environment->poller_event_remove(_poller_event);
    }

    void listen() {
        _tcp_socket.listen(1024);
        _environment->poller_event_add(_poller_event);
    }

    std::string ip() {
        return _tcp_socket.local_ip();
    }

    std::uint16_t port() {
        return _tcp_socket.local_port();
    }

private:
    void do_accept() {
        try {
            auto client_socket = _tcp_socket.accept();
            std::cout << fmt::format("client connect, fd {}, ip {}, port {}", client_socket->fd(),
                                     client_socket->peer_ip(), client_socket->peer_port())
                      << std::endl;
            _callback_newconn(client_socket);
        } catch (const std::runtime_error& e) {
            std::cout << fmt::format("client conn error: {}", e.what()) << std::endl;
        }
    }

private:
    std::shared_ptr<UsageEnvironment>                                    _environment;
    TcpSocketOfAcceptor                                                  _tcp_socket;
    std::shared_ptr<PollerDesc>                                          _poller_event;
    std::function<void(const std::shared_ptr<TcpSocketOfServerSide>& c)> _callback_newconn;
};

#endif
