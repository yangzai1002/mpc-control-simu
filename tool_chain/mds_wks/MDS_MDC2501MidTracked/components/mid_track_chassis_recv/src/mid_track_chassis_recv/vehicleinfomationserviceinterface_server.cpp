/* *
 * CLASS: VehicleInfomationServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/mid_track_chassis_recv/vehicleinfomationserviceinterface_server.h"
#include "mdc/mid_track_chassis_recv/vehicleinfomationserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class VehicleInfomationServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        VehicleInfomationServiceInterfaceServerPtr_ = std::make_unique<VehicleInfomationServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<VehicleInfomationServiceInterfaceServerImpl>& GetVehicleInfomationServiceInterfaceServer()
    {
        return VehicleInfomationServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<VehicleInfomationServiceInterfaceServerImpl> VehicleInfomationServiceInterfaceServerPtr_;
};

VehicleInfomationServiceInterfaceServer::VehicleInfomationServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<VehicleInfomationServiceInterfaceServer::Impl>(id);
}

VehicleInfomationServiceInterfaceServer::~VehicleInfomationServiceInterfaceServer()
{}

bool VehicleInfomationServiceInterfaceServer::Init()
{
    return pImpl_->GetVehicleInfomationServiceInterfaceServer()->Init();
}

void VehicleInfomationServiceInterfaceServer::Stop()
{
    pImpl_->GetVehicleInfomationServiceInterfaceServer()->Stop();
}

bool VehicleInfomationServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetVehicleInfomationServiceInterfaceServer()->IsStop();
}

uint32_t VehicleInfomationServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetVehicleInfomationServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool VehicleInfomationServiceInterfaceServer::SendEventVehicleInfoData(const SendEventVehicleInfoType& data)
{
    return pImpl_->GetVehicleInfomationServiceInterfaceServer()->SendEventVehicleInfoData(data);
}
void VehicleInfomationServiceInterfaceServer::ClearEventVehicleInfoContainer()
{
    pImpl_->GetVehicleInfomationServiceInterfaceServer()->ClearEventVehicleInfoContainer();
}

} /* namespace adsfi */
} /* namespace ara */
