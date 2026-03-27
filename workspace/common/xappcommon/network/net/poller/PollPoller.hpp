#ifndef NET_POLLER_POLL_POLLER_H
#define NET_POLLER_POLL_POLLER_H

#include <poll.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include "Poller.hpp"
#include "StringUtils.hpp"
#include "fmt/format.h"

class PollPoller : public Poller {
public:
    explicit PollPoller(std::time_t timeout_in_ms = 1000) : _timeout(timeout_in_ms) {
    }

    ~PollPoller() override = default;

    void poller_event_add(const std::shared_ptr<PollerDesc>& event) noexcept(false) override {
        poller_event_update(event);
    }

    void poller_event_update(const std::shared_ptr<PollerDesc>& event) noexcept(false) override {
        event->validate();

        std::lock_guard<std::mutex> guard(_poller_mutex);

        try {
            auto& poller_event = _fd_map_to_poller_events.at(event->get_fd());

            try {
                auto index = _pollfd_index_array.at(event->get_fd());
                if (index >= _pollfd_array.size()) {
                    throw std::out_of_range("pollfd index too large");
                }
                auto& pollfd = _pollfd_array[index];
                pollfd.events = 0;
                pollfd.revents = 0;
                if (event->is_interested_in_read()) {
                    pollfd.events |= POLLIN;
                }

                if (event->is_interested_in_write()) {
                    pollfd.events |= POLLOUT;
                }

                if (event->is_interested_in_error()) {
                    pollfd.events |= POLLERR;
                }
            } catch (const std::out_of_range& e) {
                std::cout << fmt::format("pollfd index of fd {} does not exist-{}", event->get_fd(), e.what())
                          << std::endl;
                _fd_map_to_poller_events.erase(event->get_fd());
            }
        } catch (const std::out_of_range& e) {
            pollfd pollfd{};
            pollfd.fd = event->get_fd();
            pollfd.events = 0;
            pollfd.revents = 0;

            if (event->is_interested_in_read()) {
                pollfd.events |= POLLIN;
            }
            if (event->is_interested_in_write()) {
                pollfd.events |= POLLOUT;
            }
            if (event->is_interested_in_error()) {
                pollfd.events |= POLLERR;
            }

            _pollfd_array.push_back(pollfd);
            _pollfd_index_array.insert(std::make_pair(event->get_fd(), _pollfd_array.size() - 1));
            _fd_map_to_poller_events.insert(std::make_pair(event->get_fd(), event));
        }
    }

    void poller_event_remove(const std::shared_ptr<PollerDesc>& event) noexcept(false) override {
        event->validate();

        std::lock_guard<std::mutex> guard(_poller_mutex);

        _fd_map_to_poller_events.erase(event->get_fd());

        try {
            auto index = _pollfd_index_array.at(event->get_fd());
            if (index >= _pollfd_array.size()) {
                throw std::out_of_range("pollfd index too large");
            }
            _pollfd_array.erase(_pollfd_array.begin() + index);
            for (; index < _pollfd_array.size(); ++index) {
                _pollfd_index_array[_pollfd_array[index].fd] = index;
            }
        } catch (const std::out_of_range& e) {
            std::cout << fmt::format("pollfd index of fd {} does not exist-{}", event->get_fd(), e.what()) << std::endl;

            auto iter = std::find_if(_pollfd_array.begin(), _pollfd_array.end(),
                                     [event](const pollfd& p) { return p.fd == event->get_fd(); });
            if (iter == _pollfd_array.end()) {
                return;
            }
            _pollfd_array.erase(iter);
            for (std::int32_t i = 0; i < _pollfd_array.size(); ++i) {
                _pollfd_index_array[_pollfd_array[i].fd] = i;
            }
        }
    }

    void poller_do_poll() noexcept(false) override {
        if (_pollfd_array.empty())
            return;

        auto nums = poll(_pollfd_array.data(), _pollfd_array.size(), _timeout);
        if (nums < 0) {
            if (EINTR == errno) {
                return;
            }
            throw std::runtime_error("poll error-" + app_common::StringUtils::str_error(errno));
        }

        for (auto it = _pollfd_array.begin(); it != _pollfd_array.end() && nums > 0; ++it) {
            if (it->revents <= 0) {
                continue;
            }
            try {
                std::lock_guard<std::mutex> guard(_poller_mutex);
                auto&                       poller_event = _fd_map_to_poller_events.at(it->fd);

                auto events = it->revents;
                if (events & POLLIN || events & POLLPRI || events & POLLRDHUP) {
                    poller_event->add_received_event(PollerDesc::PollerEventType::EVENT_READ);
                }
                if (events & POLLOUT) {
                    poller_event->add_received_event(PollerDesc::PollerEventType::EVENT_WRITE);
                }
                if (events & POLLERR) {
                    poller_event->add_received_event(PollerDesc::PollerEventType::EVENT_ERROR);
                }

                _polled_events.push_back(poller_event);
            } catch (const std::out_of_range& e) {
                std::cout << fmt::format("fd {} does not exist", it->fd) << std::endl;
            }

            --nums;
        }

        // TODO: 应该分发到不同的线程处理
        //    std::lock_guard guard(poller_mutex);
        for (auto& e :
             _polled_events) { // TODO：注意在 m->handle_events 这个函数中，再调用 poller 相关函数，造成循环锁（死锁）
            e->handle_events();
        }
        _polled_events.clear();
    }

private:
    const std::int32_t                   _timeout;
    std::map<std::int32_t, std::int32_t> _pollfd_index_array; // epoll_fd -> index in PollFdList
    std::vector<pollfd>                  _pollfd_array;
};

#endif