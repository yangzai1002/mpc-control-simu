/* *
 * CLASS: MsgImageDataListServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/sensor_viualize_base/msgimagedatalistserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/sensor_viualize_base/msgimagedatalistserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class MsgImageDataListServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        MsgImageDataListServiceInterfaceClientPtr_ = std::make_unique<MsgImageDataListServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<MsgImageDataListServiceInterfaceClientImpl>& GetMsgImageDataListServiceInterfaceClient()
    {
        return MsgImageDataListServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<MsgImageDataListServiceInterfaceClientImpl> MsgImageDataListServiceInterfaceClientPtr_;
};

MsgImageDataListServiceInterfaceClient::MsgImageDataListServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<MsgImageDataListServiceInterfaceClient::Impl>(instanceId);
}

MsgImageDataListServiceInterfaceClient::~MsgImageDataListServiceInterfaceClient()
{}

bool MsgImageDataListServiceInterfaceClient::Init()
{
    return pImpl_->GetMsgImageDataListServiceInterfaceClient()->Init();
}

void MsgImageDataListServiceInterfaceClient::Stop()
{
    pImpl_->GetMsgImageDataListServiceInterfaceClient()->Stop();
}

/* event relative */
void MsgImageDataListServiceInterfaceClient::RegisterEventImageListNotifyHandler(const EventImageListHandlerType handler)
{
    pImpl_->GetMsgImageDataListServiceInterfaceClient()->RegisterEventImageListNotifyHandler(handler);
}
void MsgImageDataListServiceInterfaceClient::EventImageListNotify()
{
    pImpl_->GetMsgImageDataListServiceInterfaceClient()->EventImageListNotify();
}
void MsgImageDataListServiceInterfaceClient::EventImageListContainerClear()
{
    pImpl_->GetMsgImageDataListServiceInterfaceClient()->EventImageListContainerClear();
}
bool MsgImageDataListServiceInterfaceClient::EventImageListContainerEmpty()
{
    return pImpl_->GetMsgImageDataListServiceInterfaceClient()->EventImageListContainerEmpty();
}
RecvEventImageListType MsgImageDataListServiceInterfaceClient::GetEventImageListOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetMsgImageDataListServiceInterfaceClient()->GetEventImageListOneData(freshDataTime);
}
RecvEventImageListType MsgImageDataListServiceInterfaceClient::GetEventImageListOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetMsgImageDataListServiceInterfaceClient()->GetEventImageListOneDataBlocking(blockTimeout);
}
std::vector<RecvEventImageListType> MsgImageDataListServiceInterfaceClient::GetEventImageListNdata(const size_t n)
{
    return pImpl_->GetMsgImageDataListServiceInterfaceClient()->GetEventImageListNdata(n);
}

uint32_t MsgImageDataListServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetMsgImageDataListServiceInterfaceClient()->GetRecvQSize();
}
bool MsgImageDataListServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetMsgImageDataListServiceInterfaceClient()->IsStop();
}
uint32_t MsgImageDataListServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetMsgImageDataListServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
