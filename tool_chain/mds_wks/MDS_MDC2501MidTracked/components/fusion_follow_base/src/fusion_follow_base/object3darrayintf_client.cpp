/* *
 * CLASS: Object3dArrayIntf client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/fusion_follow_base/object3darrayintf_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/fusion_follow_base/object3darrayintf_client_impl.h"

namespace adsfi {
class Object3dArrayIntfClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        Object3dArrayIntfClientPtr_ = std::make_unique<Object3dArrayIntfClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<Object3dArrayIntfClientImpl>& GetObject3dArrayIntfClient()
    {
        return Object3dArrayIntfClientPtr_;
    }
private:
    std::unique_ptr<Object3dArrayIntfClientImpl> Object3dArrayIntfClientPtr_;
};

Object3dArrayIntfClient::Object3dArrayIntfClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<Object3dArrayIntfClient::Impl>(instanceId);
}

Object3dArrayIntfClient::~Object3dArrayIntfClient()
{}

bool Object3dArrayIntfClient::Init()
{
    return pImpl_->GetObject3dArrayIntfClient()->Init();
}

void Object3dArrayIntfClient::Stop()
{
    pImpl_->GetObject3dArrayIntfClient()->Stop();
}

/* event relative */
void Object3dArrayIntfClient::RegisterEventPerceptionLidarObjectsNotifyHandler(const EventPerceptionLidarObjectsHandlerType handler)
{
    pImpl_->GetObject3dArrayIntfClient()->RegisterEventPerceptionLidarObjectsNotifyHandler(handler);
}
void Object3dArrayIntfClient::EventPerceptionLidarObjectsNotify()
{
    pImpl_->GetObject3dArrayIntfClient()->EventPerceptionLidarObjectsNotify();
}
void Object3dArrayIntfClient::EventPerceptionLidarObjectsContainerClear()
{
    pImpl_->GetObject3dArrayIntfClient()->EventPerceptionLidarObjectsContainerClear();
}
bool Object3dArrayIntfClient::EventPerceptionLidarObjectsContainerEmpty()
{
    return pImpl_->GetObject3dArrayIntfClient()->EventPerceptionLidarObjectsContainerEmpty();
}
RecvEventPerceptionLidarObjectsType Object3dArrayIntfClient::GetEventPerceptionLidarObjectsOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetObject3dArrayIntfClient()->GetEventPerceptionLidarObjectsOneData(freshDataTime);
}
RecvEventPerceptionLidarObjectsType Object3dArrayIntfClient::GetEventPerceptionLidarObjectsOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetObject3dArrayIntfClient()->GetEventPerceptionLidarObjectsOneDataBlocking(blockTimeout);
}
std::vector<RecvEventPerceptionLidarObjectsType> Object3dArrayIntfClient::GetEventPerceptionLidarObjectsNdata(const size_t n)
{
    return pImpl_->GetObject3dArrayIntfClient()->GetEventPerceptionLidarObjectsNdata(n);
}

uint32_t Object3dArrayIntfClient::GetRecvQSize() const
{
    return pImpl_->GetObject3dArrayIntfClient()->GetRecvQSize();
}
bool Object3dArrayIntfClient::IsStop() const
{
    return pImpl_->GetObject3dArrayIntfClient()->IsStop();
}
uint32_t Object3dArrayIntfClient::GetInstanceId() const
{
    return pImpl_->GetObject3dArrayIntfClient()->GetInstanceId();
}



} /* namespace adsfi */
