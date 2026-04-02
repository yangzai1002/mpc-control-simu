/* *
 * Class: SensorWheelSpeedServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORWHEELSPEEDSERVICEINTERFACECLIENT
#define ARA_ADSFI_SENSORWHEELSPEEDSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msghafwheelspeedlist.h"





namespace ara {
namespace adsfi {

using EventWheelSpeedHandlerType = std::function<void (ara::adsfi::MsgHafWheelSpeedList)>;
using EventWheelSpeedDataType = ara::adsfi::MsgHafWheelSpeedList;
using RecvEventWheelSpeedType = std::shared_ptr<EventWheelSpeedDataType>;
class SensorWheelSpeedServiceInterfaceClient final {
public:
    SensorWheelSpeedServiceInterfaceClient() = delete;
    explicit SensorWheelSpeedServiceInterfaceClient(const uint32_t instanceId);
    virtual ~SensorWheelSpeedServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventWheelSpeedNotifyHandler(const EventWheelSpeedHandlerType handler);
    void EventWheelSpeedNotify();
    void EventWheelSpeedContainerClear();
    bool EventWheelSpeedContainerEmpty();
    RecvEventWheelSpeedType GetEventWheelSpeedOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventWheelSpeedType GetEventWheelSpeedOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventWheelSpeedType> GetEventWheelSpeedNdata(const size_t n);

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