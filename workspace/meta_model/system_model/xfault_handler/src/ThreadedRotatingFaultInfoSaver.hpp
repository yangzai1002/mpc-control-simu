#ifndef _H264_THREADED_ROTATING_FILE_RECEIVER_HPP
#define _H264_THREADED_ROTATING_FILE_RECEIVER_HPP
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <thread>

#include "DatetimeUtil.hpp"
#include "RotatingFaultInfoSaver.hpp"
#include "SafeQueue.hpp"

class ThreadedRotatingFaultInfoSaver : public RotatingFaultInfoSaver {
public:
    ThreadedRotatingFaultInfoSaver() : RotatingFaultInfoSaver(), _stopped(false) {
        _thread_write = std::thread([this]() { loop(); });
    }

    ~ThreadedRotatingFaultInfoSaver() override {
        _stopped = true;
        if (_thread_write.joinable()) {
            _thread_write.join();
        }

        if (_current_fd >= 0) {
            close(_current_fd);
            _current_fd = -1;
        }
    }

    void push(const std::string& fault_record) override {
        _fault_records.push(fault_record);
    }

private:
    void loop() {
        pthread_setname_np(pthread_self(), "record_writer");
        std::string fault_record;
        while (!_stopped) {
            if (!_fault_records.timeout_move_pop(fault_record, 100000)) {
                continue;
            }

            try {
                RotatingFaultInfoSaver::push(fault_record);
            } catch (const std::exception& e) {
            }
        }
    }

private:
    std::atomic_bool                   _stopped;
    std::thread                        _thread_write;
    app_common::SafeQueue<std::string> _fault_records;
};

#endif
