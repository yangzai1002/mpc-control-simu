/* *
 * CLASS: LocationServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/bd21_imu/locationserviceinterface_server_impl.h"

namespace mdc {
namespace location {
LocationServiceInterfaceServerImpl::LocationServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : LocationServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

LocationServiceInterfaceServerImpl::~LocationServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool LocationServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void LocationServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void LocationServiceInterfaceServerImpl::LocationEventNotify()
{
    sendLocationEventCv_.notify_one();
    return;
}

bool LocationServiceInterfaceServerImpl::SendLocationEventData(const SendLocationEventType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->LocationEvent.Allocate();
    *val = *data;
    this->LocationEvent.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void LocationServiceInterfaceServerImpl::TransferLocationEventData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendLocationEventMtx_);
        std::cv_status status = sendLocationEventCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *locationEventContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->LocationEvent.Allocate();
        *val = *data;
        this->LocationEvent.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void LocationServiceInterfaceServerImpl::ClearLocationEventContainer()
{
    std::lock_guard<std::mutex> lck(sendLocationEventMtx_);
    locationEventContainer_.Clear();
    return;
}

void LocationServiceInterfaceServerImpl::CreateLocationEventThread()
{
    locationEventThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&LocationServiceInterfaceServerImpl::TransferLocationEventData, this));
}

} /* namespace location */
} /* namespace mdc */
