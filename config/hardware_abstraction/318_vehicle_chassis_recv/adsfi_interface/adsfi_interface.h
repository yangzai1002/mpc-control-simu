
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <memory>
#include <string>

#include "G318CanbusReceive.hpp"
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

private:
    std::shared_ptr<G318CanbusRecvData> _driver_ptr;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
