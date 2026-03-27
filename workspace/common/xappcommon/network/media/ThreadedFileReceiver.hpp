#ifndef MEDIA_THREADED_FILE_RECEIVER_HPP
#define MEDIA_THREADED_FILE_RECEIVER_HPP

#include <fcntl.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

#include "FileReceiver.hpp"
#include "RingBuffer.hpp"
#include "StringUtils.hpp"
#include "fmt/format.h"

class ThreadedFileReceiver : public FileReceiver {
public:
    ThreadedFileReceiver(std::shared_ptr<UsageEnvironment> env, std::string path)
        : FileReceiver(std::move(env), std::move(path)), _stopped(false), _buf(1024 * 1024 * 100) {
        std::cout << fmt::format("---- use threaded file receiver ----") << std::endl;
        _fd = open(_name.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IWGRP | S_IWUSR | S_IRGRP | S_IRUSR);
        if (_fd < 0) {
            throw std::runtime_error("open error-" + app_common::StringUtils::str_error(errno));
        }

        _thread_write = std::thread([this]() { loop(); });
    }

    ~ThreadedFileReceiver() override {
        _stopped = true;
        if (_thread_write.joinable()) {
            _thread_write.join();
        }

        if (_fd >= 0) {
            close(_fd);
            _fd = -1;
        }
    }

public:
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

            std::int64_t offset = 0;
            while (offset < l) {
                auto r = write(_fd, buf, l);
                if (r < 0) {
                    auto es = app_common::StringUtils::str_error(errno);
                    std::cout << fmt::format("write error: {}", es) << std::endl;
                    _env->malfunction_occur(Malfunction::MalType::MALTYPE_HW_SD, es);
                    break;
                }
                offset += l;
            }
        }
    }

private:
    std::atomic_bool _stopped;
    std::thread      _thread_write;
    RingBuffer       _buf;
};

#endif
