/* *
 * Class: BodyCommandServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_BODYCOMMANDSERVICEINTERFACESERVER
#define ARA_ADSFI_BODYCOMMANDSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_vehicleactcontrol.h"





namespace ara {
namespace adsfi {

using EventVehicleActControlDataType = ara::adsfi::VehicleActControl;
using SendEventVehicleActControlType = std::shared_ptr<EventVehicleActControlDataType>;


class BodyCommandServiceInterfaceServer final {
public:
    BodyCommandServiceInterfaceServer() = delete;

    explicit BodyCommandServiceInterfaceServer(const uint32_t id);

    virtual ~BodyCommandServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventVehicleActControlData(const SendEventVehicleActControlType& data);
    void ClearEventVehicleActControlContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif