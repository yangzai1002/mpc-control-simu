/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_BD21IMUSWC_H
#define MDC_BD21IMUSWC_H

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

namespace mdc {
namespace location {
class LocationServiceInterfaceServer;
}
}
namespace ara {
namespace adsfi {
class SensorWheelSpeedServiceInterfaceServer;
}
}
namespace ara {
namespace adsfi {
class SensorGpsServiceInterfaceServer;
}
}
namespace ara {
namespace adsfi {
class SensorImuServiceInterfaceServer;
}
}
namespace mdc {
namespace bd21_imu {
namespace {
using mdc::location::LocationServiceInterfaceServer;
using ara::adsfi::SensorWheelSpeedServiceInterfaceServer;
using ara::adsfi::SensorGpsServiceInterfaceServer;
using ara::adsfi::SensorImuServiceInterfaceServer;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class Bd21ImuSwC {
public:
    explicit Bd21ImuSwC();

    virtual ~Bd21ImuSwC();

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

    ara::core::Vector<std::string> GetLocationServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(locationServiceInterfaceServerPortVecMtx_);
        return locationServiceInterfaceServerPortVec_;
    }
    ara::core::Vector<std::string> GetSensorWheelSpeedServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(sensorWheelSpeedServiceInterfaceServerPortVecMtx_);
        return sensorWheelSpeedServiceInterfaceServerPortVec_;
    }
    ara::core::Vector<std::string> GetSensorGpsServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(sensorGpsServiceInterfaceServerPortVecMtx_);
        return sensorGpsServiceInterfaceServerPortVec_;
    }
    ara::core::Vector<std::string> GetSensorImuServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(sensorImuServiceInterfaceServerPortVecMtx_);
        return sensorImuServiceInterfaceServerPortVec_;
    }
    

    std::shared_ptr<LocationServiceInterfaceServer> GetLocationServiceInterfaceServer(const ara::core::String& portName);
    std::shared_ptr<SensorWheelSpeedServiceInterfaceServer> GetSensorWheelSpeedServiceInterfaceServer(const ara::core::String& portName);
    std::shared_ptr<SensorGpsServiceInterfaceServer> GetSensorGpsServiceInterfaceServer(const ara::core::String& portName);
    std::shared_ptr<SensorImuServiceInterfaceServer> GetSensorImuServiceInterfaceServer(const ara::core::String& portName);
    

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
    ara::core::Vector<uint32_t> locationServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> locationServiceInterfaceServerPortVec_;
    std::mutex locationServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> locationServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<LocationServiceInterfaceServer>> locationServiceInterfaceServerInsMap_;
    std::mutex locationServiceInterfaceServerInsMapMtx_;
    /* server agent */
    ara::core::Vector<uint32_t> sensorWheelSpeedServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> sensorWheelSpeedServiceInterfaceServerPortVec_;
    std::mutex sensorWheelSpeedServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorWheelSpeedServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorWheelSpeedServiceInterfaceServer>> sensorWheelSpeedServiceInterfaceServerInsMap_;
    std::mutex sensorWheelSpeedServiceInterfaceServerInsMapMtx_;
    /* server agent */
    ara::core::Vector<uint32_t> sensorGpsServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> sensorGpsServiceInterfaceServerPortVec_;
    std::mutex sensorGpsServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorGpsServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorGpsServiceInterfaceServer>> sensorGpsServiceInterfaceServerInsMap_;
    std::mutex sensorGpsServiceInterfaceServerInsMapMtx_;
    /* server agent */
    ara::core::Vector<uint32_t> sensorImuServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> sensorImuServiceInterfaceServerPortVec_;
    std::mutex sensorImuServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorImuServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorImuServiceInterfaceServer>> sensorImuServiceInterfaceServerInsMap_;
    std::mutex sensorImuServiceInterfaceServerInsMapMtx_;

};
}  /* namespace bd21_imu */
}  /* namespace mdc */

#endif  /* MDC_BD21IMUSWC_H */