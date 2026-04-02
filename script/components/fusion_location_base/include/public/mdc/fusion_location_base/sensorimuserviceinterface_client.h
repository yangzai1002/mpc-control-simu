/* *
 * Class: SensorImuServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORIMUSERVICEINTERFACECLIENT
#define ARA_ADSFI_SENSORIMUSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msghafimu.h"





namespace ara {
namespace adsfi {

using EventSensorImuHandlerType = std::function<void (ara::adsfi::MsgHafIMU)>;
using EventSensorImuDataType = ara::adsfi::MsgHafIMU;
using RecvEventSensorImuType = std::shared_ptr<EventSensorImuDataType>;
class SensorImuServiceInterfaceClient final {
public:
    SensorImuServiceInterfaceClient() = delete;
    explicit SensorImuServiceInterfaceClient(const uint32_t instanceId);
    virtual ~SensorImuServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventSensorImuNotifyHandler(const EventSensorImuHandlerType handler);
    void EventSensorImuNotify();
    void EventSensorImuContainerClear();
    bool EventSensorImuContainerEmpty();
    RecvEventSensorImuType GetEventSensorImuOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventSensorImuType GetEventSensorImuOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventSensorImuType> GetEventSensorImuNdata(const size_t n);

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