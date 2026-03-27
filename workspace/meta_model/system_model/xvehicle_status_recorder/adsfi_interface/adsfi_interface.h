
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "../src/RecorderH265.hpp"
#include "../src/RecorderSqlite3.hpp"
#include "DatetimeUtil.hpp"
#include "StringUtils.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msghafgnssinfo.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "config_loader/custom_stack.h"
#include "ap_log/ap_log_interface.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    void Init();

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::FaultData> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafGnssInfo> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleActControl> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PlanningStatus> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppRemoteControl> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageDataList> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::FaultData> &msg);

private:
    // configuration

    // runtime state

    ara::adsfi::FaultData _out_fault_data;

    std::mutex                                         _recorder_h265_mtx;
    std::shared_ptr<RecorderH265>                      _recorder_h265;
    std::mutex                                         _recorder_sqlite3_mtx;
    std::shared_ptr<vehicle_recorder::RecorderSqlite3> _recorder_sqlite3;

    // ── FR-05: 降频至 ≤ 10 Hz（100 ms 丢弃策略），在 _recorder_sqlite3_mtx 持有期间调用 ──

    static constexpr std::chrono::milliseconds _RECORD_INTERVAL{100};

    bool elapsed_and_reset(std::chrono::steady_clock::time_point& last) {
        const auto now = std::chrono::steady_clock::now();
        if (now - last < _RECORD_INTERVAL) return false;
        last = now;
        return true;
    }

    std::chrono::steady_clock::time_point _last_record_vehicle_information{};
    std::chrono::steady_clock::time_point _last_record_location{};
    std::chrono::steady_clock::time_point _last_record_gnss{};
    std::chrono::steady_clock::time_point _last_record_vehicle_control{};
    std::chrono::steady_clock::time_point _last_record_business_command{};
    std::chrono::steady_clock::time_point _last_record_trajectory{};
    std::chrono::steady_clock::time_point _last_record_planning_status{};
    std::chrono::steady_clock::time_point _last_record_remote_control{};

    // ── FR-06: FaultData 变化检测，内容相同则跳过写入 ──
    ara::adsfi::FaultData _last_fault_data{};

    // ── 频率检测阈值（Hz；0.0 = 禁用），在 Init() 中从 CustomStack 读取 ──
    double _freq_threshold_vehicle_info{0.0};
    double _freq_threshold_fault_data{0.0};
    double _freq_threshold_location{0.0};
    double _freq_threshold_image_data_list{0.0};

public:
    BASE_TEMPLATE_FUNCION
};
#endif
