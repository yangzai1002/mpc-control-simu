#ifndef ANCILLARY_DEV_BINOCULAR_SENDER_HPP
#define ANCILLARY_DEV_BINOCULAR_SENDER_HPP

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

#include "Receiver.hpp"
#include "fmt/format.h"

#define SENDER_RTSP "rtsp"
#define SENDER_28181 "28181"
#define SENDER_LOCALRECORD "record"

class Sender {
public:
    Sender() : _p(nullptr), _s(0) {
    }

public:
    virtual void content(const std::uint8_t *p, std::size_t s) {
        if (s < 1) {
            throw std::runtime_error("length error");
        }
        _p = p;
        _s = s;
    }

    virtual void send(const std::uint8_t *p, std::size_t s) = 0;

    virtual void sdp(const std::string &s) {
        _sdp = s;
    };

    [[maybe_unused]] virtual std::string sdp() {
        return _sdp;
    }

    virtual std::uint16_t receiver_count() {
        std::lock_guard<std::mutex> g(_receivers_mutex);

        return _receivers.size();
    }

    virtual void dump_receivers() {
        std::lock_guard<std::mutex> g(_receivers_mutex);
        for (const auto &x : _receivers) {
            std::cout << fmt::format("{}: {}", x.first, x.second->string()) << std::endl;
        }
    }

    virtual void add_receiver(const std::string &receiver_name, const std::shared_ptr<Receiver> &s) {
        std::lock_guard<std::mutex> g(_receivers_mutex);

        _receivers[receiver_name] = s;
    }

    virtual void del_receiver(const std::string &receiver_name) {
        std::lock_guard<std::mutex> g(_receivers_mutex);

        _receivers.erase(receiver_name);
    }

protected:
    std::mutex                                       _receivers_mutex;
    std::map<std::string, std::shared_ptr<Receiver>> _receivers;

    std::string _sdp;

    const std::uint8_t *_p;
    std::size_t         _s;
};

#endif // ANCILLARY_DEV_BINOCULAR_SENDER_HPP
