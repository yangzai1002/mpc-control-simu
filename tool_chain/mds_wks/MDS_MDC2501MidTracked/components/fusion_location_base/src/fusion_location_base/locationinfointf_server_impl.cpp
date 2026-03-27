/* *
 * CLASS: LocationInfoIntf server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/fusion_location_base/locationinfointf_server_impl.h"

namespace adsfi {
LocationInfoIntfServerImpl::LocationInfoIntfServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : LocationInfoIntfSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

LocationInfoIntfServerImpl::~LocationInfoIntfServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool LocationInfoIntfServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void LocationInfoIntfServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void LocationInfoIntfServerImpl::LocationInfoNotify()
{
    sendLocationInfoCv_.notify_one();
    return;
}

bool LocationInfoIntfServerImpl::SendLocationInfoData(const SendLocationInfoType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->LocationInfo.Allocate();
    *val = *data;
    this->LocationInfo.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void LocationInfoIntfServerImpl::TransferLocationInfoData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendLocationInfoMtx_);
        std::cv_status status = sendLocationInfoCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *locationInfoContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->LocationInfo.Allocate();
        *val = *data;
        this->LocationInfo.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void LocationInfoIntfServerImpl::ClearLocationInfoContainer()
{
    std::lock_guard<std::mutex> lck(sendLocationInfoMtx_);
    locationInfoContainer_.Clear();
    return;
}

void LocationInfoIntfServerImpl::CreateLocationInfoThread()
{
    locationInfoThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&LocationInfoIntfServerImpl::TransferLocationInfoData, this));
}

} /* namespace adsfi */
