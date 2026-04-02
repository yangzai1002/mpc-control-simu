/* *
 * CLASS: SensorImuServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/bd21_imu/sensorimuserviceinterface_server.h"
#include "mdc/bd21_imu/sensorimuserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class SensorImuServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        SensorImuServiceInterfaceServerPtr_ = std::make_unique<SensorImuServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<SensorImuServiceInterfaceServerImpl>& GetSensorImuServiceInterfaceServer()
    {
        return SensorImuServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<SensorImuServiceInterfaceServerImpl> SensorImuServiceInterfaceServerPtr_;
};

SensorImuServiceInterfaceServer::SensorImuServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<SensorImuServiceInterfaceServer::Impl>(id);
}

SensorImuServiceInterfaceServer::~SensorImuServiceInterfaceServer()
{}

bool SensorImuServiceInterfaceServer::Init()
{
    return pImpl_->GetSensorImuServiceInterfaceServer()->Init();
}

void SensorImuServiceInterfaceServer::Stop()
{
    pImpl_->GetSensorImuServiceInterfaceServer()->Stop();
}

bool SensorImuServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetSensorImuServiceInterfaceServer()->IsStop();
}

uint32_t SensorImuServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetSensorImuServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool SensorImuServiceInterfaceServer::SendEventSensorImuData(const SendEventSensorImuType& data)
{
    return pImpl_->GetSensorImuServiceInterfaceServer()->SendEventSensorImuData(data);
}
void SensorImuServiceInterfaceServer::ClearEventSensorImuContainer()
{
    pImpl_->GetSensorImuServiceInterfaceServer()->ClearEventSensorImuContainer();
}

} /* namespace adsfi */
} /* namespace ara */
