/* *
 * CLASS: RemoteControlServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xzmq_rc/remotecontrolserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
RemoteControlServiceInterfaceServerImpl::RemoteControlServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : RemoteControlServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

RemoteControlServiceInterfaceServerImpl::~RemoteControlServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool RemoteControlServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void RemoteControlServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void RemoteControlServiceInterfaceServerImpl::EventRCAppRemoteCtlNotify()
{
    sendEventRCAppRemoteCtlCv_.notify_one();
    return;
}

bool RemoteControlServiceInterfaceServerImpl::SendEventRCAppRemoteCtlData(const SendEventRCAppRemoteCtlType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventRCAppRemoteCtl.Allocate();
    *val = *data;
    this->EventRCAppRemoteCtl.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void RemoteControlServiceInterfaceServerImpl::TransferEventRCAppRemoteCtlData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventRCAppRemoteCtlMtx_);
        std::cv_status status = sendEventRCAppRemoteCtlCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventRCAppRemoteCtlContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventRCAppRemoteCtl.Allocate();
        *val = *data;
        this->EventRCAppRemoteCtl.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void RemoteControlServiceInterfaceServerImpl::ClearEventRCAppRemoteCtlContainer()
{
    std::lock_guard<std::mutex> lck(sendEventRCAppRemoteCtlMtx_);
    eventRCAppRemoteCtlContainer_.Clear();
    return;
}

void RemoteControlServiceInterfaceServerImpl::CreateEventRCAppRemoteCtlThread()
{
    eventRCAppRemoteCtlThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&RemoteControlServiceInterfaceServerImpl::TransferEventRCAppRemoteCtlData, this));
}

} /* namespace adsfi */
} /* namespace ara */
