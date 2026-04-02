/* *
 * Class: LocationServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef MDC_LOCATION_LOCATIONSERVICEINTERFACECLIENT
#define MDC_LOCATION_LOCATIONSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msghaflocation.h"





namespace mdc {
namespace location {

using LocationEventHandlerType = std::function<void (ara::adsfi::MsgHafLocation)>;
using LocationEventDataType = ara::adsfi::MsgHafLocation;
using RecvLocationEventType = std::shared_ptr<LocationEventDataType>;
class LocationServiceInterfaceClient final {
public:
    LocationServiceInterfaceClient() = delete;
    explicit LocationServiceInterfaceClient(const uint32_t instanceId);
    virtual ~LocationServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterLocationEventNotifyHandler(const LocationEventHandlerType handler);
    void LocationEventNotify();
    void LocationEventContainerClear();
    bool LocationEventContainerEmpty();
    RecvLocationEventType GetLocationEventOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvLocationEventType GetLocationEventOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvLocationEventType> GetLocationEventNdata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace location */
} /* namespace mdc */

#endif