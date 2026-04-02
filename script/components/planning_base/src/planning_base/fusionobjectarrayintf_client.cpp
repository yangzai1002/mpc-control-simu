/* *
 * CLASS: FusionObjectArrayIntf client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/planning_base/fusionobjectarrayintf_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/planning_base/fusionobjectarrayintf_client_impl.h"

namespace adsfi {
class FusionObjectArrayIntfClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        FusionObjectArrayIntfClientPtr_ = std::make_unique<FusionObjectArrayIntfClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<FusionObjectArrayIntfClientImpl>& GetFusionObjectArrayIntfClient()
    {
        return FusionObjectArrayIntfClientPtr_;
    }
private:
    std::unique_ptr<FusionObjectArrayIntfClientImpl> FusionObjectArrayIntfClientPtr_;
};

FusionObjectArrayIntfClient::FusionObjectArrayIntfClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<FusionObjectArrayIntfClient::Impl>(instanceId);
}

FusionObjectArrayIntfClient::~FusionObjectArrayIntfClient()
{}

bool FusionObjectArrayIntfClient::Init()
{
    return pImpl_->GetFusionObjectArrayIntfClient()->Init();
}

void FusionObjectArrayIntfClient::Stop()
{
    pImpl_->GetFusionObjectArrayIntfClient()->Stop();
}

/* event relative */
void FusionObjectArrayIntfClient::RegisterFusionObjectArrayNotifyHandler(const FusionObjectArrayHandlerType handler)
{
    pImpl_->GetFusionObjectArrayIntfClient()->RegisterFusionObjectArrayNotifyHandler(handler);
}
void FusionObjectArrayIntfClient::FusionObjectArrayNotify()
{
    pImpl_->GetFusionObjectArrayIntfClient()->FusionObjectArrayNotify();
}
void FusionObjectArrayIntfClient::FusionObjectArrayContainerClear()
{
    pImpl_->GetFusionObjectArrayIntfClient()->FusionObjectArrayContainerClear();
}
bool FusionObjectArrayIntfClient::FusionObjectArrayContainerEmpty()
{
    return pImpl_->GetFusionObjectArrayIntfClient()->FusionObjectArrayContainerEmpty();
}
RecvFusionObjectArrayType FusionObjectArrayIntfClient::GetFusionObjectArrayOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetFusionObjectArrayIntfClient()->GetFusionObjectArrayOneData(freshDataTime);
}
RecvFusionObjectArrayType FusionObjectArrayIntfClient::GetFusionObjectArrayOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetFusionObjectArrayIntfClient()->GetFusionObjectArrayOneDataBlocking(blockTimeout);
}
std::vector<RecvFusionObjectArrayType> FusionObjectArrayIntfClient::GetFusionObjectArrayNdata(const size_t n)
{
    return pImpl_->GetFusionObjectArrayIntfClient()->GetFusionObjectArrayNdata(n);
}

uint32_t FusionObjectArrayIntfClient::GetRecvQSize() const
{
    return pImpl_->GetFusionObjectArrayIntfClient()->GetRecvQSize();
}
bool FusionObjectArrayIntfClient::IsStop() const
{
    return pImpl_->GetFusionObjectArrayIntfClient()->IsStop();
}
uint32_t FusionObjectArrayIntfClient::GetInstanceId() const
{
    return pImpl_->GetFusionObjectArrayIntfClient()->GetInstanceId();
}



} /* namespace adsfi */
