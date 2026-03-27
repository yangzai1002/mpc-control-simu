#ifndef NET_POLLER_POLLER_H
#define NET_POLLER_POLLER_H

#include <list>
#include <map>
#include <mutex>

#include "PollerDesc.hpp"

class Poller {
public:
    Poller() = default;

    virtual ~Poller() = default;

    virtual void poller_event_add(const std::shared_ptr<PollerDesc>& event) noexcept(false) = 0;

    virtual void poller_event_update(const std::shared_ptr<PollerDesc>& event) noexcept(false) = 0;

    virtual void poller_event_remove(const std::shared_ptr<PollerDesc>& event) noexcept(false) = 0;

    virtual void poller_do_poll() noexcept(false) = 0;

protected:
    std::mutex                                 _poller_mutex;
    std::map<int, std::shared_ptr<PollerDesc>> _fd_map_to_poller_events;
    std::list<std::shared_ptr<PollerDesc>>     _polled_events;
};

#endif
