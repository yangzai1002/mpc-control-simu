#ifndef MEDIA_FILE_RECEIVER_HPP
#define MEDIA_FILE_RECEIVER_HPP

#include <fcntl.h>
#include <sys/stat.h>

#include <iostream>
#include <string>

#include "../base/UsageEnvironment.hpp"
#include "Receiver.hpp"

class FileReceiver : public Receiver {
public:
    FileReceiver(std::shared_ptr<UsageEnvironment> env, std::string n)
        : _env(std::move(env)), _name(std::move(n)), _fd(-1) {
        // clang-format on
        std::cout << fmt::format("---- use file receiver ----") << std::endl;
        _fd = open(_name.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IWGRP | S_IWUSR | S_IRGRP | S_IRUSR);
        if (_fd < 0) {
            throw std::runtime_error("open error-" + app_common::StringUtils::str_error(errno));
        }
    }

    ~FileReceiver() override {
        if (_fd >= 0) {
            close(_fd);
        }
    }

public:
    [[nodiscard]] std::string string() const override {
        return "file: " + _name;
    }

    void push(const std::uint8_t *p, std::size_t s, std::int32_t arg) noexcept(false) override {
        std::size_t offset = 0;
        while (offset < s) {
            auto n = write(_fd, p + offset, s - offset);
            if (n < 0) {
                auto es = app_common::StringUtils::str_error(errno);
                std::cout << fmt::format("write error: {}", es) << std::endl;
                _env->malfunction_occur(Malfunction::MalType::MALTYPE_HW_SD, es);
                return;
            }
            offset += n;
        }
    }

    void push(const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2, std::size_t s2,
              std::int32_t arg) noexcept(false) override {
        push(p1, s1, arg);
        push(p2, s2, arg);
    }

protected:
    const std::shared_ptr<UsageEnvironment> _env;
    const std::string                       _name;
    std::int32_t                            _fd;
};

#endif
