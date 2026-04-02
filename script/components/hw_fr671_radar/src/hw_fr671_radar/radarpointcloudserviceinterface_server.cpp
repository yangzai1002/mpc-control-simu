/* *
 * CLASS: RadarPointCloudServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_fr671_radar/radarpointcloudserviceinterface_server.h"
#include "mdc/hw_fr671_radar/radarpointcloudserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class RadarPointCloudServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        RadarPointCloudServiceInterfaceServerPtr_ = std::make_unique<RadarPointCloudServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<RadarPointCloudServiceInterfaceServerImpl>& GetRadarPointCloudServiceInterfaceServer()
    {
        return RadarPointCloudServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<RadarPointCloudServiceInterfaceServerImpl> RadarPointCloudServiceInterfaceServerPtr_;
};

RadarPointCloudServiceInterfaceServer::RadarPointCloudServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<RadarPointCloudServiceInterfaceServer::Impl>(id);
}

RadarPointCloudServiceInterfaceServer::~RadarPointCloudServiceInterfaceServer()
{}

bool RadarPointCloudServiceInterfaceServer::Init()
{
    return pImpl_->GetRadarPointCloudServiceInterfaceServer()->Init();
}

void RadarPointCloudServiceInterfaceServer::Stop()
{
    pImpl_->GetRadarPointCloudServiceInterfaceServer()->Stop();
}

bool RadarPointCloudServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetRadarPointCloudServiceInterfaceServer()->IsStop();
}

uint32_t RadarPointCloudServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetRadarPointCloudServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool RadarPointCloudServiceInterfaceServer::SendEventRadarPointCloudData(const SendEventRadarPointCloudType& data)
{
    return pImpl_->GetRadarPointCloudServiceInterfaceServer()->SendEventRadarPointCloudData(data);
}
void RadarPointCloudServiceInterfaceServer::ClearEventRadarPointCloudContainer()
{
    pImpl_->GetRadarPointCloudServiceInterfaceServer()->ClearEventRadarPointCloudContainer();
}

} /* namespace adsfi */
} /* namespace ara */
