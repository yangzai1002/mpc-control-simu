/* *
 * Class: Object3dArrayIntf client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ADSFI_OBJECT3DARRAYINTFCLIENT
#define ADSFI_OBJECT3DARRAYINTFCLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msghaf3ddetectionoutarray.h"





namespace adsfi {

using EventPerceptionLidarObjectsHandlerType = std::function<void (ara::adsfi::MsgHaf3dDetectionOutArray)>;
using EventPerceptionLidarObjectsDataType = ara::adsfi::MsgHaf3dDetectionOutArray;
using RecvEventPerceptionLidarObjectsType = std::shared_ptr<EventPerceptionLidarObjectsDataType>;
class Object3dArrayIntfClient final {
public:
    Object3dArrayIntfClient() = delete;
    explicit Object3dArrayIntfClient(const uint32_t instanceId);
    virtual ~Object3dArrayIntfClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventPerceptionLidarObjectsNotifyHandler(const EventPerceptionLidarObjectsHandlerType handler);
    void EventPerceptionLidarObjectsNotify();
    void EventPerceptionLidarObjectsContainerClear();
    bool EventPerceptionLidarObjectsContainerEmpty();
    RecvEventPerceptionLidarObjectsType GetEventPerceptionLidarObjectsOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventPerceptionLidarObjectsType GetEventPerceptionLidarObjectsOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventPerceptionLidarObjectsType> GetEventPerceptionLidarObjectsNdata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */

#endif