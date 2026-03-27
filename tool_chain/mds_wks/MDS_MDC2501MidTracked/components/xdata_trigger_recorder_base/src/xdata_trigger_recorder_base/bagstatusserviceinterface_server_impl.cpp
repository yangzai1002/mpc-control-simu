/* *
 * CLASS: BagStatusServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xdata_trigger_recorder_base/bagstatusserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
BagStatusServiceInterfaceServerImpl::BagStatusServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : BagStatusServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

BagStatusServiceInterfaceServerImpl::~BagStatusServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool BagStatusServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void BagStatusServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void BagStatusServiceInterfaceServerImpl::EventBagStatusNotify()
{
    sendEventBagStatusCv_.notify_one();
    return;
}

bool BagStatusServiceInterfaceServerImpl::SendEventBagStatusData(const SendEventBagStatusType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventBagStatus.Allocate();
    *val = *data;
    this->EventBagStatus.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void BagStatusServiceInterfaceServerImpl::TransferEventBagStatusData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventBagStatusMtx_);
        std::cv_status status = sendEventBagStatusCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventBagStatusContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventBagStatus.Allocate();
        *val = *data;
        this->EventBagStatus.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void BagStatusServiceInterfaceServerImpl::ClearEventBagStatusContainer()
{
    std::lock_guard<std::mutex> lck(sendEventBagStatusMtx_);
    eventBagStatusContainer_.Clear();
    return;
}

void BagStatusServiceInterfaceServerImpl::CreateEventBagStatusThread()
{
    eventBagStatusThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&BagStatusServiceInterfaceServerImpl::TransferEventBagStatusData, this));
}

} /* namespace adsfi */
} /* namespace ara */
