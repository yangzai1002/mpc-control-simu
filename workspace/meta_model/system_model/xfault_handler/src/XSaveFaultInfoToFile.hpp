#include "DatetimeUtil.hpp"
#include "ThreadedRotatingFaultInfoSaver.hpp"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_faultdata.h"

void XSaveFaultInfoToFile(const ara::adsfi::FaultData& fault_data) {
    static std::shared_ptr<FaultInfoSaver> _receiver = nullptr;
    static ara::adsfi::FaultData           _last_fault_data{};
    static std::int64_t                    _last_all_ms = 0;

    if (!_receiver) {
        try {
            _receiver = std::make_shared<ThreadedRotatingFaultInfoSaver>();
        } catch (const std::exception& e) {
            ApInfo("fault_handler") << "ERROR: " << e.what();
            return;
        }
    }

    try {
        ara::adsfi::FaultData this_fault_data = fault_data;
        std::sort(this_fault_data.fault_info.begin(), this_fault_data.fault_info.end(),
                  [](const ara::adsfi::FaultInfo& a, const ara::adsfi::FaultInfo& b) { return a.code < b.code; });

        std::vector<ara::adsfi::FaultInfo> add_s;
        std::vector<ara::adsfi::FaultInfo> del_s;
        std::set_difference(
            this_fault_data.fault_info.begin(), this_fault_data.fault_info.end(), _last_fault_data.fault_info.begin(),
            _last_fault_data.fault_info.end(), std::back_inserter(add_s),
            [](const ara::adsfi::FaultInfo& a, const ara::adsfi::FaultInfo& b) { return a.code < b.code; });
        std::set_difference(
            _last_fault_data.fault_info.begin(), _last_fault_data.fault_info.end(), this_fault_data.fault_info.begin(),
            this_fault_data.fault_info.end(), std::back_inserter(del_s),
            [](const ara::adsfi::FaultInfo& a, const ara::adsfi::FaultInfo& b) { return a.code < b.code; });

        auto now = app_common::DatetimeUtil::current_millisecond();
        if ((_last_all_ms + 2000) < now) { // 每2秒输出一次所有故障信息，其余时间只输出新增和删除的故障信息
            auto all = fmt::format("ALL@{}", now);
            std::for_each(this_fault_data.fault_info.begin(), this_fault_data.fault_info.end(),
                          [&all](const ara::adsfi::FaultInfo& fault_info) {
                              auto s = fmt::format(", code {}#{}, timestamp {:20.6f}", fault_info.code,
                                                   fault_info.extra_desc, fault_info.timestamp);
                              all.append(std::move(s));
                          });
            all.append("\n");
            _receiver->push(all);
            _last_all_ms = now;
        } else if (!add_s.empty() || !del_s.empty()) { // 故障信息有新增或删除时，输出当前故障快照
            auto all = fmt::format("SNP@{}", now);
            std::for_each(this_fault_data.fault_info.begin(), this_fault_data.fault_info.end(),
                          [&all](const ara::adsfi::FaultInfo& fault_info) {
                              auto s = fmt::format(", code {}#{}, timestamp {:20.6f}", fault_info.code,
                                                   fault_info.extra_desc, fault_info.timestamp);
                              all.append(std::move(s));
                          });
            all.append("\n");
            _receiver->push(all);
        }

        _last_fault_data = std::move(this_fault_data);

        for (auto& fault_info : add_s) {
            auto l = fmt::format("NEW@{}, code {}#{}, timestamp {:20.6f}\n", now, fault_info.code,
                                 fault_info.extra_desc, fault_info.timestamp);
            _receiver->push(l);
        }
        for (auto& fault_info : del_s) {
            auto l = fmt::format("DEL@{}, code {}#{}, timestamp {:20.6f}\n", now, fault_info.code,
                                 fault_info.extra_desc, fault_info.timestamp);
            _receiver->push(l);
        }
    } catch (const std::exception& e) {
        ApError("fault_handler") << "ERROR: " << e.what();
    }
}
