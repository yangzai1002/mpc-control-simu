#include "CsvUtils.hpp"
#include "StringUtils.hpp"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include "ap_log/ap_log_interface.h"
#include "monitor/monitor_api/monitor_api.h"

namespace DetermineBehaviorPolicyFromFault {
    struct FaultBehaviorPolicy {
        std::string  code;
        std::string  desc;
        std::int32_t when_track_OBSTACLE_ignore;
        std::int32_t when_track_OBSTACLE_avoid;
        std::int32_t when_rc_MANUAL;
        std::int32_t when_rc_SecurityEnabled;
        std::int32_t when_follow_OBSTACLE_ignore;
        std::int32_t when_follow_OBSTACLE_avoid;
    };

    struct NodeFaultBehaviorPolicy {
        std::string  node_id;
        std::int32_t when_track_OBSTACLE_ignore;
        std::int32_t when_track_OBSTACLE_avoid;
        std::int32_t when_rc_MANUAL;
        std::int32_t when_rc_SecurityEnabled;
        std::int32_t when_follow_OBSTACLE_ignore;
        std::int32_t when_follow_OBSTACLE_avoid;
    };

    enum class WorkMode : std::int32_t {
        TRACK_With_Obstacle_ignored = 0,
        TRACK_With_Obstacle_avoided = 1,
        RC_With_Manual = 2,
        RC_With_SecurityEnabled = 3,
        FOLLOW_With_Obstacle_ignored = 4,
        FOLLOW_With_Obstacle_avoided = 5,
    };

    std::map<std::string, FaultBehaviorPolicy> load_policies(const std::string& path) {
        std::map<std::string, FaultBehaviorPolicy> policies;
        auto                                       records = app_common::CSVUtils::read(path, ',', 1);
        for (auto& record : records) {
            if (record.size() != 8) {
                throw std::runtime_error("Fault code csv file format error: line=" +
                                         app_common::StringUtils::join(record, ","));
            }
            if (record[0].empty()) {
                throw std::runtime_error("Fault code csv file format error: line=" +
                                         app_common::StringUtils::join(record, ","));
            }

            FaultBehaviorPolicy policy;
            policy.code = record[0];
            policy.when_track_OBSTACLE_ignore = std::strtol(record[1].c_str(), nullptr, 10);
            policy.when_track_OBSTACLE_avoid = std::strtol(record[2].c_str(), nullptr, 10);
            policy.when_rc_MANUAL = std::strtol(record[3].c_str(), nullptr, 10);
            policy.when_rc_SecurityEnabled = std::strtol(record[4].c_str(), nullptr, 10);
            policy.when_follow_OBSTACLE_ignore = std::strtol(record[5].c_str(), nullptr, 10);
            policy.when_follow_OBSTACLE_avoid = std::strtol(record[6].c_str(), nullptr, 10);
            policy.desc = record[7];
            policies[policy.code] = std::move(policy);
        }

        return policies;
    }

    std::map<std::string, NodeFaultBehaviorPolicy> load_nodefault_policies(const std::string& path) {
        std::map<std::string, NodeFaultBehaviorPolicy> policies;
        auto                                           records = app_common::CSVUtils::read(path, ',', 1);
        for (auto& record : records) {
            if (record.size() != 7) {
                throw std::runtime_error("Fault code csv file format error: line=" +
                                         app_common::StringUtils::join(record, ","));
            }
            if (record[0].empty()) {
                throw std::runtime_error("Fault code csv file format error: line=" +
                                         app_common::StringUtils::join(record, ","));
            }

            NodeFaultBehaviorPolicy policy;
            policy.node_id = record[0];
            policy.when_track_OBSTACLE_ignore = std::strtol(record[1].c_str(), nullptr, 10);
            policy.when_track_OBSTACLE_avoid = std::strtol(record[2].c_str(), nullptr, 10);
            policy.when_rc_MANUAL = std::strtol(record[3].c_str(), nullptr, 10);
            policy.when_rc_SecurityEnabled = std::strtol(record[4].c_str(), nullptr, 10);
            policy.when_follow_OBSTACLE_ignore = std::strtol(record[5].c_str(), nullptr, 10);
            policy.when_follow_OBSTACLE_avoid = std::strtol(record[6].c_str(), nullptr, 10);
            policies[policy.node_id] = std::move(policy);
        }

        return policies;
    }

    void dump_policies(const std::map<std::string, FaultBehaviorPolicy>& policies) {
        for (auto& policy : policies) {
            ApInfo("fault_handler") << fmt::format(
                "code: {}, when_track_OBSTACLE_ignore: {}, when_track_OBSTACLE_avoid: {}, when_rc_MANUAL: {}, "
                "when_rc_AVOID: {}, when_follow_OBSTACLE_ignore: {}, when_follow_OBSTACLE_avoid: {}, desc: {}",
                policy.first, policy.second.when_track_OBSTACLE_ignore, policy.second.when_track_OBSTACLE_avoid,
                policy.second.when_rc_MANUAL, policy.second.when_rc_SecurityEnabled,
                policy.second.when_follow_OBSTACLE_ignore, policy.second.when_follow_OBSTACLE_avoid,
                policy.second.desc);
        }
    }

    void dump_nodefault_policies(const std::map<std::string, NodeFaultBehaviorPolicy>& policies) {
        for (auto& policy : policies) {
            ApInfo("fault_handler") << fmt::format(
                "node: {}, when_track_OBSTACLE_ignore: {}, when_track_OBSTACLE_avoid: {}, when_rc_MANUAL: {}, "
                "when_rc_AVOID: {}, when_follow_OBSTACLE_ignore: {}, when_follow_OBSTACLE_avoid: {}, desc: {}",
                policy.first, policy.second.when_track_OBSTACLE_ignore, policy.second.when_track_OBSTACLE_avoid,
                policy.second.when_rc_MANUAL, policy.second.when_rc_SecurityEnabled,
                policy.second.when_follow_OBSTACLE_ignore, policy.second.when_follow_OBSTACLE_avoid);
        }
    }

    std::int32_t determine_behavior_policy(const std::map<std::string, FaultBehaviorPolicy>& policies,
                                           ara::adsfi::FaultInfo&                            fault_info,
                                           DetermineBehaviorPolicyFromFault::WorkMode        work_mode) {
        std::int32_t        r;
        FaultBehaviorPolicy policy{};
        try {
            policy = policies.at(fault_info.code);
            switch (work_mode) {
                case DetermineBehaviorPolicyFromFault::WorkMode::RC_With_Manual:
                    r = policy.when_rc_MANUAL;
                    break;
                case DetermineBehaviorPolicyFromFault::WorkMode::RC_With_SecurityEnabled:
                    r = policy.when_rc_SecurityEnabled;
                    break;
                case DetermineBehaviorPolicyFromFault::WorkMode::FOLLOW_With_Obstacle_ignored:
                    r = policy.when_follow_OBSTACLE_ignore;
                    break;
                case DetermineBehaviorPolicyFromFault::WorkMode::FOLLOW_With_Obstacle_avoided:
                    r = policy.when_follow_OBSTACLE_avoid;
                    break;
                case DetermineBehaviorPolicyFromFault::WorkMode::TRACK_With_Obstacle_ignored:
                    r = policy.when_track_OBSTACLE_ignore;
                    break;
                case DetermineBehaviorPolicyFromFault::WorkMode::TRACK_With_Obstacle_avoided:
                default:
                    r = policy.when_track_OBSTACLE_avoid;
                    break;
            }
            fault_info.handle = r;
            fault_info.desc = policy.desc;

            ApInfo("fault_handler") << fmt::format(
                "code: {}, when_track_OBSTACLE_ignore: {}, when_track_OBSTACLE_avoid: {}, when_rc_MANUAL: {}, "
                "when_rc_AVOID: {}, when_follow_OBSTACLE_ignore: {}, when_follow_OBSTACLE_avoid: {}, work_mode: {}, "
                "policy: {}",
                fault_info.code, policy.when_track_OBSTACLE_ignore, policy.when_track_OBSTACLE_avoid,
                policy.when_rc_MANUAL, policy.when_rc_SecurityEnabled, policy.when_follow_OBSTACLE_ignore,
                policy.when_follow_OBSTACLE_avoid, static_cast<std::int32_t>(work_mode), r);

            return r;
        } catch (const std::out_of_range& e) {
            ApError("fault_handler") << "Fault code not found: " << fault_info.code;
            r = 0;
            fault_info.desc = "未知故障 " + fault_info.code + " 请检查故障表";
        }

        return r;
    }

    std::int32_t determine_nodefault_behavior_policy(const std::map<std::string, NodeFaultBehaviorPolicy>& policies,
                                                     ara::adsfi::FaultInfo&                                fault_info,
                                                     DetermineBehaviorPolicyFromFault::WorkMode            work_mode) {
        if ("2012001" != fault_info.code) {
            return 0;
        }
        std::int32_t            r;
        NodeFaultBehaviorPolicy policy{};
        try {
            policy = policies.at(fault_info.extra_desc);
            switch (work_mode) {
                case DetermineBehaviorPolicyFromFault::WorkMode::RC_With_Manual:
                    r = policy.when_rc_MANUAL;
                    break;
                case DetermineBehaviorPolicyFromFault::WorkMode::RC_With_SecurityEnabled:
                    r = policy.when_rc_SecurityEnabled;
                    break;
                case DetermineBehaviorPolicyFromFault::WorkMode::FOLLOW_With_Obstacle_ignored:
                    r = policy.when_follow_OBSTACLE_ignore;
                    break;
                case DetermineBehaviorPolicyFromFault::WorkMode::FOLLOW_With_Obstacle_avoided:
                    r = policy.when_follow_OBSTACLE_avoid;
                    break;
                case DetermineBehaviorPolicyFromFault::WorkMode::TRACK_With_Obstacle_ignored:
                    r = policy.when_track_OBSTACLE_ignore;
                    break;
                case DetermineBehaviorPolicyFromFault::WorkMode::TRACK_With_Obstacle_avoided:
                default:
                    r = policy.when_track_OBSTACLE_avoid;
                    break;
            }

            if (r > fault_info.handle) {
                fault_info.handle = r;
            }

            ApInfo("fault_handler") << fmt::format(
                "node: {}, when_track_OBSTACLE_ignore: {}, when_track_OBSTACLE_avoid: {}, when_rc_MANUAL: {}, "
                "when_rc_AVOID: {}, when_follow_OBSTACLE_ignore: {}, when_follow_OBSTACLE_avoid: {}, work_mode: {}, "
                "policy: {}",
                fault_info.extra_desc, policy.when_track_OBSTACLE_ignore, policy.when_track_OBSTACLE_avoid,
                policy.when_rc_MANUAL, policy.when_rc_SecurityEnabled, policy.when_follow_OBSTACLE_ignore,
                policy.when_follow_OBSTACLE_avoid, static_cast<std::int32_t>(work_mode), r);

            return r;
        } catch (const std::out_of_range& e) {
            ApError("fault_handler") << "node id not found: " << fault_info.extra_desc;
            r = 0;
        }

        return r;
    }

    std::int32_t determine_behavior_policy(const std::map<std::string, FaultBehaviorPolicy>& policies,
                                           ara::adsfi::FaultData&                            fault_data,
                                           DetermineBehaviorPolicyFromFault::WorkMode        work_mode) {
        std::int32_t policy = 0;
        for (auto& fault_info : fault_data.fault_info) {
            auto new_policy = determine_behavior_policy(policies, fault_info, work_mode);
            if (new_policy > policy) {
                policy = new_policy;
            }
        }
        return policy;
    }

    std::int32_t determine_nodefault_behavior_policy(const std::map<std::string, NodeFaultBehaviorPolicy>& policies,
                                                     ara::adsfi::FaultData&                                fault_data,
                                                     DetermineBehaviorPolicyFromFault::WorkMode            work_mode) {
        std::int32_t policy = 0;
        for (auto& fault_info : fault_data.fault_info) {
            auto new_policy = determine_nodefault_behavior_policy(policies, fault_info, work_mode);
            if (new_policy > policy) {
                policy = new_policy;
            }
        }
        return policy;
    }
} // namespace DetermineBehaviorPolicyFromFault

void XDetermineBehaviorPolicyFromFault(const ara::adsfi::BusinessCommand&  business_cmd,
                                       const ara::adsfi::AppRemoteControl& remote_control,
                                       const ara::adsfi::FaultData&        fault_data_in,
                                       ara::adsfi::FaultData&              fault_data_out) {
    static std::map<std::string, DetermineBehaviorPolicyFromFault::FaultBehaviorPolicy> _behavior_policies;
    static std::map<std::string, DetermineBehaviorPolicyFromFault::NodeFaultBehaviorPolicy>
        _nodefault_behavior_policies;

    if (_behavior_policies.empty()) {
        try {
            auto        ptr = CustomStack::Instance();
            auto        project_config_path = ptr->GetProjectConfigPath();
            auto        vehicle_config_path = ptr->GetVehicleConfigPath();
            auto        PROJECT_CSV_PATH = std::filesystem::path(project_config_path) / "fault.csv";
            auto        VEHICLE_CSV_PATH = std::filesystem::path(vehicle_config_path) / "fault.csv";
            std::string path;
            if (std::filesystem::exists(VEHICLE_CSV_PATH)) {
                path = VEHICLE_CSV_PATH.string();
            } else {
                path = PROJECT_CSV_PATH.string();
            }
            ApInfo("fault_handler") << "Load fault policies from: " << path;
            _behavior_policies = DetermineBehaviorPolicyFromFault::load_policies(path);
            DetermineBehaviorPolicyFromFault::dump_policies(_behavior_policies);
        } catch (const std::exception& e) {
            ApError("fault_handler") << "Load fault policies failed: " << e.what();
            return;
        }
    }

    if (_nodefault_behavior_policies.empty()) {
        try {
            auto        ptr = CustomStack::Instance();
            auto        project_config_path = ptr->GetProjectConfigPath();
            auto        vehicle_config_path = ptr->GetVehicleConfigPath();
            auto        PROJECT_CSV_PATH = std::filesystem::path(project_config_path) / "node_fault.csv";
            auto        VEHICLE_CSV_PATH = std::filesystem::path(vehicle_config_path) / "node_fault.csv";
            std::string path;
            if (std::filesystem::exists(VEHICLE_CSV_PATH)) {
                path = VEHICLE_CSV_PATH.string();
            } else {
                path = PROJECT_CSV_PATH.string();
            }
            ApInfo("fault_handler") << "Load node fault policies from: " << path;
            _nodefault_behavior_policies = DetermineBehaviorPolicyFromFault::load_nodefault_policies(path);
            DetermineBehaviorPolicyFromFault::dump_nodefault_policies(_nodefault_behavior_policies);
        } catch (const std::exception& e) {
            ApError("fault_handler") << "Load node fault policies failed: " << e.what();
            // 这里就不能 return 了，因为前面的整体的故障处理逻辑还是要进行的
        }
    }

    fault_data_out = fault_data_in;

    DetermineBehaviorPolicyFromFault::WorkMode work_mode = DetermineBehaviorPolicyFromFault::WorkMode::RC_With_Manual;
    if (1 == business_cmd.business_mode) {             // 循迹
        if (3 == business_cmd.param.task_avoid_mode) { // 忽略障碍物模式
            work_mode = DetermineBehaviorPolicyFromFault::WorkMode::TRACK_With_Obstacle_ignored;
        } else {
            work_mode = DetermineBehaviorPolicyFromFault::WorkMode::TRACK_With_Obstacle_avoided;
        }
    } else if (3 == business_cmd.business_mode) {      // 跟随
        if (3 == business_cmd.param.task_avoid_mode) { // 忽略障碍物模式
            work_mode = DetermineBehaviorPolicyFromFault::WorkMode::FOLLOW_With_Obstacle_ignored;
        } else {
            work_mode = DetermineBehaviorPolicyFromFault::WorkMode::FOLLOW_With_Obstacle_avoided;
        }
    }

    if (1 == remote_control.remotectrl_flag) {
        if (1 == remote_control.security_assist_enabled) {
            work_mode = DetermineBehaviorPolicyFromFault::WorkMode::RC_With_SecurityEnabled;
        } else {
            work_mode = DetermineBehaviorPolicyFromFault::WorkMode::RC_With_Manual;
        }
    }

    ApInfo("fault_handler") << "business_mode: " << business_cmd.business_mode
                            << ", task_avoid_mode: " << business_cmd.param.task_avoid_mode
                            << ", remote_control.remotectrl_flag: " << remote_control.remotectrl_flag
                            << ", remote_control.security_assist_enabled: " << remote_control.security_assist_enabled
                            << ", work_mode: " << static_cast<std::int32_t>(work_mode);

    fault_data_out.fault_level =
        DetermineBehaviorPolicyFromFault::determine_behavior_policy(_behavior_policies, fault_data_out, work_mode);

    auto r = DetermineBehaviorPolicyFromFault::determine_nodefault_behavior_policy(_nodefault_behavior_policies,
                                                                                   fault_data_out, work_mode);
    if (r > fault_data_out.fault_level) {
        fault_data_out.fault_level = r;
    }
    std::sort(fault_data_out.fault_info.begin(), fault_data_out.fault_info.end(),
              [](const ara::adsfi::FaultInfo& a, const ara::adsfi::FaultInfo& b) { return a.code < b.code; });
}
