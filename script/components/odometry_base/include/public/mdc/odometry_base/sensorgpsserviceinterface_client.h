/* *
 * Class: SensorGpsServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORGPSSERVICEINTERFACECLIENT
#define ARA_ADSFI_SENSORGPSSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msghafgnssinfo.h"





namespace ara {
namespace adsfi {

using EventGpsHandlerType = std::function<void (ara::adsfi::MsgHafGnssInfo)>;
using EventGpsDataType = ara::adsfi::MsgHafGnssInfo;
using RecvEventGpsType = std::shared_ptr<EventGpsDataType>;
class SensorGpsServiceInterfaceClient final {
public:
    SensorGpsServiceInterfaceClient() = delete;
    explicit SensorGpsServiceInterfaceClient(const uint32_t instanceId);
    virtual ~SensorGpsServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventGpsNotifyHandler(const EventGpsHandlerType handler);
    void EventGpsNotify();
    void EventGpsContainerClear();
    bool EventGpsContainerEmpty();
    RecvEventGpsType GetEventGpsOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventGpsType GetEventGpsOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventGpsType> GetEventGpsNdata(const size_t n);

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