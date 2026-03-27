#ifndef MEDIA_RAW_SENDER_HPP
#define MEDIA_RAW_SENDER_HPP

#include <netinet/in.h>

#include <map>
#include <memory>
#include <string>

#include "Sender.hpp"

class RawSender : public Sender {
public:
    RawSender() : Sender() {
    }

    ~RawSender() = default;

    void send(const std::uint8_t *p, std::size_t s) override {
        std::lock_guard<std::mutex> g(_receivers_mutex);

        for (auto &r : _receivers) {
            if (0 == r.second->error()) { //
                try {
                    r.second->push(p, s, 0);
                } catch (const std::runtime_error &e) {
                    std::cout << fmt::format("push to {} error: {}", r.first, e.what()) << std::endl;
                }
            }
        }
    }
};

#endif
