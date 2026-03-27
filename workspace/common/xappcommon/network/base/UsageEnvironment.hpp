#ifndef BASE_USAGE_ENVIRONMENT_HPP
#define BASE_USAGE_ENVIRONMENT_HPP

#include <sys/time.h>

#include <atomic>

#include "../media/MediaMan.hpp"
#include "../media/Sender.hpp"
#include "../net/poller/EPollPoller.hpp"
#include "../net/poller/PollPoller.hpp"
#include "../net/poller/Poller.hpp"
#include "../net/poller/PollerDesc.hpp"
#include "../net/poller/SelectPoller.hpp"
#include "Malfunction.hpp"

class UsageEnvironment {
public:
    enum PollerType { POLLER_SELECT, POLLER_POLL, POLLER_EPOLL };

    explicit UsageEnvironment(PollerType type, std::string ip, std::int32_t port, std::int32_t connection_expiry_ms)
        : _listen_ip(std::move(ip))
        , _listen_port(port)
        , _connection_expiry_ms(connection_expiry_ms)
        , _stopped(false)
        , _start_time(0) {
        switch (type) {
            case POLLER_SELECT:
                _poller = std::make_shared<SelectPoller>();
                break;
            case POLLER_POLL:
                _poller = std::make_shared<PollPoller>();
                break;
            case POLLER_EPOLL:
                _poller = std::make_shared<EPollPoller>();
                break;
            default:
                throw std::runtime_error("poller type error");
        }

        _malfunction = std::make_shared<Malfunction>();
        _start_time = current_milliseconds();
    }

    ~UsageEnvironment() = default;

    std::int64_t current_milliseconds() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return (std::int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    std::string listen_ip() const {
        return _listen_ip;
    }

    std::int32_t listen_port() const {
        return _listen_port;
    }

    std::int32_t connection_expiry_ms() const {
        return _connection_expiry_ms;
    }

    void poller_event_add(const std::shared_ptr<PollerDesc>& event) {
        _poller->poller_event_add(event);
    }

    void poller_event_update(const std::shared_ptr<PollerDesc>& event) {
        _poller->poller_event_update(event);
    }

    void poller_event_remove(const std::shared_ptr<PollerDesc>& event) {
        _poller->poller_event_remove(event);
    }

    void malfunction_occur(Malfunction::MalType type, std::string reason) {
        _malfunction->occur(type, std::move(reason));
    }

    std::list<Malfunction::MalEntry> malfunctions() {
        return std::move(_malfunction->malfunctions());
    }

    std::int64_t start_time() {
        return _start_time;
    }

    void loop() {
        //    while (!_stopped) {
        _poller->poller_do_poll();
        //    }
    }

    void stop() {
        _stopped = true;
        std::lock_guard<std::mutex> g(_mutex);
        _media_man.clear();
    }

    void media_man_add(const std::string& name, std::shared_ptr<MediaMan> m) {
        std::lock_guard<std::mutex> guard(_mutex);
        try {
            auto s = _media_man.at(name);
            if (s != m) {
                throw std::runtime_error(fmt::format("media man {} already exist", name));
            }
        } catch (const std::out_of_range& e) {
            _media_man.emplace(name, std::move(m));
        }
    };

    void media_man_del(const std::string& name, const std::shared_ptr<MediaMan>& m) {
        std::lock_guard<std::mutex> guard(_mutex);
        try {
            auto s = _media_man.at(name);
            if (s != m) {
                throw std::runtime_error(fmt::format("media man {} mismatch", name));
            }
            _media_man.erase(name);
        } catch (const std::out_of_range& e) {
        }
    };

    std::shared_ptr<MediaMan> media_man_get(const std::string& name) {
        std::lock_guard<std::mutex> guard(_mutex);

        try {
            auto& s = _media_man.at(name);
            return s;
        } catch (const std::out_of_range& e) {
            throw std::runtime_error(fmt::format("media man {} cannot found", name));
        }
    };

    void media_man_dump() {
        std::lock_guard<std::mutex> g(_mutex);

        for (const auto& m : _media_man) {
            std::cout << fmt::format("==== dump media man {}", m.first) << std::endl;
            m.second->dump_receivers();
        }
    };

    std::string media_url(const std::string& name) {
        std::lock_guard<std::mutex> guard(_mutex);

        try {
            auto& s = _media_man.at(name);
            return fmt::format("rtsp://{}:{}/{}", app_common::NetUtils::get_dev_ip("eth0"), _listen_port,
                               name);
        } catch (const std::out_of_range& e) {
            throw std::runtime_error(fmt::format("media man {} cannot found", name));
        }
    };

private:
    const std::string  _listen_ip;
    const std::int32_t _listen_port;
    const std::int32_t _connection_expiry_ms;

    std::mutex                                       _mutex;
    std::map<std::string, std::shared_ptr<MediaMan>> _media_man;

    std::atomic_bool             _stopped;
    std::shared_ptr<Poller>      _poller;
    std::shared_ptr<Malfunction> _malfunction;
    std::int64_t                 _start_time;
};

#endif
