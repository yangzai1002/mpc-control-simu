#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "StringUtils.hpp"
#include "ap_log/ap_log_interface.h"

// 从项目配置加载“不可通行故障码列表”到指定集合。
// 说明：
// - 本函数不做兜底故障码；缺少配置时将保持集合为空，从而不触发道路阻断判定。
// - 建议只在初始化阶段调用一次，避免运行期频繁读取配置。
template <typename CustomStackPtr>
inline void load_unpassable_fault_codes_from_config(const CustomStackPtr& ptr, const std::string& prefix,
                                                    std::unordered_set<std::string>& unpassable_fault_codes_set) {
    unpassable_fault_codes_set.clear();
    if (!ptr) {
        ApError("zmq_notification") << "CustomStack 实例为空，无法加载不可通行故障码列表，将不进行道路阻断判断";
        return;
    }

    std::vector<std::string> unpassable_fault_codes;
    const std::string        full_path = prefix + "hmi/zmq/unpassable_fault_codes";

    if (!ptr->GetProjectConfigArray(full_path, unpassable_fault_codes)) {
        ApError("zmq_notification") << "未配置 " << full_path << "，将不进行道路阻断判断";
        return;
    }

    if (!unpassable_fault_codes.empty()) {
        for (auto& code : unpassable_fault_codes) {
            app_common::StringUtils::trim(code);
            if (!code.empty()) {
                unpassable_fault_codes_set.insert(code);
            }
        }

        if (!unpassable_fault_codes_set.empty()) {
            ApError("zmq_notification") << "加载不可通行故障码列表成功：" << full_path << "，数量=" << unpassable_fault_codes_set.size();
            return;
        }
    }
    ApError("zmq_notification") << "不可通行故障码列表为空（含去空格后为空）：" << full_path << "，将不进行道路阻断判断";
}
