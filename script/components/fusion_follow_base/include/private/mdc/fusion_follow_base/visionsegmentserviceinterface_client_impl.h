/* *
 * Class: VisionSegmentServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/visionsegmentserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/fusion_follow_base_logger.h"
namespace ara {
namespace adsfi {
using VisionSegmentServiceInterfaceProxy = ara::adsfi::proxy::VisionSegmentServiceInterfaceProxy;

using EventVisionSegmentHandlerType = std::function<void (ara::adsfi::MsgCameraVPResult)>;
using EventVisionSegmentDataType = ara::adsfi::MsgCameraVPResult;
using RecvEventVisionSegmentType = std::shared_ptr<EventVisionSegmentDataType>;
namespace {
using LOG_SPACE = mdc::fusion_follow_base::FusionFollowBaseLogger;
}

class VisionSegmentServiceInterfaceClientImpl {
public:
    explicit VisionSegmentServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~VisionSegmentServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventVisionSegmentNotifyHandler(const EventVisionSegmentHandlerType handler);
    void EventVisionSegmentNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventVisionSegmentMtx_);
        EventVisionSegmentCv_.notify_all();
    }

    void EventVisionSegmentContainerClear()
    {
        EventVisionSegmentContainer_.Clear();
    }

    bool EventVisionSegmentContainerEmpty()
    {
        return EventVisionSegmentContainer_.Empty();
    }

    RecvEventVisionSegmentType GetEventVisionSegmentOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventVisionSegmentType GetEventVisionSegmentOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventVisionSegmentType> GetEventVisionSegmentNdata(const size_t n);
    uint32_t GetRecvQSize() const
    {
        return recvQSize_;
    }

    bool IsStop() const
    {
        return !workFlag_;
    }

    uint32_t GetInstanceId() const
    {
        return instanceIdx_;
    }
    
private:
    /* 服务实例ID */
    uint32_t instanceIdx_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 寻找服务标识 */
    std::atomic<bool> findServFlag_;

    /* 注册标识 */
    std::atomic<bool> registerFlag_;

    /* EM模块 */
    ara::exec::ExecutionClient execClient_ {};

    /* EM 上报标识 */
    std::atomic<bool> emReport_;

    /* 服务发现回调 避免多线程同时执行标识 */
    std::once_flag callFlag_{};

    void VisionSegmentServiceInterfaceCallback(ara::com::ServiceHandleContainer<VisionSegmentServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<VisionSegmentServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventVisionSegmentType> EventVisionSegmentContainer_;
    std::mutex EventVisionSegmentMtx_;
    std::condition_variable EventVisionSegmentCv_;
    void PushEventVisionSegmentDataToContainer(EventVisionSegmentDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif