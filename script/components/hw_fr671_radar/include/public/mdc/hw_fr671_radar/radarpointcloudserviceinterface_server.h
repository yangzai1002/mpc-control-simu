/* *
 * Class: RadarPointCloudServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_RADARPOINTCLOUDSERVICEINTERFACESERVER
#define ARA_ADSFI_RADARPOINTCLOUDSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_sensorradarpointcloud.h"





namespace ara {
namespace adsfi {

using EventRadarPointCloudDataType = ara::adsfi::SensorRadarPointCloud;
using SendEventRadarPointCloudType = std::shared_ptr<EventRadarPointCloudDataType>;


class RadarPointCloudServiceInterfaceServer final {
public:
    RadarPointCloudServiceInterfaceServer() = delete;

    explicit RadarPointCloudServiceInterfaceServer(const uint32_t id);

    virtual ~RadarPointCloudServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventRadarPointCloudData(const SendEventRadarPointCloudType& data);
    void ClearEventRadarPointCloudContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif