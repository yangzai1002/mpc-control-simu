/* *
 * CLASS: VisionSegmentServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/multy_camera_det_base/visionsegmentserviceinterface_server.h"
#include "mdc/multy_camera_det_base/visionsegmentserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class VisionSegmentServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        VisionSegmentServiceInterfaceServerPtr_ = std::make_unique<VisionSegmentServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<VisionSegmentServiceInterfaceServerImpl>& GetVisionSegmentServiceInterfaceServer()
    {
        return VisionSegmentServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<VisionSegmentServiceInterfaceServerImpl> VisionSegmentServiceInterfaceServerPtr_;
};

VisionSegmentServiceInterfaceServer::VisionSegmentServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<VisionSegmentServiceInterfaceServer::Impl>(id);
}

VisionSegmentServiceInterfaceServer::~VisionSegmentServiceInterfaceServer()
{}

bool VisionSegmentServiceInterfaceServer::Init()
{
    return pImpl_->GetVisionSegmentServiceInterfaceServer()->Init();
}

void VisionSegmentServiceInterfaceServer::Stop()
{
    pImpl_->GetVisionSegmentServiceInterfaceServer()->Stop();
}

bool VisionSegmentServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetVisionSegmentServiceInterfaceServer()->IsStop();
}

uint32_t VisionSegmentServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetVisionSegmentServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool VisionSegmentServiceInterfaceServer::SendEventVisionSegmentData(const SendEventVisionSegmentType& data)
{
    return pImpl_->GetVisionSegmentServiceInterfaceServer()->SendEventVisionSegmentData(data);
}
void VisionSegmentServiceInterfaceServer::ClearEventVisionSegmentContainer()
{
    pImpl_->GetVisionSegmentServiceInterfaceServer()->ClearEventVisionSegmentContainer();
}

} /* namespace adsfi */
} /* namespace ara */
