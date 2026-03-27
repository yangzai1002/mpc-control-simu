
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
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_businesscommand.h"
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

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppRemoteControl> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::FaultData> &msg);

private:
    std::mutex                   _mtx;
    ara::adsfi::BusinessCommand  _in_business_command{};
    ara::adsfi::AppRemoteControl _in_remote_control{};
    ara::adsfi::FaultData        _out_fault_data;

    ara::adsfi::FaultData _tmp_fault_data;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
