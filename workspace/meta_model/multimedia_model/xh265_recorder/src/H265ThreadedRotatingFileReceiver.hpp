#ifndef _H265_THREADED_ROTATING_FILE_RECEIVER_HPP
#define _H265_THREADED_ROTATING_FILE_RECEIVER_HPP
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <thread>

#include "DatetimeUtil.hpp"
#include "H265RotatingFileReceiver.hpp"
#include "SafeQueue.hpp"

class H265ThreadedRotatingFileReceiver : public H265RotatingFileReceiver {
public:
    H265ThreadedRotatingFileReceiver() : H265RotatingFileReceiver(), _stopped(false) {
        _thread_write = std::thread([this]() { loop(); });
    }

    ~H265ThreadedRotatingFileReceiver() override {
        _stopped = true;
        if (_thread_write.joinable()) {
            _thread_write.join();
        }

        if (_current_fd >= 0) {
            close(_current_fd);
            _current_fd = -1;
        }
    }

    void push(const std::uint8_t* p, std::size_t s) override {
        _nalus.push(std::vector<std::uint8_t>(p, p + s));
    }

private:
    void loop() {
        pthread_setname_np(pthread_self(), "h265_file_writer");
        auto                      last_recv = app_common::DatetimeUtil::current_millisecond();
        std::vector<std::uint8_t> nalu;
        while (!_stopped) {
            auto now = app_common::DatetimeUtil::current_millisecond();
            if (!_nalus.timeout_move_pop(nalu, 100000)) {
                if (now - last_recv > 2000) {
                    ApError("h265_recorder") << "no data for 2s";
                    _ec.ec_add(_ERRORCODE_NO_DATA);
                } else {
                    _ec.ec_remove(_ERRORCODE_NO_DATA);
                }
                continue;
            }
            last_recv = now;

            H265RotatingFileReceiver::push(nalu.data(), nalu.size());
        }
    }

private:
    std::atomic_bool                                 _stopped;
    std::thread                                      _thread_write;
    app_common::SafeQueue<std::vector<std::uint8_t>> _nalus;
};

#endif
