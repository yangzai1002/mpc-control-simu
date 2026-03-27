#ifndef NET_TCP_CONN_OF_SERVER_SIDE_HPP
#define NET_TCP_CONN_OF_SERVER_SIDE_HPP
#include <iostream>
#include <utility>

#include "../base/UsageEnvironment.hpp"
#include "SimpleSocketBuffer.hpp"
#include "StringUtils.hpp"
#include "TcpSocketOfServerSide.hpp"
#include "fmt/format.h"

class TcpConnOfServerSide {
public:
    // clang-format off
    TcpConnOfServerSide(
        std::shared_ptr<UsageEnvironment> env
        , std::function<void(std::int32_t)> cb_disconn
        , std::shared_ptr<TcpSocketOfServerSide> c)    : _environment(std::move(env))
        , _callback_disconn(std::move(cb_disconn))
        , _tcp_socket(std::move(c))
        , _input(1024)
        , _output(1024) {
        _poller_desc = std::make_shared<PollerDesc>(
                    _tcp_socket->fd(),
                    (std::uint32_t)PollerDesc::PollerEventType::EVENT_RDEX,
                   std::optional<std::function<void()>>( [this]() { on_read_ready(); }),
                   std::optional<std::function<void()>>( [this]() { on_write_ready(); }),
                   std::optional<std::function<void()>>( [this]() { on_exception_occur(); }));
        // clang-format on
        _environment->poller_event_add(_poller_desc);
    }

    virtual ~TcpConnOfServerSide() {
        _environment->poller_event_remove(_poller_desc);
        std::cout << fmt::format("########### destroy tcp connection {}:{}", _tcp_socket->peer_ip(),
                                 _tcp_socket->peer_port())
                  << std::endl;
    }

    virtual void close() {
        _tcp_socket->close();
    }

    virtual void send_sync(const std::uint8_t *p, std::size_t s) {
        std::size_t off = 0;
        while (off < s) {
            auto n = write(_tcp_socket->fd(), p + off, s - off);
            if (n < 0) {
                throw std::runtime_error("write error-" + app_common::StringUtils::str_error(errno));
            }

            off += n;
        }
    }

    virtual void sendmsg_sync(const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2, std::size_t s2) {
        iovec iov[2];
        iov[0].iov_base = (void *)p1;
        iov[0].iov_len = s1;
        iov[1].iov_base = (void *)p2;
        iov[1].iov_len = s2;

        msghdr msg{};
        msg.msg_iov = iov;
        msg.msg_iovlen = 2;

        if (sendmsg(_tcp_socket->fd(), &msg, 0) < 0) {
            throw std::runtime_error("sendmsg error-" + app_common::StringUtils::str_error(errno));
        }
    }

protected:
    virtual void on_read_ready() {
        try {
            auto r = _input.read(_tcp_socket->fd());
            if (0 == r) {
                std::cout << fmt::format("client disconnect, fd {}, ip {}, port {}", _tcp_socket->fd(),
                                         _tcp_socket->peer_ip(), _tcp_socket->peer_port())
                          << std::endl;
                do_close();
                return;
            }
        } catch (const std::runtime_error &e) {
            std::cout << fmt::format("client read error, fd {}, ip {}, port {}, error {}", _tcp_socket->fd(),
                                     _tcp_socket->peer_ip(), _tcp_socket->peer_port(), e.what())
                      << std::endl;
            do_close();
            return;
        }

        handleReadBytes();
    }

    virtual void on_write_ready() {
        std::cout << fmt::format("default wirte handle\n") << std::endl;
        //    mOutBuffer.retrieveAll();
    }

    virtual void on_exception_occur() {
        std::cout << fmt::format("default error handle\n") << std::endl;
    }

    virtual void handleReadBytes() {
        std::cout << fmt::format("default read handle") << std::endl;
        //    mInputBuffer.retrieveAll();
    }

private:
    void do_close() {
        _callback_disconn(_tcp_socket->fd());
    }

protected:
    std::shared_ptr<UsageEnvironment> _environment;

private:
    std::function<void(std::int32_t)> _callback_disconn;

protected:
    std::shared_ptr<TcpSocketOfServerSide>  _tcp_socket;
    std::shared_ptr<PollerDesc>             _poller_desc;
    app_common::SimpleSocketBuffer _input;
    app_common::SimpleSocketBuffer _output;
};

#endif