#ifndef MEDIA_ROTATING_FILE_RECEIVER_HPP
#define MEDIA_ROTATING_FILE_RECEIVER_HPP

#include <fcntl.h>

#include <Datetime14Util.hpp>
#include <filesystem>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <utility>

#include "Receiver.hpp"
#include "StringUtils.hpp"
#include "fmt/format.h"

class RotatingFileReceiver : public Receiver {
public:
    struct Record {
        Record(std::string &&n, std::size_t size) : _file(std::move(n)), _size(size) {
        }
        std::string _file;
        std::size_t _size;

        bool operator<(const Record &o) const {
            return _file < o._file;
        }
    };

    // clang-format off
    RotatingFileReceiver(std::string location
        , std::string name
        , std::uint64_t file_duration
        , std::uint64_t file_count): Receiver()
    , _location(std::move(location))
    , _name(std::move(name))
    , _file_limit_duration(file_duration)
    , _file_limit_number(file_count)
    , _total_size(0)
    , _current_fd (-1)
    , _current_size(0)
    , _current_start(0) {
        // clang-format on
        if (0 == _file_limit_duration || 0 == _file_limit_number) {
            throw std::runtime_error("invalid argument");
        }
        enum_exist_files();
        check_limit_and_open();
    }

    ~RotatingFileReceiver() override {
        if (_current_fd >= 0) {
            close(_current_fd);
            _current_fd = -1;
        }
    }

    [[nodiscard]] std::string string() const override {
        return std::filesystem::path(_location).append(_name).string();
    }

    void push(const std::uint8_t *p, std::size_t s, std::int32_t arg) noexcept(false) override {
        check_limit_and_open();

        std::size_t offset = 0;
        while (offset < s) {
            auto n = write(_current_fd, p + offset, s - offset);
            if (n < 0) {
                error(errno);
                auto es = app_common::StringUtils::str_error(errno);
                std::cout << fmt::format("write error: {}", es) << std::endl;
                return;
            }
            offset += n;
            _current_size += n;
        }
    }

    void push(const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2, std::size_t s2,
              std::int32_t arg) noexcept(false) override {
        push(p1, s1, arg);
        push(p2, s2, arg);
    }

protected:
    void check_limit_and_open() {
        bool need_create = false;

        if (_current_fd < 0) {
            need_create = true;
        } else if ((_current_start + _file_limit_duration) <= time(nullptr) || _current_fd < 0) {
            close(_current_fd);
            _records.emplace_back(Record(std::move(_current_name), _current_size));
            _total_size += _current_size;

            _current_name = "";
            _current_size = 0;
            _current_fd = -1;
            _current_start = 0;
            need_create = true;
        }

        if (!need_create) {
            return;
        }

        auto space_info = std::filesystem::space(_location);
        if (space_info.available * 5 < space_info.capacity ||
            _records.size() >= _file_limit_number) { // 最多 80% 的使用率
            delete_until_has_free_space();
        }

        std::filesystem::path p(_location);
        p.append(_name + "." + app_common::Datetime14Util::current_yyyymmddhhmiss());
        _current_name = p.string();
        _current_fd = open(_current_name.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR);
        if (_current_fd < 0) {
            throw std::runtime_error(
                fmt::format("open {} error {}", p.string(), app_common::StringUtils::str_error(errno)));
        }
        _current_start = time(nullptr);
        _current_size = 0;
    }

    void enum_exist_files() {
        std::filesystem::directory_iterator di(_location);
        for (const auto &x : di) {
            auto path = x.path().string();
            if (app_common::StringUtils::starts_with(x.path().filename().string(), _name)) {
                try {
                    auto size = std::filesystem::file_size(x.path());
                    _records.emplace_back(Record(std::move(path), size));
                    _total_size += size;
                } catch (...) {
                    std::cout << fmt::format("get size of {} failed", x.path().string()) << std::endl;
                }
            }
        }
        // std::sort(_records.begin(), _records.end()); // list 不支持随机访问
        _records.sort();

        // dump_exist_files();
    }

    void dump_exist_files() {
        for (const auto &x : _records) {
            std::cout << fmt::format("----- file: {}    size: {}", x._file, x._size) << std::endl;
        }
    }

    void delete_until_has_free_space() {
        if (_records.empty()) {
            return;
        }

        auto space_info = std::filesystem::space(_location);

        while (!_records.empty() && space_info.available * 5 < space_info.capacity) {
            auto &x = _records.front();
            try {
                std::filesystem::remove(x._file);
                _records.pop_front();
                _total_size -= x._size;
                std::cout << fmt::format("del file {} of size {}, rest size {}", x._file, x._size, _total_size)
                          << std::endl;
            } catch (const std::runtime_error &e) {
                std::cout << fmt::format("del file {} of size {} error: {}", x._file, x._size, e.what()) << std::endl;
            }
        }
    }

protected:
    const std::string   _location;
    const std::string   _name;
    const std::uint64_t _file_limit_duration;
    const std::uint64_t _file_limit_number;
    std::uint64_t       _total_size;
    std::list<Record>   _records;
    std::string         _current_name;
    std::int32_t        _current_fd;
    std::uint64_t       _current_size;
    time_t              _current_start;
};

#endif
