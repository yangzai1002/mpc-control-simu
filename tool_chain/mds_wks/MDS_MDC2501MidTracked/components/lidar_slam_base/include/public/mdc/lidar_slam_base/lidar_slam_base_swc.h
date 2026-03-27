/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_LIDARSLAMBASESWC_H
#define MDC_LIDARSLAMBASESWC_H

#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <atomic>
#include <mutex>
#include <sys/types.h>

#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "ara/core/map.h"

namespace mdc {
class MdcYamlNode;
}

namespace adsfi {
class LocationInfoIntfServer;
}
namespace ara {
namespace adsfi {
class SensorLidarPointCloudServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class SensorImuServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class SensorWheelSpeedServiceInterfaceClient;
}
}
namespace mdc {
namespace lidar_slam_base {
namespace {
using adsfi::LocationInfoIntfServer;
using ara::adsfi::SensorLidarPointCloudServiceInterfaceClient;
using ara::adsfi::SensorImuServiceInterfaceClient;
using ara::adsfi::SensorWheelSpeedServiceInterfaceClient;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class LidarSlamBaseSwC {
public:
    explicit LidarSlamBaseSwC();

    virtual ~LidarSlamBaseSwC();

    bool Init();

    void Stop();

    inline void SetInitCallback(InitHandleType handler)
    {
        initHandler_ = handler;
    }

    inline void SetStopCallback(StopHandleType handler)
    {
        stopHandler_ = handler;
    }

    inline bool IsStop() const
    {
        return !workFlag_;
    }

    ara::core::Vector<std::string> GetLocationInfoIntfServerVec()
    {
        std::lock_guard<std::mutex> lck(locationInfoIntfServerPortVecMtx_);
        return locationInfoIntfServerPortVec_;
    }
    ara::core::Vector<std::string> GetSensorLidarPointCloudServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(sensorLidarPointCloudServiceInterfaceClientPortVecMtx_);
        return sensorLidarPointCloudServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetSensorImuServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(sensorImuServiceInterfaceClientPortVecMtx_);
        return sensorImuServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetSensorWheelSpeedServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(sensorWheelSpeedServiceInterfaceClientPortVecMtx_);
        return sensorWheelSpeedServiceInterfaceClientPortVec_;
    }
    

    std::shared_ptr<LocationInfoIntfServer> GetLocationInfoIntfServer(const ara::core::String& portName);
    std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient> GetSensorLidarPointCloudServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<SensorImuServiceInterfaceClient> GetSensorImuServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<SensorWheelSpeedServiceInterfaceClient> GetSensorWheelSpeedServiceInterfaceClient(const ara::core::String& portName);
    

private:
    /**
     * @brief 加载instanceID等初始化配置
     *
     */
    bool LoadConfig();

    /**
     * @brief 解析swc管理的interface的instanceID信息
     *
     */
    bool ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config);

    /**
     * @brief 判断文件是否存在
     *
     */
    bool DoesFileExist(const ara::core::String& filePath);
    bool IsDirType(const mode_t& fileMode);

private:
    /* 初始化标识, 防止多次初始化 */
    std::atomic<bool> initFlag_;

    /* 工作标识 */
    std::atomic<bool> workFlag_;

    /* init回调函数 */
    InitHandleType initHandler_{nullptr};

    /* stop回调函数 */
    StopHandleType stopHandler_{nullptr};

    /* 参数文件路径 */
    ara::core::String configFile_{};

    /* swc标识符 */
    ara::core::String identifier_{};

    /* logger描述符 */
    ara::core::String loggerId_{};
    /* server agent */
    ara::core::Vector<uint32_t> locationInfoIntfServerIdVec_;
    ara::core::Vector<std::string> locationInfoIntfServerPortVec_;
    std::mutex locationInfoIntfServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> locationInfoIntfServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<LocationInfoIntfServer>> locationInfoIntfServerInsMap_;
    std::mutex locationInfoIntfServerInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> sensorLidarPointCloudServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> sensorLidarPointCloudServiceInterfaceClientPortVec_;
    std::mutex sensorLidarPointCloudServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorLidarPointCloudServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient>> sensorLidarPointCloudServiceInterfaceClientInsMap_;
    std::mutex sensorLidarPointCloudServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> sensorImuServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> sensorImuServiceInterfaceClientPortVec_;
    std::mutex sensorImuServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorImuServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorImuServiceInterfaceClient>> sensorImuServiceInterfaceClientInsMap_;
    std::mutex sensorImuServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> sensorWheelSpeedServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> sensorWheelSpeedServiceInterfaceClientPortVec_;
    std::mutex sensorWheelSpeedServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorWheelSpeedServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorWheelSpeedServiceInterfaceClient>> sensorWheelSpeedServiceInterfaceClientInsMap_;
    std::mutex sensorWheelSpeedServiceInterfaceClientInsMapMtx_;

};
}  /* namespace lidar_slam_base */
}  /* namespace mdc */

#endif  /* MDC_LIDARSLAMBASESWC_H */