/* *
 * Class: FaultDataServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_FAULTDATASERVICEINTERFACECLIENT
#define ARA_ADSFI_FAULTDATASERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_faultdata.h"





namespace ara {
namespace adsfi {

using EventFaultDataHandlerType = std::function<void (ara::adsfi::FaultData)>;
using EventFaultDataDataType = ara::adsfi::FaultData;
using RecvEventFaultDataType = std::shared_ptr<EventFaultDataDataType>;
class FaultDataServiceInterfaceClient final {
public:
    FaultDataServiceInterfaceClient() = delete;
    explicit FaultDataServiceInterfaceClient(const uint32_t instanceId);
    virtual ~FaultDataServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventFaultDataNotifyHandler(const EventFaultDataHandlerType handler);
    void EventFaultDataNotify();
    void EventFaultDataContainerClear();
    bool EventFaultDataContainerEmpty();
    RecvEventFaultDataType GetEventFaultDataOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventFaultDataType GetEventFaultDataOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventFaultDataType> GetEventFaultDataNdata(const size_t n);

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