/* *
 * CLASS: MsgImageDataListServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_camera_list/msgimagedatalistserviceinterface_server.h"
#include "mdc/hw_camera_list/msgimagedatalistserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class MsgImageDataListServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        MsgImageDataListServiceInterfaceServerPtr_ = std::make_unique<MsgImageDataListServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<MsgImageDataListServiceInterfaceServerImpl>& GetMsgImageDataListServiceInterfaceServer()
    {
        return MsgImageDataListServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<MsgImageDataListServiceInterfaceServerImpl> MsgImageDataListServiceInterfaceServerPtr_;
};

MsgImageDataListServiceInterfaceServer::MsgImageDataListServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<MsgImageDataListServiceInterfaceServer::Impl>(id);
}

MsgImageDataListServiceInterfaceServer::~MsgImageDataListServiceInterfaceServer()
{}

bool MsgImageDataListServiceInterfaceServer::Init()
{
    return pImpl_->GetMsgImageDataListServiceInterfaceServer()->Init();
}

void MsgImageDataListServiceInterfaceServer::Stop()
{
    pImpl_->GetMsgImageDataListServiceInterfaceServer()->Stop();
}

bool MsgImageDataListServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetMsgImageDataListServiceInterfaceServer()->IsStop();
}

uint32_t MsgImageDataListServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetMsgImageDataListServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool MsgImageDataListServiceInterfaceServer::SendEventImageListData(const SendEventImageListType& data)
{
    return pImpl_->GetMsgImageDataListServiceInterfaceServer()->SendEventImageListData(data);
}
void MsgImageDataListServiceInterfaceServer::ClearEventImageListContainer()
{
    pImpl_->GetMsgImageDataListServiceInterfaceServer()->ClearEventImageListContainer();
}

} /* namespace adsfi */
} /* namespace ara */
