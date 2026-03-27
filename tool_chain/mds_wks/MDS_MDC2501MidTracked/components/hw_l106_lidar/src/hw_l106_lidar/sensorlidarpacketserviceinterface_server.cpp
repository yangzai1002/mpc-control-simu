/* *
 * CLASS: SensorLidarPacketServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_l106_lidar/sensorlidarpacketserviceinterface_server.h"
#include "mdc/hw_l106_lidar/sensorlidarpacketserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class SensorLidarPacketServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        SensorLidarPacketServiceInterfaceServerPtr_ = std::make_unique<SensorLidarPacketServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<SensorLidarPacketServiceInterfaceServerImpl>& GetSensorLidarPacketServiceInterfaceServer()
    {
        return SensorLidarPacketServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<SensorLidarPacketServiceInterfaceServerImpl> SensorLidarPacketServiceInterfaceServerPtr_;
};

SensorLidarPacketServiceInterfaceServer::SensorLidarPacketServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<SensorLidarPacketServiceInterfaceServer::Impl>(id);
}

SensorLidarPacketServiceInterfaceServer::~SensorLidarPacketServiceInterfaceServer()
{}

bool SensorLidarPacketServiceInterfaceServer::Init()
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceServer()->Init();
}

void SensorLidarPacketServiceInterfaceServer::Stop()
{
    pImpl_->GetSensorLidarPacketServiceInterfaceServer()->Stop();
}

bool SensorLidarPacketServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceServer()->IsStop();
}

uint32_t SensorLidarPacketServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool SensorLidarPacketServiceInterfaceServer::SendEventLidarPacketData(const SendEventLidarPacketType& data)
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceServer()->SendEventLidarPacketData(data);
}
void SensorLidarPacketServiceInterfaceServer::ClearEventLidarPacketContainer()
{
    pImpl_->GetSensorLidarPacketServiceInterfaceServer()->ClearEventLidarPacketContainer();
}

} /* namespace adsfi */
} /* namespace ara */
