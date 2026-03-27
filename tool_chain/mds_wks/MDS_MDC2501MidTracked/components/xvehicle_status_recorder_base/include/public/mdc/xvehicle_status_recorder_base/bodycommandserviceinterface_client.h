/* *
 * Class: BodyCommandServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_BODYCOMMANDSERVICEINTERFACECLIENT
#define ARA_ADSFI_BODYCOMMANDSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_vehicleactcontrol.h"





namespace ara {
namespace adsfi {

using EventVehicleActControlHandlerType = std::function<void (ara::adsfi::VehicleActControl)>;
using EventVehicleActControlDataType = ara::adsfi::VehicleActControl;
using RecvEventVehicleActControlType = std::shared_ptr<EventVehicleActControlDataType>;
class BodyCommandServiceInterfaceClient final {
public:
    BodyCommandServiceInterfaceClient() = delete;
    explicit BodyCommandServiceInterfaceClient(const uint32_t instanceId);
    virtual ~BodyCommandServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventVehicleActControlNotifyHandler(const EventVehicleActControlHandlerType handler);
    void EventVehicleActControlNotify();
    void EventVehicleActControlContainerClear();
    bool EventVehicleActControlContainerEmpty();
    RecvEventVehicleActControlType GetEventVehicleActControlOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventVehicleActControlType GetEventVehicleActControlOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventVehicleActControlType> GetEventVehicleActControlNdata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif