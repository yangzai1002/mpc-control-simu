#pragma once

#include <mutex>
#include <string>

#include "PerfIndicators.hpp"
#include "faulthandle_api.hpp"

static const constexpr char* _ERRORCODE_CONFIG = "4072001";          // 配置参数读取失败
static const constexpr char* _ERRORCODE_NO_DATA = "4072002";         // 无数据输入
static const constexpr char* _ERRORCODE_FREQ = "4072003";            // 数据频率异常
static const constexpr char* _ERRORCODE_FORMAT = "4072004";          // 数据格式错误
static const constexpr char* _ERRORCODE_SEND = "4072005";            // 数据发送失败
static const constexpr char* _ERRORCODE_SRTP_INIT = "4072006";       // SRTP初始化失败
static const constexpr char* _ERRORCODE_ADDRESS_RESOLVE = "4072007"; // 地址解析失败
static const constexpr char* _ERRORCODE_ADDRESS_BIND = "4072008";    // 地址绑定失败

class Ec407 {
public:
    Ec407() = default;

    std::uint32_t ec_add(const std::string& ec) {
        std::lock_guard<std::mutex> lg(_mtx);
        auto                        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            auto r = _ec_map.insert({ec, 0});
            if (!r.second) {
                return 0;
            }
            ptr = r.first;
        }

        ++ptr->second;
        if (ptr->second % 20 == 1) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec);
        }
        return ptr->second;
    }

    std::uint32_t ec_add(const std::string& ec, const std::string& ec_detail) {
        std::lock_guard<std::mutex> lg(_mtx);
        auto                        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            _ec_map[ec] = 0;
        }
        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            return 0;
        }

        ++ptr->second;
        if (ptr->second % 20 == 1) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec, ec_detail);
        }
        return ptr->second;
    }

    void ec_remove(const std::string& ec) {
        std::lock_guard<std::mutex> lg(_mtx);
        if (_ec_map.find(ec) != _ec_map.end()) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec);
            _ec_map.erase(ec);
        }
    }

    void ec_clear() {
        std::lock_guard<std::mutex> lg(_mtx);
        for (const auto& ec : _ec_map) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec.first);
        }
        _ec_map.clear();
    }

    void check_freq(double data_time, double threshold_freq) {
        double avg = 0;
        {
            std::lock_guard<std::mutex> lg(_mtx);
            _per_second_indicator.add(data_time * 1000, 1, 1);
            avg = _per_second_indicator.avg_per_second().first;
        }

        if (avg < threshold_freq) {
            ec_add(_ERRORCODE_FREQ);
        } else {
            ec_remove(_ERRORCODE_FREQ);
        }
    }

private:
    std::mutex                              _mtx;
    std::map<std::string, std::uint32_t>    _ec_map;
    app_common::PerSecondIndicator<10, 100> _per_second_indicator;
};