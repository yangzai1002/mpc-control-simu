/* *
 * CLASS: SensorImageServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xh265_encoder/sensorimageserviceinterface_server.h"
#include "mdc/xh265_encoder/sensorimageserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class SensorImageServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        SensorImageServiceInterfaceServerPtr_ = std::make_unique<SensorImageServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<SensorImageServiceInterfaceServerImpl>& GetSensorImageServiceInterfaceServer()
    {
        return SensorImageServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<SensorImageServiceInterfaceServerImpl> SensorImageServiceInterfaceServerPtr_;
};

SensorImageServiceInterfaceServer::SensorImageServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<SensorImageServiceInterfaceServer::Impl>(id);
}

SensorImageServiceInterfaceServer::~SensorImageServiceInterfaceServer()
{}

bool SensorImageServiceInterfaceServer::Init()
{
    return pImpl_->GetSensorImageServiceInterfaceServer()->Init();
}

void SensorImageServiceInterfaceServer::Stop()
{
    pImpl_->GetSensorImageServiceInterfaceServer()->Stop();
}

bool SensorImageServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetSensorImageServiceInterfaceServer()->IsStop();
}

uint32_t SensorImageServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetSensorImageServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool SensorImageServiceInterfaceServer::SendEventSensorImageData(const SendEventSensorImageType& data)
{
    return pImpl_->GetSensorImageServiceInterfaceServer()->SendEventSensorImageData(data);
}
void SensorImageServiceInterfaceServer::ClearEventSensorImageContainer()
{
    pImpl_->GetSensorImageServiceInterfaceServer()->ClearEventSensorImageContainer();
}

} /* namespace adsfi */
} /* namespace ara */
