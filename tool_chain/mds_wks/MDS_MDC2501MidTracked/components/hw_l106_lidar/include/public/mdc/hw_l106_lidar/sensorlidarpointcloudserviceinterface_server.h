/* *
 * Class: SensorLidarPointCloudServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORLIDARPOINTCLOUDSERVICEINTERFACESERVER
#define ARA_ADSFI_SENSORLIDARPOINTCLOUDSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_msglidarframe.h"





namespace ara {
namespace adsfi {

using EventPointCloud2DataType = ara::adsfi::MsgLidarFrame;
using SendEventPointCloud2Type = std::shared_ptr<EventPointCloud2DataType>;


class SensorLidarPointCloudServiceInterfaceServer final {
public:
    SensorLidarPointCloudServiceInterfaceServer() = delete;

    explicit SensorLidarPointCloudServiceInterfaceServer(const uint32_t id);

    virtual ~SensorLidarPointCloudServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventPointCloud2Data(const SendEventPointCloud2Type& data);
    void ClearEventPointCloud2Container();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif