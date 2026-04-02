/* *
 * CLASS: SensorLidarPointCloudServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_l106_lidar/sensorlidarpointcloudserviceinterface_server.h"
#include "mdc/hw_l106_lidar/sensorlidarpointcloudserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class SensorLidarPointCloudServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        SensorLidarPointCloudServiceInterfaceServerPtr_ = std::make_unique<SensorLidarPointCloudServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<SensorLidarPointCloudServiceInterfaceServerImpl>& GetSensorLidarPointCloudServiceInterfaceServer()
    {
        return SensorLidarPointCloudServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<SensorLidarPointCloudServiceInterfaceServerImpl> SensorLidarPointCloudServiceInterfaceServerPtr_;
};

SensorLidarPointCloudServiceInterfaceServer::SensorLidarPointCloudServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<SensorLidarPointCloudServiceInterfaceServer::Impl>(id);
}

SensorLidarPointCloudServiceInterfaceServer::~SensorLidarPointCloudServiceInterfaceServer()
{}

bool SensorLidarPointCloudServiceInterfaceServer::Init()
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceServer()->Init();
}

void SensorLidarPointCloudServiceInterfaceServer::Stop()
{
    pImpl_->GetSensorLidarPointCloudServiceInterfaceServer()->Stop();
}

bool SensorLidarPointCloudServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceServer()->IsStop();
}

uint32_t SensorLidarPointCloudServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool SensorLidarPointCloudServiceInterfaceServer::SendEventPointCloud2Data(const SendEventPointCloud2Type& data)
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceServer()->SendEventPointCloud2Data(data);
}
void SensorLidarPointCloudServiceInterfaceServer::ClearEventPointCloud2Container()
{
    pImpl_->GetSensorLidarPointCloudServiceInterfaceServer()->ClearEventPointCloud2Container();
}

} /* namespace adsfi */
} /* namespace ara */
