#ifndef NET_TCP_CONN_OF_CLIENT_SIDE_HPP
#define NET_TCP_CONN_OF_CLIENT_SIDE_HPP

#include <iostream>

#include "../base/UsageEnvironment.hpp"
#include "SimpleSocketBuffer.hpp"
#include "StringUtils.hpp"
#include "TcpSocketOfClientSide.hpp"
#include "fmt/format.h"

class TcpConnOfClientSide {
public:
    // clang-format off
    TcpConnOfClientSide(
        std::shared_ptr<UsageEnvironment> env
        , const std::string &local_ip
        , std::uint16_t local_port
        , const std::string &peer_ip
        , std::uint16_t peer_port)
        : _environment(std::move(env))
        , _tcp_socket(peer_ip, peer_port)
        , _poller_desc(nullptr)
        , _input(1024)
        , _output(1024) {
        // clang-format on
        _tcp_socket.connect(5000);

        // clang-format off
        _poller_desc = std::make_shared<PollerDesc>(
            _tcp_socket.fd()
            , (std::uint32_t)PollerDesc::PollerEventType::EVENT_ALL
            ,std::optional<std::function<void()>>( [this]() { on_rd_ready(); })
            ,std::optional<std::function<void()>>( [this]() { on_wr_ready(); })
            ,std::optional<std::function<void()>>( [this]() { on_ex_ready(); })
        );
        // clang-format on
        _environment->poller_event_add(_poller_desc);
    }

    virtual ~TcpConnOfClientSide() {
        if (_poller_desc) {
            _environment->poller_event_remove(_poller_desc);
            std::cout << fmt::format("########### destroy tcp connection {}:{}", _tcp_socket.peer_ip(),
                                     _tcp_socket.peer_port())
                      << std::endl;
        }
    };

protected:
    void on_rd_ready() {
        try {
            auto r = _input.read(_tcp_socket.fd());
            if (0 == r) {
                std::cout << fmt::format("connection closed, fd {}, ip {}, port {}", _tcp_socket.fd(),
                                         _tcp_socket.peer_ip(), _tcp_socket.peer_port())
                          << std::endl;
                on_disconn();
                return;
            }
        } catch (const std::runtime_error &e) {
            std::cout << fmt::format("connection read error, fd {}, ip {}, port {}, error {}", _tcp_socket.fd(),
                                     _tcp_socket.peer_ip(), _tcp_socket.peer_port(), e.what())
                      << std::endl;
            on_disconn();
            return;
        }

        on_recv();
    }

    void on_wr_ready() {
        on_conn();
        _poller_desc->discard_write();
        _environment->poller_event_update(_poller_desc);
    }

    void on_ex_ready() {
    }

    virtual void on_conn() {
    }

    virtual void on_disconn() {
    }

    virtual void on_recv() {
    }

    void send_sync(const void *p, std::size_t s) {
        auto        d = (const std::uint8_t *)p;
        std::size_t off = 0;
        while (off < s) {
            auto n = write(_tcp_socket.fd(), d + off, s - off);
            if (n < 0) {
                throw std::runtime_error("write error with " + app_common::StringUtils::str_error(errno));
            }
            off += n;
        }
    }

protected:
    std::shared_ptr<UsageEnvironment> _environment;

protected:
    TcpSocketOfClientSide          _tcp_socket;
    std::shared_ptr<PollerDesc>    _poller_desc;
    app_common::SimpleSocketBuffer _input;
    app_common::SimpleSocketBuffer _output;

private:
    std::function<void(std::int32_t)> _callback_disconn;
};

#endif
