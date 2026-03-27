/* *
 * CLASS: SensorCompressedImageListServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_camera_list/sensorcompressedimagelistserviceinterface_server.h"
#include "mdc/hw_camera_list/sensorcompressedimagelistserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class SensorCompressedImageListServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        SensorCompressedImageListServiceInterfaceServerPtr_ = std::make_unique<SensorCompressedImageListServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<SensorCompressedImageListServiceInterfaceServerImpl>& GetSensorCompressedImageListServiceInterfaceServer()
    {
        return SensorCompressedImageListServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<SensorCompressedImageListServiceInterfaceServerImpl> SensorCompressedImageListServiceInterfaceServerPtr_;
};

SensorCompressedImageListServiceInterfaceServer::SensorCompressedImageListServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<SensorCompressedImageListServiceInterfaceServer::Impl>(id);
}

SensorCompressedImageListServiceInterfaceServer::~SensorCompressedImageListServiceInterfaceServer()
{}

bool SensorCompressedImageListServiceInterfaceServer::Init()
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceServer()->Init();
}

void SensorCompressedImageListServiceInterfaceServer::Stop()
{
    pImpl_->GetSensorCompressedImageListServiceInterfaceServer()->Stop();
}

bool SensorCompressedImageListServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceServer()->IsStop();
}

uint32_t SensorCompressedImageListServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool SensorCompressedImageListServiceInterfaceServer::SendEventCompressedImageListData(const SendEventCompressedImageListType& data)
{
    return pImpl_->GetSensorCompressedImageListServiceInterfaceServer()->SendEventCompressedImageListData(data);
}
void SensorCompressedImageListServiceInterfaceServer::ClearEventCompressedImageListContainer()
{
    pImpl_->GetSensorCompressedImageListServiceInterfaceServer()->ClearEventCompressedImageListContainer();
}

} /* namespace adsfi */
} /* namespace ara */
