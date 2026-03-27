#pragma once

#include <map>
#include <mutex>
#include <string>

#include "PerfIndicators.hpp"
#include "monitor/faulthandle_api.hpp"

class EC400 {
public:
    static constexpr const char* CONFIG           = "4002001"; // 配置参数读取失败
    static constexpr const char* DATA_EMPTY       = "4002002"; // 相机数据无输入
    static constexpr const char* DATA_INVALID     = "4002003"; // 接收数据无效
    static constexpr const char* ENCODING_ERROR   = "4002004"; // 相机编码格式错误
    static constexpr const char* FREQ_ERROR       = "4002005"; // 数据接收频率异常
    static constexpr const char* SHM_FAILED       = "4002006"; // 共享内存申请失败
    static constexpr const char* DATA_RESULT_NULL = "4002008"; // 图像处理结果为空
    static constexpr const char* SPLIT_INVALID    = "4002009"; // 拼接参数无效
    static constexpr const char* PLAN_INVALID     = "4002010"; // 规划点无效
    static constexpr const char* CAM_TYPE_INVALID        = "4002011"; // 相机类型无效（类型字符串为空）
    static constexpr const char* CAM_INTRINSICS_NOT_FOUND = "4002012"; // 相机内参未找到（内参列表中无该类型）

    static EC400* Instance() {
        static EC400 inst;
        return &inst;
    }

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
            auto r = _ec_map.insert({ec, 0});
            if (!r.second) {
                return 0;
            }
            ptr = r.first;
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

    // 检查数据接收频率，低于 threshold_freq 时上报 FREQ_ERROR
    void check_freq(double data_time, double threshold_freq) {
        _freq_indicator.add(data_time, 1, 0);
        double freq = _freq_indicator.avg_per_second().first;
        if (freq > 0.0 && freq < threshold_freq) {
            ec_add(FREQ_ERROR);
        } else {
            ec_remove(FREQ_ERROR);
        }
    }

private:
    EC400() = default;

    std::mutex                              _mtx;
    std::map<std::string, std::uint32_t>    _ec_map;
    app_common::PerSecondIndicator<10, 100> _freq_indicator;
};
