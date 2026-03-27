/* *
 * Class: VehicleInfomationServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_VEHICLEINFOMATIONSERVICEINTERFACECLIENT
#define ARA_ADSFI_VEHICLEINFOMATIONSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_vehicleinformation.h"





namespace ara {
namespace adsfi {

using EventVehicleInfoHandlerType = std::function<void (ara::adsfi::VehicleInformation)>;
using EventVehicleInfoDataType = ara::adsfi::VehicleInformation;
using RecvEventVehicleInfoType = std::shared_ptr<EventVehicleInfoDataType>;
class VehicleInfomationServiceInterfaceClient final {
public:
    VehicleInfomationServiceInterfaceClient() = delete;
    explicit VehicleInfomationServiceInterfaceClient(const uint32_t instanceId);
    virtual ~VehicleInfomationServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventVehicleInfoNotifyHandler(const EventVehicleInfoHandlerType handler);
    void EventVehicleInfoNotify();
    void EventVehicleInfoContainerClear();
    bool EventVehicleInfoContainerEmpty();
    RecvEventVehicleInfoType GetEventVehicleInfoOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventVehicleInfoType GetEventVehicleInfoOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventVehicleInfoType> GetEventVehicleInfoNdata(const size_t n);

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