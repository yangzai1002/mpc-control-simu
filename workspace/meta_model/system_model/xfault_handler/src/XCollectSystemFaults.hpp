#include <sys/time.h>

#include "StringUtils.hpp"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "common/monitor/monitor_api/monitor_api.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include "log/avos_node.h"

static std::string get_node_name(const std::string& full_name) {
    std::string node_name;
    auto        node_info = app_common::StringUtils::tokenize<std::vector>((char*)full_name.c_str(), ":");
    if (0 == node_info.size() || node_info.size() > 2) {
        ApError("fault_handler") << "Invalid module name: " << full_name << ", skip it!";
    } else if (1 == node_info.size()) {
        node_name = std::move(node_info[0]);
    } else if (2 == node_info.size()) {
        node_name = std::move(node_info[1]);
    }

    return node_name;
}

void XCollectSystemFaults(ara::adsfi::FaultData& fault_data) {
    static std::int64_t _seq{};

    timeval tv;
    gettimeofday(&tv, nullptr);
    auto ts = tv.tv_sec + tv.tv_usec / 1000000.0;

    std::vector<avos::common::ModuleHeartbeatInfo> all_modules;
    avos::common::MonitorApi::Instance()->GetAllRegesterdModule(all_modules);

    if (_seq % 10 == 0) {
        ApInfo("fault_handler") << "all module size:" << all_modules.size();
        for (auto module : all_modules) {
            ApInfo("fault_handler") << fmt::format("get all module {}, {:20.8f}", module.GetModuleName(),
                                                   module.last_hear_beat_time);
        }
    }

    std::vector<avos::common::ModuleHeartbeatInfo> modules;
    avos::common::MonitorApi::Instance()->GetExceptionModule(modules);
    for (auto module : modules) {
        ApInfo("fault_handler") << fmt::format(
            "get error module name {}, heart_beat_time: {:20.8f}, systime: {:20.8f}, diff: {:20.8f}",
            module.GetModuleName(), module.last_hear_beat_time, ts, ts - module.last_hear_beat_time);
        ara::adsfi::FaultInfo fault_info{};
        fault_info.code = "2012001";
        fault_info.extra_desc = get_node_name(module.GetModuleName());
        fault_info.timestamp = ts;
        fault_info.from = avos::common::AvosNode::GetNodeName();

        fault_data.fault_info.emplace_back(std::move(fault_info));
    }

    std::vector<avos::common::SHMFaultInfo> infos;
    avos::common::MonitorApi::Instance()->GetErrorItem(infos);
    for (auto info : infos) {
        ara::adsfi::FaultInfo fault_info{};

        std::cout << "get error info " << info.module_name << "|" << info.message << std::endl;
        fault_info.code = std::to_string(info.error_code);
        fault_info.timestamp = info.fault_time / 1000.0;
        fault_info.extra_desc = info.message;
        fault_info.from = get_node_name(std::string(info.module_name));
        fault_data.fault_info.emplace_back(std::move(fault_info));
    }

    fault_data.header.time.sec = tv.tv_sec;
    fault_data.header.time.nsec = tv.tv_usec * 1000;
    fault_data.header.seq = ++_seq;
}
