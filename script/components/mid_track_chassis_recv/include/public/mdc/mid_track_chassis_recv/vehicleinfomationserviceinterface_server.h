/* *
 * Class: VehicleInfomationServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_VEHICLEINFOMATIONSERVICEINTERFACESERVER
#define ARA_ADSFI_VEHICLEINFOMATIONSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_vehicleinformation.h"





namespace ara {
namespace adsfi {

using EventVehicleInfoDataType = ara::adsfi::VehicleInformation;
using SendEventVehicleInfoType = std::shared_ptr<EventVehicleInfoDataType>;


class VehicleInfomationServiceInterfaceServer final {
public:
    VehicleInfomationServiceInterfaceServer() = delete;

    explicit VehicleInfomationServiceInterfaceServer(const uint32_t id);

    virtual ~VehicleInfomationServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventVehicleInfoData(const SendEventVehicleInfoType& data);
    void ClearEventVehicleInfoContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif