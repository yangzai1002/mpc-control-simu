#include "adsfi_interface.h"
#include <fmt/format.h>
#include "ap_log/ap_log_interface.h"
#include "config_loader/custom_stack.h"
#include "monitor/faulthandle_api.hpp"
#include "../src/EC204.hpp"

AdsfiInterface::AdsfiInterface() {
    auto        ptr = CustomStack::Instance();
    std::string full_name;

    full_name = "system/recorder/status/frequency/vehicle_information_min_hz";
    if (!ptr->GetProjectConfigValue(full_name, _freq_threshold_vehicle_info)) {
        FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
        throw std::runtime_error(fmt::format("read project config {} failed", full_name));
    }
    full_name = "system/recorder/status/frequency/fault_data_min_hz";
    if (!ptr->GetProjectConfigValue(full_name, _freq_threshold_fault_data)) {
        FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
        throw std::runtime_error(fmt::format("read project config {} failed", full_name));
    }
    full_name = "system/recorder/status/frequency/location_min_hz";
    if (!ptr->GetProjectConfigValue(full_name, _freq_threshold_location)) {
        FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
        throw std::runtime_error(fmt::format("read project config {} failed", full_name));
    }
    full_name = "system/recorder/status/frequency/image_data_list_min_hz";
    if (!ptr->GetProjectConfigValue(full_name, _freq_threshold_image_data_list)) {
        FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
        throw std::runtime_error(fmt::format("read project config {} failed", full_name));
    }
}

AdsfiInterface::~AdsfiInterface() {
}

void AdsfiInterface::Init() {
    SetScheduleInfo("timmer", 100);
    // ApLogInterface::Instance()->Init("XVehicleStatusRecorder");
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("status_recorder") << "msg is null";
        return;
    }

    {
        std::lock_guard lg(_recorder_h265_mtx);
        if (__glibc_unlikely(nullptr == _recorder_h265)) {
            ApWarn("status_recorder") << "_recorder_h265 is null";
        } else {
            _recorder_h265->update_vehicle_info(*msg);
        }
    }

    // 频率检测（每帧都检测，不受降频策略影响）
    EC204::Instance().check_freq(EC204::_ERRORCODE_FREQUENCY_ERROR_VEHICLE_INFORMATION,
        std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(),
        _freq_threshold_vehicle_info);

    std::lock_guard lg(_recorder_sqlite3_mtx);
    if (__glibc_unlikely(nullptr == _recorder_sqlite3)) {
        ApError("status_recorder") << "_recorder_sqlite3 is null";
        return;
    }
    if (!elapsed_and_reset(_last_record_vehicle_information)) {
        return;
    }

    _recorder_sqlite3->record_vehicle_information(*msg);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("status_recorder") << "msg is null";
        return;
    }

    {
        std::lock_guard lg(_recorder_h265_mtx);
        if (__glibc_unlikely(nullptr == _recorder_h265)) {
            ApWarn("status_recorder") << "_recorder_h265 is null";
        } else {
            _recorder_h265->update_location(*msg);
        }
    }

    // 频率检测（每帧都检测，不受降频策略影响）
    EC204::Instance().check_freq(EC204::_ERRORCODE_FREQUENCY_ERROR_LOCATION,
        std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(),
        _freq_threshold_location);

    std::lock_guard lg(_recorder_sqlite3_mtx);
    if (__glibc_unlikely(nullptr == _recorder_sqlite3)) {
        ApError("status_recorder") << "_recorder_sqlite3 is null";
        return;
    }
    if (!elapsed_and_reset(_last_record_location)) {
        return;
    }

    _recorder_sqlite3->record_location(*msg);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::FaultData> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("status_recorder") << "msg is null";
        return;
    }

    {
        std::lock_guard lg(_recorder_h265_mtx);
        if (__glibc_unlikely(nullptr == _recorder_h265)) {
            ApWarn("status_recorder") << "_recorder_h265 is null";
        } else {
            _recorder_h265->update_fault_data(*msg);
        }
    }

    // 频率检测（每帧都检测，不受变化检测策略影响）
    EC204::Instance().check_freq(EC204::_ERRORCODE_FREQUENCY_ERROR_FAULT_DATA,
        std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(),
        _freq_threshold_fault_data);

    std::lock_guard lg(_recorder_sqlite3_mtx);
    if (__glibc_unlikely(nullptr == _recorder_sqlite3)) {
        ApError("status_recorder") << "_recorder_sqlite3 is null";
        return;
    }
    if (_last_fault_data == *msg) {
        return;
    }
    _last_fault_data = *msg;

    _recorder_sqlite3->record_fault_data(*msg);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafGnssInfo> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("status_recorder") << "msg is null";
        return;
    }

    std::lock_guard lg(_recorder_sqlite3_mtx);
    if (__glibc_unlikely(nullptr == _recorder_sqlite3)) {
        ApError("status_recorder") << "_recorder_sqlite3 is null";
        return;
    }
    if (!elapsed_and_reset(_last_record_gnss)) {
        return;
    }

    _recorder_sqlite3->record_gnss_info(*msg);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleActControl> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("status_recorder") << "msg is null";
        return;
    }

    {
        std::lock_guard lg(_recorder_h265_mtx);
        if (__glibc_unlikely(nullptr == _recorder_h265)) {
            ApWarn("status_recorder") << "_recorder_h265 is null";
        } else {
            _recorder_h265->update_vehicle_control(*msg);
        }
    }

    std::lock_guard lg(_recorder_sqlite3_mtx);
    if (__glibc_unlikely(nullptr == _recorder_sqlite3)) {
        ApError("status_recorder") << "_recorder_sqlite3 is null";
        return;
    }
    if (!elapsed_and_reset(_last_record_vehicle_control)) {
        return;
    }

    _recorder_sqlite3->record_vehicle_act_control(*msg);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("status_recorder") << "msg is null";
        return;
    }

    std::lock_guard lg(_recorder_sqlite3_mtx);
    if (__glibc_unlikely(nullptr == _recorder_sqlite3)) {
        ApError("status_recorder") << "_recorder_sqlite3 is null";
        return;
    }
    if (!elapsed_and_reset(_last_record_business_command)) {
        return;
    }

    _recorder_sqlite3->record_business_command(*msg);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("status_recorder") << "msg is null";
        return;
    }

    std::lock_guard lg(_recorder_sqlite3_mtx);
    if (__glibc_unlikely(nullptr == _recorder_sqlite3)) {
        ApError("status_recorder") << "_recorder_sqlite3 is null";
        return;
    }
    if (!elapsed_and_reset(_last_record_trajectory)) {
        return;
    }

    _recorder_sqlite3->record_ego_trajectory(*msg);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PlanningStatus> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("status_recorder") << "msg is null";
        return;
    }

    std::lock_guard lg(_recorder_sqlite3_mtx);
    if (__glibc_unlikely(nullptr == _recorder_sqlite3)) {
        ApError("status_recorder") << "_recorder_sqlite3 is null";
        return;
    }
    if (!elapsed_and_reset(_last_record_planning_status)) {
        return;
    }

    _recorder_sqlite3->record_planning_status(*msg);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppRemoteControl> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("status_recorder") << "msg is null";
        return;
    }

    std::lock_guard lg(_recorder_sqlite3_mtx);
    if (__glibc_unlikely(nullptr == _recorder_sqlite3)) {
        ApError("status_recorder") << "_recorder_sqlite3 is null";
        return;
    }
    if (!elapsed_and_reset(_last_record_remote_control)) {
        return;
    }

    _recorder_sqlite3->record_remote_control(*msg);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageDataList> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("status_recorder") << "msg is null";
        return;
    }

    // 频率检测（每帧都检测）
    EC204::Instance().check_freq(EC204::_ERRORCODE_FREQUENCY_ERROR_IMAGE_DATA_LIST,
        std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(),
        _freq_threshold_image_data_list);

    std::lock_guard lg(_recorder_h265_mtx);
    if (__glibc_unlikely(nullptr == _recorder_h265)) {
        ApError("status_recorder") << "_recorder_h265 is null";
        return;
    }

    _recorder_h265->add(msg);
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::FaultData> &msg) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard lg(_recorder_h265_mtx);
        if (nullptr == _recorder_h265) {
            try {
                _recorder_h265 = std::make_shared<RecorderH265>();
            } catch (const std::exception &e) {
                ApError("status_recorder") << e.what();
            }
        }
    }

    {
        std::lock_guard lg(_recorder_sqlite3_mtx);
        if (nullptr == _recorder_sqlite3) {
            try {
                _recorder_sqlite3 = std::make_shared<vehicle_recorder::RecorderSqlite3>();
            } catch (const std::exception &e) {
                ApError("status_recorder") << e.what();
            }
        }
    }

    return -1;
}