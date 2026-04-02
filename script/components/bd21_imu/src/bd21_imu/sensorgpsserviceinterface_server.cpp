/* *
 * CLASS: SensorGpsServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/bd21_imu/sensorgpsserviceinterface_server.h"
#include "mdc/bd21_imu/sensorgpsserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class SensorGpsServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        SensorGpsServiceInterfaceServerPtr_ = std::make_unique<SensorGpsServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<SensorGpsServiceInterfaceServerImpl>& GetSensorGpsServiceInterfaceServer()
    {
        return SensorGpsServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<SensorGpsServiceInterfaceServerImpl> SensorGpsServiceInterfaceServerPtr_;
};

SensorGpsServiceInterfaceServer::SensorGpsServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<SensorGpsServiceInterfaceServer::Impl>(id);
}

SensorGpsServiceInterfaceServer::~SensorGpsServiceInterfaceServer()
{}

bool SensorGpsServiceInterfaceServer::Init()
{
    return pImpl_->GetSensorGpsServiceInterfaceServer()->Init();
}

void SensorGpsServiceInterfaceServer::Stop()
{
    pImpl_->GetSensorGpsServiceInterfaceServer()->Stop();
}

bool SensorGpsServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetSensorGpsServiceInterfaceServer()->IsStop();
}

uint32_t SensorGpsServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetSensorGpsServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool SensorGpsServiceInterfaceServer::SendEventGpsData(const SendEventGpsType& data)
{
    return pImpl_->GetSensorGpsServiceInterfaceServer()->SendEventGpsData(data);
}
void SensorGpsServiceInterfaceServer::ClearEventGpsContainer()
{
    pImpl_->GetSensorGpsServiceInterfaceServer()->ClearEventGpsContainer();
}

} /* namespace adsfi */
} /* namespace ara */
