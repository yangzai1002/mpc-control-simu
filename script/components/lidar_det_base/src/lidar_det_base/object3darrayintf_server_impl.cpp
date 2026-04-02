/* *
 * CLASS: Object3dArrayIntf server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/lidar_det_base/object3darrayintf_server_impl.h"

namespace adsfi {
Object3dArrayIntfServerImpl::Object3dArrayIntfServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : Object3dArrayIntfSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

Object3dArrayIntfServerImpl::~Object3dArrayIntfServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool Object3dArrayIntfServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void Object3dArrayIntfServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void Object3dArrayIntfServerImpl::EventPerceptionLidarObjectsNotify()
{
    sendEventPerceptionLidarObjectsCv_.notify_one();
    return;
}

bool Object3dArrayIntfServerImpl::SendEventPerceptionLidarObjectsData(const SendEventPerceptionLidarObjectsType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventPerceptionLidarObjects.Allocate();
    *val = *data;
    this->EventPerceptionLidarObjects.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void Object3dArrayIntfServerImpl::TransferEventPerceptionLidarObjectsData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventPerceptionLidarObjectsMtx_);
        std::cv_status status = sendEventPerceptionLidarObjectsCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventPerceptionLidarObjectsContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventPerceptionLidarObjects.Allocate();
        *val = *data;
        this->EventPerceptionLidarObjects.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void Object3dArrayIntfServerImpl::ClearEventPerceptionLidarObjectsContainer()
{
    std::lock_guard<std::mutex> lck(sendEventPerceptionLidarObjectsMtx_);
    eventPerceptionLidarObjectsContainer_.Clear();
    return;
}

void Object3dArrayIntfServerImpl::CreateEventPerceptionLidarObjectsThread()
{
    eventPerceptionLidarObjectsThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&Object3dArrayIntfServerImpl::TransferEventPerceptionLidarObjectsData, this));
}

} /* namespace adsfi */
