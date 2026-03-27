#ifndef MEDIA_THREADED_ROTATING_FILE_RECEIVER_HPP
#define MEDIA_THREADED_ROTATING_FILE_RECEIVER_HPP
#include <unistd.h>

#include <RingBuffer.hpp>
#include <atomic>
#include <iostream>
#include <thread>

#include "RotatingFileReceiver.hpp"
#include "StringUtils.hpp"
#include "fmt/format.h"

class ThreadedRotatingFileReceiver : public RotatingFileReceiver {
public:
    ThreadedRotatingFileReceiver(std::string location, std::string name, std::uint64_t file_duration,
                                 std::uint64_t file_count)
        : RotatingFileReceiver(std::move(location), std::move(name), file_duration, file_count)
        , _stopped(false)
        , _buf(1024 * 1024 * 100) {
        _thread_write = std::thread([this]() { loop(); });
        check_limit_and_open();
    }

    ~ThreadedRotatingFileReceiver() override {
        _stopped = true;
        if (_thread_write.joinable()) {
            _thread_write.join();
        }

        if (_current_fd >= 0) {
            close(_current_fd);
            _current_fd = -1;
        }
    }

    void push(const std::uint8_t *p, std::size_t s, std::int32_t arg) override {
        _buf.write(p, s);
    }

    void push(const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2, std::size_t s2,
              std::int32_t arg) noexcept(false) override {
        _buf.write(p1, s1);
        _buf.write(p2, s2);
    }

private:
    void loop() {
        std::uint8_t buf[2048];

        while (!_stopped) {
            auto l = _buf.read(buf, sizeof(buf), 4);
            if (0 == l) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            check_limit_and_open();

            std::int64_t offset = 0;
            while (offset < l) {
                auto r = write(_current_fd, buf, l);
                if (r < 0) {
                    auto es = app_common::StringUtils::str_error(errno);
                    std::cout << fmt::format("write error: {}", es) << std::endl;

                    break;
                }
                offset += l;
                _current_size += l;
            }
        }
    }

private:
    std::atomic_bool       _stopped;
    std::thread            _thread_write;
    app_common::RingBuffer _buf;
};

#endif
