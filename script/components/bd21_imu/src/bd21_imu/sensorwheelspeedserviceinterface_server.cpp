/* *
 * CLASS: SensorWheelSpeedServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/bd21_imu/sensorwheelspeedserviceinterface_server.h"
#include "mdc/bd21_imu/sensorwheelspeedserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class SensorWheelSpeedServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        SensorWheelSpeedServiceInterfaceServerPtr_ = std::make_unique<SensorWheelSpeedServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<SensorWheelSpeedServiceInterfaceServerImpl>& GetSensorWheelSpeedServiceInterfaceServer()
    {
        return SensorWheelSpeedServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<SensorWheelSpeedServiceInterfaceServerImpl> SensorWheelSpeedServiceInterfaceServerPtr_;
};

SensorWheelSpeedServiceInterfaceServer::SensorWheelSpeedServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<SensorWheelSpeedServiceInterfaceServer::Impl>(id);
}

SensorWheelSpeedServiceInterfaceServer::~SensorWheelSpeedServiceInterfaceServer()
{}

bool SensorWheelSpeedServiceInterfaceServer::Init()
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceServer()->Init();
}

void SensorWheelSpeedServiceInterfaceServer::Stop()
{
    pImpl_->GetSensorWheelSpeedServiceInterfaceServer()->Stop();
}

bool SensorWheelSpeedServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceServer()->IsStop();
}

uint32_t SensorWheelSpeedServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool SensorWheelSpeedServiceInterfaceServer::SendEventWheelSpeedData(const SendEventWheelSpeedType& data)
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceServer()->SendEventWheelSpeedData(data);
}
void SensorWheelSpeedServiceInterfaceServer::ClearEventWheelSpeedContainer()
{
    pImpl_->GetSensorWheelSpeedServiceInterfaceServer()->ClearEventWheelSpeedContainer();
}

} /* namespace adsfi */
} /* namespace ara */
