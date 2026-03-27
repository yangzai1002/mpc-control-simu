#ifndef EXOSIP2_GB28181_CLIENT_PSFILERTPSOURCE_HPP
#define EXOSIP2_GB28181_CLIENT_PSFILERTPSOURCE_HPP

#include <atomic>
#include <cstdio>
#include <string>
#include <thread>

#include "FileH264StreamReader.hpp"
#include "MediaMan.hpp"

class FileH264MediaMan : public MediaMan {
public:
    FileH264MediaMan(std::shared_ptr<FileH264StreamReader> reader, bool rewind = false)
        : MediaMan(1), _rewind(rewind), _file_h264_stopped(false), _reader(std::move(reader)) {
    }

    // clang-format on

    ~FileH264MediaMan() override {
        _file_h264_stopped = true;
        if (_file_h264_thread.joinable()) {
            _file_h264_thread.join();
        }
    }

public:
    void stream_start() override {
        if (0 == receiver_count(TrackId::Track0)) {
            return;
        }
        _file_h264_stopped = false;
        _file_h264_thread = std::thread([this]() { loop(); });
        std::cout << fmt::format("----file h264 media man started") << std::endl;
    }

    void stream_stop() override {
        _file_h264_stopped = true;
        if (_file_h264_thread.joinable()) {
            _file_h264_thread.join();
        }
        std::cout << fmt::format("----file h264 media man stopped") << std::endl;
    }

private:
    void loop() {
        while (!_file_h264_stopped) {
            if (0 == receiver_count(TrackId::Track0)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                continue;
            }

            auto n = _reader->next_nalu();
            if (nullptr == n.first) {
                if (!_rewind) {
                    std::cout << fmt::format("...end") << std::endl;
                    break;
                } else {
                    std::cout << fmt::format("...rewind") << std::endl;
                    _reader->rewind();
                    _reader->reset();
                    continue;
                }
            }

            {
                std::lock_guard<std::mutex> g(_track0->_senders_mutex);
                for (auto& x : _track0->_rtp_senders) {
                    x.second->send(n.first, n.second);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    }

private:
    bool                                  _rewind;
    std::atomic_bool                      _file_h264_stopped;
    std::thread                           _file_h264_thread;
    std::shared_ptr<FileH264StreamReader> _reader;
};

#endif // EXOSIP2_GB28181_CLIENT_PSFILERTPSOURCE_HPP
