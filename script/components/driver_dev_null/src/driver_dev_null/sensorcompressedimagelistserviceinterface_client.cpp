/* *
 * CLASS: SensorCompressedImageListServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/driver_dev_null/sensorcompressedimagelistserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/driver_dev_null/sensorcompressedimagelistserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class SensorCompressedImageListServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        SensorCompressedImageListServiceInterfaceClientPtr_ = std::make_unique<SensorCompressedImageListServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<SensorCompressedImageListServiceInterfaceClientImpl>& GetSensorCompressedImageListServiceInterfaceClient()
    {
        return SensorCompressedImageListServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<SensorCompressedImageListServiceInterfaceClientImpl> SensorCompressedImageListServiceInterfaceClientPtr_;
};

SensorCompressedImageListServiceInterfaceClient::SensorCompressedImageListServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<SensorCompressedImageListServiceInterfaceClient::Impl>(instanceId);
}

SensorCompressedImageListServiceInterfaceClient::~SensorCompressedImageListServiceInterfaceClient()
{}

bool SensorCompressedImageListServiceInterfaceClient::Init()
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->Init();
}

void SensorCompressedImageListServiceInterfaceClient::Stop()
{
    pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->Stop();
}

/* event relative */
void SensorCompressedImageListServiceInterfaceClient::RegisterEventCompressedImageListNotifyHandler(const EventCompressedImageListHandlerType handler)
{
    pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->RegisterEventCompressedImageListNotifyHandler(handler);
}
void SensorCompressedImageListServiceInterfaceClient::EventCompressedImageListNotify()
{
    pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->EventCompressedImageListNotify();
}
void SensorCompressedImageListServiceInterfaceClient::EventCompressedImageListContainerClear()
{
    pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->EventCompressedImageListContainerClear();
}
bool SensorCompressedImageListServiceInterfaceClient::EventCompressedImageListContainerEmpty()
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->EventCompressedImageListContainerEmpty();
}
RecvEventCompressedImageListType SensorCompressedImageListServiceInterfaceClient::GetEventCompressedImageListOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->GetEventCompressedImageListOneData(freshDataTime);
}
RecvEventCompressedImageListType SensorCompressedImageListServiceInterfaceClient::GetEventCompressedImageListOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->GetEventCompressedImageListOneDataBlocking(blockTimeout);
}
std::vector<RecvEventCompressedImageListType> SensorCompressedImageListServiceInterfaceClient::GetEventCompressedImageListNdata(const size_t n)
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->GetEventCompressedImageListNdata(n);
}

uint32_t SensorCompressedImageListServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->GetRecvQSize();
}
bool SensorCompressedImageListServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->IsStop();
}
uint32_t SensorCompressedImageListServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
