/* *
 * CLASS: SensorRadarPacketServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_fr671_radar/sensorradarpacketserviceinterface_server.h"
#include "mdc/hw_fr671_radar/sensorradarpacketserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class SensorRadarPacketServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        SensorRadarPacketServiceInterfaceServerPtr_ = std::make_unique<SensorRadarPacketServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<SensorRadarPacketServiceInterfaceServerImpl>& GetSensorRadarPacketServiceInterfaceServer()
    {
        return SensorRadarPacketServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<SensorRadarPacketServiceInterfaceServerImpl> SensorRadarPacketServiceInterfaceServerPtr_;
};

SensorRadarPacketServiceInterfaceServer::SensorRadarPacketServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<SensorRadarPacketServiceInterfaceServer::Impl>(id);
}

SensorRadarPacketServiceInterfaceServer::~SensorRadarPacketServiceInterfaceServer()
{}

bool SensorRadarPacketServiceInterfaceServer::Init()
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceServer()->Init();
}

void SensorRadarPacketServiceInterfaceServer::Stop()
{
    pImpl_->GetSensorRadarPacketServiceInterfaceServer()->Stop();
}

bool SensorRadarPacketServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceServer()->IsStop();
}

uint32_t SensorRadarPacketServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool SensorRadarPacketServiceInterfaceServer::SendEventSensorRadarPacketData(const SendEventSensorRadarPacketType& data)
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceServer()->SendEventSensorRadarPacketData(data);
}
void SensorRadarPacketServiceInterfaceServer::ClearEventSensorRadarPacketContainer()
{
    pImpl_->GetSensorRadarPacketServiceInterfaceServer()->ClearEventSensorRadarPacketContainer();
}

} /* namespace adsfi */
} /* namespace ara */
