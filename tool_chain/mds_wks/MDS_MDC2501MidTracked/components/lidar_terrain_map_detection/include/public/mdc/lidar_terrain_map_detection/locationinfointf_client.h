/* *
 * Class: LocationInfoIntf client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ADSFI_LOCATIONINFOINTFCLIENT
#define ADSFI_LOCATIONINFOINTFCLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msghaflocation.h"





namespace adsfi {

using LocationInfoHandlerType = std::function<void (ara::adsfi::MsgHafLocation)>;
using LocationInfoDataType = ara::adsfi::MsgHafLocation;
using RecvLocationInfoType = std::shared_ptr<LocationInfoDataType>;
class LocationInfoIntfClient final {
public:
    LocationInfoIntfClient() = delete;
    explicit LocationInfoIntfClient(const uint32_t instanceId);
    virtual ~LocationInfoIntfClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterLocationInfoNotifyHandler(const LocationInfoHandlerType handler);
    void LocationInfoNotify();
    void LocationInfoContainerClear();
    bool LocationInfoContainerEmpty();
    RecvLocationInfoType GetLocationInfoOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvLocationInfoType GetLocationInfoOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvLocationInfoType> GetLocationInfoNdata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */

#endif