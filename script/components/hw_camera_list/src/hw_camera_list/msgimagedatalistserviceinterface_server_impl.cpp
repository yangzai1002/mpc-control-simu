/* *
 * CLASS: MsgImageDataListServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_camera_list/msgimagedatalistserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
MsgImageDataListServiceInterfaceServerImpl::MsgImageDataListServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : MsgImageDataListServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

MsgImageDataListServiceInterfaceServerImpl::~MsgImageDataListServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool MsgImageDataListServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void MsgImageDataListServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void MsgImageDataListServiceInterfaceServerImpl::EventImageListNotify()
{
    sendEventImageListCv_.notify_one();
    return;
}

bool MsgImageDataListServiceInterfaceServerImpl::SendEventImageListData(const SendEventImageListType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventImageList.Allocate();
    *val = *data;
    this->EventImageList.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void MsgImageDataListServiceInterfaceServerImpl::TransferEventImageListData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventImageListMtx_);
        std::cv_status status = sendEventImageListCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventImageListContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventImageList.Allocate();
        *val = *data;
        this->EventImageList.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void MsgImageDataListServiceInterfaceServerImpl::ClearEventImageListContainer()
{
    std::lock_guard<std::mutex> lck(sendEventImageListMtx_);
    eventImageListContainer_.Clear();
    return;
}

void MsgImageDataListServiceInterfaceServerImpl::CreateEventImageListThread()
{
    eventImageListThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&MsgImageDataListServiceInterfaceServerImpl::TransferEventImageListData, this));
}

} /* namespace adsfi */
} /* namespace ara */
