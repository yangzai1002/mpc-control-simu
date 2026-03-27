/* *
 * CLASS: VehicleInfomationServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/control_base/vehicleinfomationserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/control_base/vehicleinfomationserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class VehicleInfomationServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        VehicleInfomationServiceInterfaceClientPtr_ = std::make_unique<VehicleInfomationServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<VehicleInfomationServiceInterfaceClientImpl>& GetVehicleInfomationServiceInterfaceClient()
    {
        return VehicleInfomationServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<VehicleInfomationServiceInterfaceClientImpl> VehicleInfomationServiceInterfaceClientPtr_;
};

VehicleInfomationServiceInterfaceClient::VehicleInfomationServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<VehicleInfomationServiceInterfaceClient::Impl>(instanceId);
}

VehicleInfomationServiceInterfaceClient::~VehicleInfomationServiceInterfaceClient()
{}

bool VehicleInfomationServiceInterfaceClient::Init()
{
    return pImpl_->GetVehicleInfomationServiceInterfaceClient()->Init();
}

void VehicleInfomationServiceInterfaceClient::Stop()
{
    pImpl_->GetVehicleInfomationServiceInterfaceClient()->Stop();
}

/* event relative */
void VehicleInfomationServiceInterfaceClient::RegisterEventVehicleInfoNotifyHandler(const EventVehicleInfoHandlerType handler)
{
    pImpl_->GetVehicleInfomationServiceInterfaceClient()->RegisterEventVehicleInfoNotifyHandler(handler);
}
void VehicleInfomationServiceInterfaceClient::EventVehicleInfoNotify()
{
    pImpl_->GetVehicleInfomationServiceInterfaceClient()->EventVehicleInfoNotify();
}
void VehicleInfomationServiceInterfaceClient::EventVehicleInfoContainerClear()
{
    pImpl_->GetVehicleInfomationServiceInterfaceClient()->EventVehicleInfoContainerClear();
}
bool VehicleInfomationServiceInterfaceClient::EventVehicleInfoContainerEmpty()
{
    return pImpl_->GetVehicleInfomationServiceInterfaceClient()->EventVehicleInfoContainerEmpty();
}
RecvEventVehicleInfoType VehicleInfomationServiceInterfaceClient::GetEventVehicleInfoOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetVehicleInfomationServiceInterfaceClient()->GetEventVehicleInfoOneData(freshDataTime);
}
RecvEventVehicleInfoType VehicleInfomationServiceInterfaceClient::GetEventVehicleInfoOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetVehicleInfomationServiceInterfaceClient()->GetEventVehicleInfoOneDataBlocking(blockTimeout);
}
std::vector<RecvEventVehicleInfoType> VehicleInfomationServiceInterfaceClient::GetEventVehicleInfoNdata(const size_t n)
{
    return pImpl_->GetVehicleInfomationServiceInterfaceClient()->GetEventVehicleInfoNdata(n);
}

uint32_t VehicleInfomationServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetVehicleInfomationServiceInterfaceClient()->GetRecvQSize();
}
bool VehicleInfomationServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetVehicleInfomationServiceInterfaceClient()->IsStop();
}
uint32_t VehicleInfomationServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetVehicleInfomationServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
