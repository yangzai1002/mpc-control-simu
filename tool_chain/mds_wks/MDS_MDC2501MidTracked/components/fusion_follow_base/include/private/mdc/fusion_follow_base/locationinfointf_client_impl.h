/* *
 * Class: LocationInfoIntf client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ADSFI_LOCATIONINFOINTFCLIENT_IMPL
#define ADSFI_LOCATIONINFOINTFCLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "adsfi/locationinfointf_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/fusion_follow_base_logger.h"
namespace adsfi {
using LocationInfoIntfProxy = adsfi::proxy::LocationInfoIntfProxy;

using LocationInfoHandlerType = std::function<void (ara::adsfi::MsgHafLocation)>;
using LocationInfoDataType = ara::adsfi::MsgHafLocation;
using RecvLocationInfoType = std::shared_ptr<LocationInfoDataType>;
namespace {
using LOG_SPACE = mdc::fusion_follow_base::FusionFollowBaseLogger;
}

class LocationInfoIntfClientImpl {
public:
    explicit LocationInfoIntfClientImpl(const uint32_t instanceId);
    virtual ~LocationInfoIntfClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterLocationInfoNotifyHandler(const LocationInfoHandlerType handler);
    void LocationInfoNotify()
    {
        std::lock_guard<std::mutex> recvLk(LocationInfoMtx_);
        LocationInfoCv_.notify_all();
    }

    void LocationInfoContainerClear()
    {
        LocationInfoContainer_.Clear();
    }

    bool LocationInfoContainerEmpty()
    {
        return LocationInfoContainer_.Empty();
    }

    RecvLocationInfoType GetLocationInfoOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvLocationInfoType GetLocationInfoOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvLocationInfoType> GetLocationInfoNdata(const size_t n);
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

    void LocationInfoIntfCallback(ara::com::ServiceHandleContainer<LocationInfoIntfProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<LocationInfoIntfProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvLocationInfoType> LocationInfoContainer_;
    std::mutex LocationInfoMtx_;
    std::condition_variable LocationInfoCv_;
    void PushLocationInfoDataToContainer(LocationInfoDataType&& data);
};
} /* namespace adsfi */
#endif