/* *
 * CLASS: FusionObjectArrayIntf server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/fusion_follow_base/fusionobjectarrayintf_server_impl.h"

namespace adsfi {
FusionObjectArrayIntfServerImpl::FusionObjectArrayIntfServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : FusionObjectArrayIntfSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

FusionObjectArrayIntfServerImpl::~FusionObjectArrayIntfServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool FusionObjectArrayIntfServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void FusionObjectArrayIntfServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void FusionObjectArrayIntfServerImpl::FusionObjectArrayNotify()
{
    sendFusionObjectArrayCv_.notify_one();
    return;
}

bool FusionObjectArrayIntfServerImpl::SendFusionObjectArrayData(const SendFusionObjectArrayType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->FusionObjectArray.Allocate();
    *val = *data;
    this->FusionObjectArray.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void FusionObjectArrayIntfServerImpl::TransferFusionObjectArrayData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendFusionObjectArrayMtx_);
        std::cv_status status = sendFusionObjectArrayCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *fusionObjectArrayContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->FusionObjectArray.Allocate();
        *val = *data;
        this->FusionObjectArray.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void FusionObjectArrayIntfServerImpl::ClearFusionObjectArrayContainer()
{
    std::lock_guard<std::mutex> lck(sendFusionObjectArrayMtx_);
    fusionObjectArrayContainer_.Clear();
    return;
}

void FusionObjectArrayIntfServerImpl::CreateFusionObjectArrayThread()
{
    fusionObjectArrayThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&FusionObjectArrayIntfServerImpl::TransferFusionObjectArrayData, this));
}

} /* namespace adsfi */
