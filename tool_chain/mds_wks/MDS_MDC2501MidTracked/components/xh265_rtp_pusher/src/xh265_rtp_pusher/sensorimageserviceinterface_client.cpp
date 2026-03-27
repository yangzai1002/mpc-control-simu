/* *
 * CLASS: SensorImageServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xh265_rtp_pusher/sensorimageserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/xh265_rtp_pusher/sensorimageserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class SensorImageServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        SensorImageServiceInterfaceClientPtr_ = std::make_unique<SensorImageServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<SensorImageServiceInterfaceClientImpl>& GetSensorImageServiceInterfaceClient()
    {
        return SensorImageServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<SensorImageServiceInterfaceClientImpl> SensorImageServiceInterfaceClientPtr_;
};

SensorImageServiceInterfaceClient::SensorImageServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<SensorImageServiceInterfaceClient::Impl>(instanceId);
}

SensorImageServiceInterfaceClient::~SensorImageServiceInterfaceClient()
{}

bool SensorImageServiceInterfaceClient::Init()
{
    return pImpl_->GetSensorImageServiceInterfaceClient()->Init();
}

void SensorImageServiceInterfaceClient::Stop()
{
    pImpl_->GetSensorImageServiceInterfaceClient()->Stop();
}

/* event relative */
void SensorImageServiceInterfaceClient::RegisterEventSensorImageNotifyHandler(const EventSensorImageHandlerType handler)
{
    pImpl_->GetSensorImageServiceInterfaceClient()->RegisterEventSensorImageNotifyHandler(handler);
}
void SensorImageServiceInterfaceClient::EventSensorImageNotify()
{
    pImpl_->GetSensorImageServiceInterfaceClient()->EventSensorImageNotify();
}
void SensorImageServiceInterfaceClient::EventSensorImageContainerClear()
{
    pImpl_->GetSensorImageServiceInterfaceClient()->EventSensorImageContainerClear();
}
bool SensorImageServiceInterfaceClient::EventSensorImageContainerEmpty()
{
    return pImpl_->GetSensorImageServiceInterfaceClient()->EventSensorImageContainerEmpty();
}
RecvEventSensorImageType SensorImageServiceInterfaceClient::GetEventSensorImageOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetSensorImageServiceInterfaceClient()->GetEventSensorImageOneData(freshDataTime);
}
RecvEventSensorImageType SensorImageServiceInterfaceClient::GetEventSensorImageOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetSensorImageServiceInterfaceClient()->GetEventSensorImageOneDataBlocking(blockTimeout);
}
std::vector<RecvEventSensorImageType> SensorImageServiceInterfaceClient::GetEventSensorImageNdata(const size_t n)
{
    return pImpl_->GetSensorImageServiceInterfaceClient()->GetEventSensorImageNdata(n);
}

uint32_t SensorImageServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetSensorImageServiceInterfaceClient()->GetRecvQSize();
}
bool SensorImageServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetSensorImageServiceInterfaceClient()->IsStop();
}
uint32_t SensorImageServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetSensorImageServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
