#ifndef MEDIA_LOCAL_RECORD_SENDER_HPP
#define MEDIA_LOCAL_RECORD_SENDER_HPP

#include "Sender.hpp"

class LocalRecordSender : public Sender {
public:
    void send(const std::uint8_t *p, std::size_t s) override {
        std::lock_guard<std::mutex> g(_receivers_mutex);

        for (auto &x : _receivers) {
            x.second->push(p, s, 0);
        }
    }
};

#endif
