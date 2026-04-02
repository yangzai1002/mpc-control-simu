/* *
 * CLASS: BagStatusServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xdata_trigger_recorder_base/bagstatusserviceinterface_server.h"
#include "mdc/xdata_trigger_recorder_base/bagstatusserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class BagStatusServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        BagStatusServiceInterfaceServerPtr_ = std::make_unique<BagStatusServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<BagStatusServiceInterfaceServerImpl>& GetBagStatusServiceInterfaceServer()
    {
        return BagStatusServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<BagStatusServiceInterfaceServerImpl> BagStatusServiceInterfaceServerPtr_;
};

BagStatusServiceInterfaceServer::BagStatusServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<BagStatusServiceInterfaceServer::Impl>(id);
}

BagStatusServiceInterfaceServer::~BagStatusServiceInterfaceServer()
{}

bool BagStatusServiceInterfaceServer::Init()
{
    return pImpl_->GetBagStatusServiceInterfaceServer()->Init();
}

void BagStatusServiceInterfaceServer::Stop()
{
    pImpl_->GetBagStatusServiceInterfaceServer()->Stop();
}

bool BagStatusServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetBagStatusServiceInterfaceServer()->IsStop();
}

uint32_t BagStatusServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetBagStatusServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool BagStatusServiceInterfaceServer::SendEventBagStatusData(const SendEventBagStatusType& data)
{
    return pImpl_->GetBagStatusServiceInterfaceServer()->SendEventBagStatusData(data);
}
void BagStatusServiceInterfaceServer::ClearEventBagStatusContainer()
{
    pImpl_->GetBagStatusServiceInterfaceServer()->ClearEventBagStatusContainer();
}

} /* namespace adsfi */
} /* namespace ara */
