/* *
 * Class: MsgImageDataListServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_MSGIMAGEDATALISTSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_MSGIMAGEDATALISTSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/msgimagedatalistserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/xvehicle_status_recorder_base_logger.h"
namespace ara {
namespace adsfi {
using MsgImageDataListServiceInterfaceProxy = ara::adsfi::proxy::MsgImageDataListServiceInterfaceProxy;

using EventImageListHandlerType = std::function<void (ara::adsfi::MsgImageDataList)>;
using EventImageListDataType = ara::adsfi::MsgImageDataList;
using RecvEventImageListType = std::shared_ptr<EventImageListDataType>;
namespace {
using LOG_SPACE = mdc::xvehicle_status_recorder_base::XvehicleStatusRecorderBaseLogger;
}

class MsgImageDataListServiceInterfaceClientImpl {
public:
    explicit MsgImageDataListServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~MsgImageDataListServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventImageListNotifyHandler(const EventImageListHandlerType handler);
    void EventImageListNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventImageListMtx_);
        EventImageListCv_.notify_all();
    }

    void EventImageListContainerClear()
    {
        EventImageListContainer_.Clear();
    }

    bool EventImageListContainerEmpty()
    {
        return EventImageListContainer_.Empty();
    }

    RecvEventImageListType GetEventImageListOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventImageListType GetEventImageListOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventImageListType> GetEventImageListNdata(const size_t n);
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

    void MsgImageDataListServiceInterfaceCallback(ara::com::ServiceHandleContainer<MsgImageDataListServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<MsgImageDataListServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventImageListType> EventImageListContainer_;
    std::mutex EventImageListMtx_;
    std::condition_variable EventImageListCv_;
    void PushEventImageListDataToContainer(EventImageListDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif