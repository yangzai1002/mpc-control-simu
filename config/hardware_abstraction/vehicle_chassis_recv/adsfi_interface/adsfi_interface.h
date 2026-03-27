
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <memory>
#include <string>

#include "XMidTrackReceiveByZMQ.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "log/log.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    void Init();

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleInformation> &msg);

public:
    BASE_TEMPLATE_FUNCION
};
#endif
