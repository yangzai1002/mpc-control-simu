/* *
 * CLASS: FusionObjectArrayIntf server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/fusion_follow_base/fusionobjectarrayintf_server.h"
#include "mdc/fusion_follow_base/fusionobjectarrayintf_server_impl.h"

namespace adsfi {
class FusionObjectArrayIntfServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        FusionObjectArrayIntfServerPtr_ = std::make_unique<FusionObjectArrayIntfServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<FusionObjectArrayIntfServerImpl>& GetFusionObjectArrayIntfServer()
    {
        return FusionObjectArrayIntfServerPtr_;
    }
private:
    std::unique_ptr<FusionObjectArrayIntfServerImpl> FusionObjectArrayIntfServerPtr_;
};

FusionObjectArrayIntfServer::FusionObjectArrayIntfServer(const uint32_t id)
{
    pImpl_ = std::make_unique<FusionObjectArrayIntfServer::Impl>(id);
}

FusionObjectArrayIntfServer::~FusionObjectArrayIntfServer()
{}

bool FusionObjectArrayIntfServer::Init()
{
    return pImpl_->GetFusionObjectArrayIntfServer()->Init();
}

void FusionObjectArrayIntfServer::Stop()
{
    pImpl_->GetFusionObjectArrayIntfServer()->Stop();
}

bool FusionObjectArrayIntfServer::IsStop() const
{
    return pImpl_->GetFusionObjectArrayIntfServer()->IsStop();
}

uint32_t FusionObjectArrayIntfServer::GetInstanceId() const
{
    return pImpl_->GetFusionObjectArrayIntfServer()->GetInstanceId();
}

/* event relative */
bool FusionObjectArrayIntfServer::SendFusionObjectArrayData(const SendFusionObjectArrayType& data)
{
    return pImpl_->GetFusionObjectArrayIntfServer()->SendFusionObjectArrayData(data);
}
void FusionObjectArrayIntfServer::ClearFusionObjectArrayContainer()
{
    pImpl_->GetFusionObjectArrayIntfServer()->ClearFusionObjectArrayContainer();
}

} /* namespace adsfi */
