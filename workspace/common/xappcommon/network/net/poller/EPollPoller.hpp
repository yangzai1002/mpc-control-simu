#ifndef NET_POLLER_EPOLLPOLLER_H
#define NET_POLLER_EPOLLPOLLER_H

#include <sys/epoll.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "Poller.hpp"
#include "StringUtils.hpp"
#include "fmt/format.h"

class EPollPoller : public Poller {
public:
    explicit EPollPoller(std::int32_t timeout_in_ms = 1000) : timeout(timeout_in_ms), epoll_events(16) {
        epoll_fd = epoll_create1(EPOLL_CLOEXEC);
        if (epoll_fd < 0) {
            throw std::runtime_error("epoll_create1 error-" + app_common::StringUtils::str_error(errno));
        }
    }

    ~EPollPoller() override {
        close(epoll_fd);
        epoll_fd = -1;
    }

    void poller_event_add(const std::shared_ptr<PollerDesc>& event) noexcept(false) override {
        poller_event_update(event);
    }

    void poller_event_update(const std::shared_ptr<PollerDesc>& event) noexcept(false) override {
        event->validate();

        epoll_event ev{};

        ev.data.fd = event->get_fd();
        if (event->is_interested_in_read()) {
            ev.events |= EPOLLIN;
        }

        if (event->is_interested_in_write()) {
            ev.events |= EPOLLOUT;
        }

        if (event->is_interested_in_error()) {
            ev.events |= EPOLLERR;
        }

        std::lock_guard<std::mutex> guard(_poller_mutex);
        try {
            auto& x = _fd_map_to_poller_events.at(event->get_fd());
            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, event->get_fd(), &ev);
        } catch (const std::out_of_range& e) {
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event->get_fd(), &ev);

            _fd_map_to_poller_events.insert(std::make_pair(event->get_fd(), event));
            if (_fd_map_to_poller_events.size() >= epoll_events.size())
                epoll_events.resize(epoll_events.size() * 2);
        }
    }

    void poller_event_remove(const std::shared_ptr<PollerDesc>& event) noexcept(false) override {
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event->get_fd(), nullptr);

        std::lock_guard<std::mutex> guard(_poller_mutex);

        _fd_map_to_poller_events.erase(event->get_fd());
    }

    void poller_do_poll() override {
        auto nums = epoll_wait(epoll_fd, epoll_events.data(), (std::int32_t)epoll_events.size(), timeout);
        if (nums < 0) {
            if (EINTR == errno) {
                return;
            }
            throw std::runtime_error("epoll_wait error-" + app_common::StringUtils::str_error(errno));
        }

        for (int i = 0; i < nums; ++i) {
            auto& e = epoll_events[i];
            auto  fd = e.data.fd;
            auto  ev = e.events;

            try {
                std::lock_guard<std::mutex> guard(_poller_mutex);
                auto&                       poller_event = _fd_map_to_poller_events.at(fd);

                if (ev & EPOLLIN || ev & EPOLLPRI || ev & EPOLLRDHUP) {
                    poller_event->add_received_event(PollerDesc::PollerEventType::EVENT_READ);
                }
                if (ev & EPOLLOUT) {
                    poller_event->add_received_event(PollerDesc::PollerEventType::EVENT_WRITE);
                }
                if (ev & EPOLLERR) {
                    poller_event->add_received_event(PollerDesc::PollerEventType::EVENT_ERROR);
                }
                if (!poller_event->is_event_received_nothing() && !poller_event->is_interested_in_nothing()) {
                    _polled_events.push_back(poller_event);
                }
            } catch (const std::out_of_range& e) {
                std::cout << fmt::format("fd {} does not exist", fd) << std::endl;
            }
        }

        // TODO: 应该分发到不同的线程去处理
        //    std::lock_guard guard(poller_mutex);
        for (auto& e :
             _polled_events) { // TODO：注意在 m->handle_events 这个函数中，再调用 poller 相关函数，造成循环锁（死锁）
            e->handle_events();
        }

        _polled_events.clear();
    }

private:
    const std::int32_t       timeout;
    int                      epoll_fd;
    std::vector<epoll_event> epoll_events;
};

#endif
