/* *
 * CLASS: VisionSegmentServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/fusion_follow_base/visionsegmentserviceinterface_client_impl.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"

namespace ara {
namespace adsfi {
VisionSegmentServiceInterfaceClientImpl::VisionSegmentServiceInterfaceClientImpl(const uint32_t instanceId)
    : instanceIdx_(instanceId),
      workFlag_(true),
      findServFlag_(false),
      registerFlag_(false),
      emReport_(false)
{}

VisionSegmentServiceInterfaceClientImpl::~VisionSegmentServiceInterfaceClientImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool VisionSegmentServiceInterfaceClientImpl::Init()
{
    if (findServFlag_) {
        return true;
    }
    ara::core::String insIdStr = std::to_string(instanceIdx_);
    serviceHandle_ = VisionSegmentServiceInterfaceProxy::StartFindService([this](
        ara::com::ServiceHandleContainer<VisionSegmentServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle) {
            VisionSegmentServiceInterfaceCallback(std::move(handles), findServiceHandle);
            },
        ara::com::InstanceIdentifier(ara::core::StringView(insIdStr)));
    (void)serviceHandle_;
    const uint32_t maxTimeoutCount = 10U;
    for (uint32_t timeoutCount = 0U; timeoutCount < maxTimeoutCount; timeoutCount++) {
        if (proxyPtr_ != nullptr) {
            findServFlag_ = true;
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100U));
    }
    LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Failed to find the service. instance id:" << insIdStr;
    VisionSegmentServiceInterfaceProxy::StopFindService(serviceHandle_);
    return false;
}

void VisionSegmentServiceInterfaceClientImpl::VisionSegmentServiceInterfaceCallback(
    ara::com::ServiceHandleContainer<VisionSegmentServiceInterfaceProxy::HandleType> handles,
    const ara::com::FindServiceHandle findServiceHandle)
{
    (void)findServiceHandle;
    if (proxyPtr_ != nullptr) {
        return;
    }

    for (auto singleHandle : handles) {
        ara::core::String insIdStr = std::to_string(instanceIdx_);
        if (singleHandle.GetInstanceId().ToString() == insIdStr) {
            /* 查找到指定服务实例后, 创建服务的Proxy实例 */
            std::call_once(callFlag_, [&singleHandle, this]() {
                this->proxyPtr_ = std::make_unique<VisionSegmentServiceInterfaceProxy>(singleHandle);
            });
            break;
        }
    }
    return;
}

void VisionSegmentServiceInterfaceClientImpl::Stop()
{
    workFlag_ = false;
    if (findServFlag_) {
        VisionSegmentServiceInterfaceProxy::StopFindService(serviceHandle_);
    }
    if (proxyPtr_ != nullptr) {
        if (proxyPtr_->EventVisionSegment.IsSubscribed()) {
            proxyPtr_->EventVisionSegment.Unsubscribe();
            proxyPtr_->EventVisionSegment.UnsetReceiveHandler();
        }
        proxyPtr_ = nullptr;
    }
    LOG_SPACE::GetLoggerIns("CLNT")->LogInfo() << "Data receive baes stoped. idx: " << instanceIdx_;
}

void VisionSegmentServiceInterfaceClientImpl::EmReportExec()
{
    const auto sleepInterval = std::chrono::milliseconds(5U);
    const uint8_t EM_REPORT_TIME_MAX = 3U;
    int count = 0;
    ara::core::Result<void> res;
    while (count < EM_REPORT_TIME_MAX) {
        res = execClient_.ReportExecutionState(ara::exec::ExecutionState::kRunning);
        if (res.HasValue()) {
            break;
        } else {
            std::this_thread::sleep_for(sleepInterval);
            count++;
        }
    }
    if (count == EM_REPORT_TIME_MAX) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "EM report kRunning failed.";
    }
    return;
}

void VisionSegmentServiceInterfaceClientImpl::RegisterEventVisionSegmentNotifyHandler(const EventVisionSegmentHandlerType handler)
{
    if (proxyPtr_ == nullptr) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Failed to find the service instance.";
        return;
    }
    if (!handler) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Can't register a null handler. Otherwise, data will not be received.";
        return;
    }
    if (registerFlag_) {
        return;
    }
    auto NotifyCallback = [this, handler]() {
        (void)proxyPtr_->EventVisionSegment.GetNewSamples([this, handler](
            const ara::com::SamplePtr<EventVisionSegmentDataType const> ptr) {
            const auto& notifyData = ptr.Get();
            auto data = *notifyData;
            handler(data);
            PushEventVisionSegmentDataToContainer(std::move(data));
            LOG_SPACE::GetLoggerIns("CLNT")->LogInfo() << "EventVisionSegment: data received. instanceId: " << instanceIdx_;
        });
    };
    proxyPtr_->EventVisionSegment.SetReceiveHandler([NotifyCallback]() { NotifyCallback(); });
    proxyPtr_->EventVisionSegment.Subscribe(recvQSize_);
    EmReportExec();
    registerFlag_ = true;
    return;
}

void VisionSegmentServiceInterfaceClientImpl::PushEventVisionSegmentDataToContainer(EventVisionSegmentDataType&& data)
{
    RecvEventVisionSegmentType ptr = std::make_shared<EventVisionSegmentDataType>(std::forward<EventVisionSegmentDataType>(data));
    EventVisionSegmentContainer_.Push(std::move(ptr));
    EventVisionSegmentCv_.notify_all();
    return;
}

RecvEventVisionSegmentType VisionSegmentServiceInterfaceClientImpl::GetEventVisionSegmentOneData(const uint32_t freshDataTime)
{
    if (!workFlag_) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Data receive got stopFlag signal.";
        return nullptr;
    }
    auto dataPtr = EventVisionSegmentContainer_.GetOneData(freshDataTime);
    if (dataPtr == nullptr) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Get data is nullptr.";
        return nullptr;
    }
    LOG_SPACE::GetLoggerIns("CLNT")->LogInfo() << "Get one data successfully.";
    return *dataPtr;
}

RecvEventVisionSegmentType VisionSegmentServiceInterfaceClientImpl::GetEventVisionSegmentOneDataBlocking(const uint32_t blockTimeout)
{
    std::unique_lock<std::mutex> recvLk(EventVisionSegmentMtx_);
    if (EventVisionSegmentCv_.wait_for(recvLk, std::chrono::milliseconds(blockTimeout)) == std::cv_status::timeout) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "No new data arrived since this api called. timeout(ms): " <<
            blockTimeout;
        return nullptr;
    }
    if (!workFlag_) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogWarn() << "Data receive got stopFlag.";
        return nullptr;
    }
    auto dataPtr = EventVisionSegmentContainer_.GetOneData();
    recvLk.unlock();
    if (dataPtr == nullptr) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Get one data in block mode is nullptr.";
        return nullptr;
    }
    LOG_SPACE::GetLoggerIns("CLNT")->LogInfo() << "Get one data in block mode successfully.";
    return *dataPtr;
}

std::vector<RecvEventVisionSegmentType> VisionSegmentServiceInterfaceClientImpl::GetEventVisionSegmentNdata(const size_t n)
{
    if (!workFlag_) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogWarn() << "Data receive got stopFlag";
        return {};
    }

    auto result = EventVisionSegmentContainer_.GetNdata(n);

    std::vector<RecvEventVisionSegmentType> output;
    output.reserve(result.size());

    for (auto& it : result) {
        if (it != nullptr) {
            output.push_back(*it);
        }
    }
    if (output.size() < n) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogWarn()
            << "Failed to obtain " << n << " data records, because the length of the received data is "<< output.size();
        return output;
    }
    LOG_SPACE::GetLoggerIns("CLNT")->LogInfo() << "Get " << n << " data successfully.";
    return output;
}
} /* namespace adsfi */
} /* namespace ara */
