
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "DatetimeUtil.hpp"
#include "StringUtils.hpp"
#include "SystemMonitor.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "config_loader/custom_stack.h"
#include "ap_log/ap_log_interface.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() {
    }

    ~AdsfiInterface() {
    }

    void Init();

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::FaultData> &msg);

private:
    std::shared_ptr<SystemMonitor> _system_monitor;

private:
    ara::adsfi::FaultData _out_fault_data;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
