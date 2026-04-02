/* *
 * Class: RadarPointCloudServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_RADARPOINTCLOUDSERVICEINTERFACECLIENT
#define ARA_ADSFI_RADARPOINTCLOUDSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_sensorradarpointcloud.h"





namespace ara {
namespace adsfi {

using EventRadarPointCloudHandlerType = std::function<void (ara::adsfi::SensorRadarPointCloud)>;
using EventRadarPointCloudDataType = ara::adsfi::SensorRadarPointCloud;
using RecvEventRadarPointCloudType = std::shared_ptr<EventRadarPointCloudDataType>;
class RadarPointCloudServiceInterfaceClient final {
public:
    RadarPointCloudServiceInterfaceClient() = delete;
    explicit RadarPointCloudServiceInterfaceClient(const uint32_t instanceId);
    virtual ~RadarPointCloudServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventRadarPointCloudNotifyHandler(const EventRadarPointCloudHandlerType handler);
    void EventRadarPointCloudNotify();
    void EventRadarPointCloudContainerClear();
    bool EventRadarPointCloudContainerEmpty();
    RecvEventRadarPointCloudType GetEventRadarPointCloudOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventRadarPointCloudType GetEventRadarPointCloudOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventRadarPointCloudType> GetEventRadarPointCloudNdata(const size_t n);

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