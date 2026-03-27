
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "AionCanbusReceive.h"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "config_loader/custom_stack.h"
#include "log/log.h"
#include "monitor/faulthandle_api.hpp"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    void Init();

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleInformation> &msg);

private:
    std::shared_ptr<AionCanbusRecvData> _driver_ptr;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
