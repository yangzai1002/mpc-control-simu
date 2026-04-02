/* *
 * Class: SensorLidarPointCloudServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORLIDARPOINTCLOUDSERVICEINTERFACECLIENT
#define ARA_ADSFI_SENSORLIDARPOINTCLOUDSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msglidarframe.h"





namespace ara {
namespace adsfi {

using EventPointCloud2HandlerType = std::function<void (ara::adsfi::MsgLidarFrame)>;
using EventPointCloud2DataType = ara::adsfi::MsgLidarFrame;
using RecvEventPointCloud2Type = std::shared_ptr<EventPointCloud2DataType>;
class SensorLidarPointCloudServiceInterfaceClient final {
public:
    SensorLidarPointCloudServiceInterfaceClient() = delete;
    explicit SensorLidarPointCloudServiceInterfaceClient(const uint32_t instanceId);
    virtual ~SensorLidarPointCloudServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventPointCloud2NotifyHandler(const EventPointCloud2HandlerType handler);
    void EventPointCloud2Notify();
    void EventPointCloud2ContainerClear();
    bool EventPointCloud2ContainerEmpty();
    RecvEventPointCloud2Type GetEventPointCloud2OneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventPointCloud2Type GetEventPointCloud2OneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventPointCloud2Type> GetEventPointCloud2Ndata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif