#pragma once
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "monitor/faulthandle_api.hpp"

constexpr int64_t RECOVER_CONTINUOUS_MS = 1000; // 1 s 防抖恢复

class FaultManager {
public:
    FaultManager() = default;
    ~FaultManager() {
        clear();
    }

    bool add(const std::string& ec, const std::string& msg = "") {
        {
            std::lock_guard<std::mutex> lock(_mtx);
            if (_active.count(ec)) {       // 已存在
                _pendingRecover.erase(ec); // 取消待恢复计时
                return false;
            }
            if (_inflight.count(ec))
                return false;
            _inflight.insert(ec);
        }

        bool setOk = false;
        try {
            auto api = FaultHandle::FaultApi::Instance();
            if (api) {
                api->SetFaultCode(ec, msg);
                setOk = true;
            }
        } catch (...) {
            setOk = false;
        }

        {
            std::lock_guard<std::mutex> lock(_mtx);
            _inflight.erase(ec);
            if (setOk) {
                _active[ec] = std::chrono::steady_clock::now();
                _pendingRecover.erase(ec);
                return true;
            }
            return false;
        }
    }

    // 恢复故障（带 1 s 防抖）
    bool remove(const std::string& ec) {
        {
            std::lock_guard<std::mutex> lock(_mtx);
            auto                        it = _active.find(ec);
            if (it == _active.end())
                return false;
            if (_inflight.count(ec))
                return false;

            auto pit = _pendingRecover.find(ec);
            if (pit == _pendingRecover.end()) {
                // 首次满足恢复条件，开始计时
                _pendingRecover[ec] = std::chrono::steady_clock::now();
                return false;
            }
            auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - pit->second)
                    .count();
            if (elapsed < RECOVER_CONTINUOUS_MS)
                return false;
            _inflight.insert(ec); // 占坑
        }

        bool resetOk = false;
        try {
            auto api = FaultHandle::FaultApi::Instance();
            if (api) {
                api->ResetFaultCode(ec);
                resetOk = true;
            }
        } catch (...) {
            resetOk = false;
        }

        {
            std::lock_guard<std::mutex> lock(_mtx);
            _inflight.erase(ec);
            if (resetOk) {
                _active.erase(ec);
                _pendingRecover.erase(ec);
                return true;
            }
            // reset 失败，下次重新计时
            _pendingRecover.erase(ec);
            return false;
        }
    }

    void clear() {
        std::vector<std::string> toReset;
        {
            std::lock_guard<std::mutex> lock(_mtx);
            for (const auto& p : _active)
                toReset.push_back(p.first);
            _active.clear();
            _pendingRecover.clear();
            _inflight.clear();
        }
        for (const auto& code : toReset) {
            try {
                auto api = FaultHandle::FaultApi::Instance();
                if (api)
                    api->ResetFaultCode(code);
            } catch (...) {
            }
        }
    }

    // 查询故障是否存在
    bool contains(const std::string& ec) const {
        std::lock_guard<std::mutex> lock(_mtx);
        return _active.find(ec) != _active.end();
    }

private:
    mutable std::mutex                                                     _mtx;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> _active;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> _pendingRecover;
    std::unordered_set<std::string>                                        _inflight;
};