/* *
 * CLASS: FaultDataServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xfault_handler_base/faultdataserviceinterface_server.h"
#include "mdc/xfault_handler_base/faultdataserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class FaultDataServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        FaultDataServiceInterfaceServerPtr_ = std::make_unique<FaultDataServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<FaultDataServiceInterfaceServerImpl>& GetFaultDataServiceInterfaceServer()
    {
        return FaultDataServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<FaultDataServiceInterfaceServerImpl> FaultDataServiceInterfaceServerPtr_;
};

FaultDataServiceInterfaceServer::FaultDataServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<FaultDataServiceInterfaceServer::Impl>(id);
}

FaultDataServiceInterfaceServer::~FaultDataServiceInterfaceServer()
{}

bool FaultDataServiceInterfaceServer::Init()
{
    return pImpl_->GetFaultDataServiceInterfaceServer()->Init();
}

void FaultDataServiceInterfaceServer::Stop()
{
    pImpl_->GetFaultDataServiceInterfaceServer()->Stop();
}

bool FaultDataServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetFaultDataServiceInterfaceServer()->IsStop();
}

uint32_t FaultDataServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetFaultDataServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool FaultDataServiceInterfaceServer::SendEventFaultDataData(const SendEventFaultDataType& data)
{
    return pImpl_->GetFaultDataServiceInterfaceServer()->SendEventFaultDataData(data);
}
void FaultDataServiceInterfaceServer::ClearEventFaultDataContainer()
{
    pImpl_->GetFaultDataServiceInterfaceServer()->ClearEventFaultDataContainer();
}

} /* namespace adsfi */
} /* namespace ara */
