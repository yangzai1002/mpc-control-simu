/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_HWFR671RADARSWC_H
#define MDC_HWFR671RADARSWC_H

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

namespace ara {
namespace adsfi {
class RadarPointCloudServiceInterfaceServer;
}
}
namespace ara {
namespace adsfi {
class SensorRadarPacketServiceInterfaceServer;
}
}
namespace mdc {
namespace location {
class LocationServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class VehicleInfomationServiceInterfaceClient;
}
}
namespace mdc {
namespace hw_fr671_radar {
namespace {
using ara::adsfi::RadarPointCloudServiceInterfaceServer;
using ara::adsfi::SensorRadarPacketServiceInterfaceServer;
using mdc::location::LocationServiceInterfaceClient;
using ara::adsfi::VehicleInfomationServiceInterfaceClient;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class HwFr671RadarSwC {
public:
    explicit HwFr671RadarSwC();

    virtual ~HwFr671RadarSwC();

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

    ara::core::Vector<std::string> GetRadarPointCloudServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(radarPointCloudServiceInterfaceServerPortVecMtx_);
        return radarPointCloudServiceInterfaceServerPortVec_;
    }
    ara::core::Vector<std::string> GetSensorRadarPacketServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(sensorRadarPacketServiceInterfaceServerPortVecMtx_);
        return sensorRadarPacketServiceInterfaceServerPortVec_;
    }
    ara::core::Vector<std::string> GetLocationServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(locationServiceInterfaceClientPortVecMtx_);
        return locationServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetVehicleInfomationServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(vehicleInfomationServiceInterfaceClientPortVecMtx_);
        return vehicleInfomationServiceInterfaceClientPortVec_;
    }
    

    std::shared_ptr<RadarPointCloudServiceInterfaceServer> GetRadarPointCloudServiceInterfaceServer(const ara::core::String& portName);
    std::shared_ptr<SensorRadarPacketServiceInterfaceServer> GetSensorRadarPacketServiceInterfaceServer(const ara::core::String& portName);
    std::shared_ptr<LocationServiceInterfaceClient> GetLocationServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<VehicleInfomationServiceInterfaceClient> GetVehicleInfomationServiceInterfaceClient(const ara::core::String& portName);
    

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
    ara::core::Vector<uint32_t> radarPointCloudServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> radarPointCloudServiceInterfaceServerPortVec_;
    std::mutex radarPointCloudServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> radarPointCloudServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<RadarPointCloudServiceInterfaceServer>> radarPointCloudServiceInterfaceServerInsMap_;
    std::mutex radarPointCloudServiceInterfaceServerInsMapMtx_;
    /* server agent */
    ara::core::Vector<uint32_t> sensorRadarPacketServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> sensorRadarPacketServiceInterfaceServerPortVec_;
    std::mutex sensorRadarPacketServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorRadarPacketServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorRadarPacketServiceInterfaceServer>> sensorRadarPacketServiceInterfaceServerInsMap_;
    std::mutex sensorRadarPacketServiceInterfaceServerInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> locationServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> locationServiceInterfaceClientPortVec_;
    std::mutex locationServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> locationServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<LocationServiceInterfaceClient>> locationServiceInterfaceClientInsMap_;
    std::mutex locationServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> vehicleInfomationServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> vehicleInfomationServiceInterfaceClientPortVec_;
    std::mutex vehicleInfomationServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> vehicleInfomationServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<VehicleInfomationServiceInterfaceClient>> vehicleInfomationServiceInterfaceClientInsMap_;
    std::mutex vehicleInfomationServiceInterfaceClientInsMapMtx_;

};
}  /* namespace hw_fr671_radar */
}  /* namespace mdc */

#endif  /* MDC_HWFR671RADARSWC_H */