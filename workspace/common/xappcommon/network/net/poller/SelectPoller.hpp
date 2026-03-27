#ifndef NET_POLLER_SELECT_POLLER_H
#define NET_POLLER_SELECT_POLLER_H
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#include "Poller.hpp"
#include "PollerDesc.hpp"
#include "StringUtils.hpp"

class SelectPoller : public Poller {
public:
    // clang-format off
    explicit SelectPoller(std::time_t timeout_in_ms = 1000)
        : _rd_fdset{}
        , _wr_fdset{}
        , _ex_fdset{}
        , _max_fd(0)
        , _tv{} {
        // clang-format on
        FD_ZERO(&_rd_fdset);
        FD_ZERO(&_wr_fdset);
        FD_ZERO(&_ex_fdset);

        _tv.tv_sec = timeout_in_ms / 1000;
        _tv.tv_usec = (timeout_in_ms % 1000) * 1000;
    }

    ~SelectPoller() override = default;

    void poller_event_add(const std::shared_ptr<PollerDesc>& event) noexcept(false) override {
        poller_event_update(event);
    }

    void poller_event_update(const std::shared_ptr<PollerDesc>& event) noexcept(false) override {
        event->validate();

        auto fd = event->get_fd();

        std::lock_guard<std::mutex> guard(_poller_mutex);

        FD_CLR(fd, &_rd_fdset);
        FD_CLR(fd, &_wr_fdset);
        FD_CLR(fd, &_ex_fdset);

        try {
            auto& x = _fd_map_to_poller_events.at(fd);
            if (event->is_interested_in_read()) {
                FD_SET(fd, &_rd_fdset);
            }

            if (event->is_interested_in_write()) {
                FD_SET(fd, &_wr_fdset);
            }

            if (event->is_interested_in_error()) {
                FD_SET(fd, &_ex_fdset);
            }
        } catch (const std::out_of_range& e) {
            if (event->is_interested_in_read()) {
                FD_SET(fd, &_rd_fdset);
            }

            if (event->is_interested_in_write()) {
                FD_SET(fd, &_wr_fdset);
            }
            if (event->is_interested_in_error()) {
                FD_SET(fd, &_ex_fdset);
            }

            _fd_map_to_poller_events.insert(std::make_pair(fd, event));
        }

        if (fd >= _max_fd) {
            _max_fd = fd + 1;
        }
    }

    void poller_event_remove(const std::shared_ptr<PollerDesc>& event) noexcept(false) override {
        event->validate();

        auto fd = event->get_fd();

        std::lock_guard<std::mutex> guard(_poller_mutex);

        FD_CLR(fd, &_rd_fdset);
        FD_CLR(fd, &_wr_fdset);
        FD_CLR(fd, &_ex_fdset);

        _fd_map_to_poller_events.erase(fd);

        auto it =
            std::max_element(_fd_map_to_poller_events.begin(), _fd_map_to_poller_events.end(),
                             [](const decltype(_fd_map_to_poller_events)::value_type& f,
                                const decltype(_fd_map_to_poller_events)::value_type& s) { return f.first < s.first; });
        if (it != _fd_map_to_poller_events.end()) {
            _max_fd = it->first + 1;
        } else {
            _max_fd = 0;
        }
    }

    void poller_do_poll() noexcept(false) override {
        if (_max_fd <= 0) {
            return;
        }

        fd_set  rd, wr, ex;
        timeval timeout{};

        {
            std::lock_guard<std::mutex> guard(_poller_mutex);
            memcpy(&rd, &_rd_fdset, sizeof(rd));
            memcpy(&wr, &_wr_fdset, sizeof(rd));
            memcpy(&ex, &_ex_fdset, sizeof(rd));
            timeout = _tv;
        }

        auto r = select(_max_fd, &rd, &wr, &ex, &timeout);
        if (r < 0) {
            if (EINTR == errno) {
                return;
            }
            throw std::runtime_error("select error-" + app_common::StringUtils::str_error(errno));
        }
        if (0 == r) {
            return;
        }

        for (auto it = _fd_map_to_poller_events.begin(); it != _fd_map_to_poller_events.end(); ++it) {
            if (FD_ISSET(it->first, &rd)) {
                it->second->add_received_event(PollerDesc::PollerEventType::EVENT_READ);
            }

            if (FD_ISSET(it->first, &wr)) {
                it->second->add_received_event(PollerDesc::PollerEventType::EVENT_WRITE);
            }

            if (FD_ISSET(it->first, &ex)) {
                it->second->add_received_event(PollerDesc::PollerEventType::EVENT_ERROR);
            }

            if (!it->second->is_interested_in_nothing() && !it->second->is_event_received_nothing()) {
                std::lock_guard<std::mutex> guard(_poller_mutex);
                _polled_events.push_back(it->second);
            }
        }

        // TODO: 这里应该分发到不同的线程去处理
        //    std::lock_guard guard(poller_mutex);
        for (auto& m :
             _polled_events) { // TODO：注意在 m->handle_events 这个函数中，再调用 poller 相关函数，造成循环锁（死锁）
            m->handle_events();
        }

        _polled_events.clear();
    }

private:
    fd_set       _rd_fdset;
    fd_set       _wr_fdset;
    fd_set       _ex_fdset;
    timeval      _tv;
    std::int32_t _max_fd;
};

#endif