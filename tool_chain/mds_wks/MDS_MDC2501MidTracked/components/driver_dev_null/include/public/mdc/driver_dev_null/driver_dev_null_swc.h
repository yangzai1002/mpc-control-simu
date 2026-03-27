/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_DRIVERDEVNULLSWC_H
#define MDC_DRIVERDEVNULLSWC_H

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
class SensorCompressedImageListServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class SensorRadarPacketServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class SensorLidarPacketServiceInterfaceClient;
}
}
namespace mdc {
namespace driver_dev_null {
namespace {
using ara::adsfi::SensorCompressedImageListServiceInterfaceClient;
using ara::adsfi::SensorRadarPacketServiceInterfaceClient;
using ara::adsfi::SensorLidarPacketServiceInterfaceClient;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class DriverDevNullSwC {
public:
    explicit DriverDevNullSwC();

    virtual ~DriverDevNullSwC();

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

    ara::core::Vector<std::string> GetSensorCompressedImageListServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(sensorCompressedImageListServiceInterfaceClientPortVecMtx_);
        return sensorCompressedImageListServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetSensorRadarPacketServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(sensorRadarPacketServiceInterfaceClientPortVecMtx_);
        return sensorRadarPacketServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetSensorLidarPacketServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(sensorLidarPacketServiceInterfaceClientPortVecMtx_);
        return sensorLidarPacketServiceInterfaceClientPortVec_;
    }
    

    std::shared_ptr<SensorCompressedImageListServiceInterfaceClient> GetSensorCompressedImageListServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<SensorRadarPacketServiceInterfaceClient> GetSensorRadarPacketServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<SensorLidarPacketServiceInterfaceClient> GetSensorLidarPacketServiceInterfaceClient(const ara::core::String& portName);
    

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
    /* client agent */
    ara::core::Vector<uint32_t> sensorCompressedImageListServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> sensorCompressedImageListServiceInterfaceClientPortVec_;
    std::mutex sensorCompressedImageListServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorCompressedImageListServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorCompressedImageListServiceInterfaceClient>> sensorCompressedImageListServiceInterfaceClientInsMap_;
    std::mutex sensorCompressedImageListServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> sensorRadarPacketServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> sensorRadarPacketServiceInterfaceClientPortVec_;
    std::mutex sensorRadarPacketServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorRadarPacketServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorRadarPacketServiceInterfaceClient>> sensorRadarPacketServiceInterfaceClientInsMap_;
    std::mutex sensorRadarPacketServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> sensorLidarPacketServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> sensorLidarPacketServiceInterfaceClientPortVec_;
    std::mutex sensorLidarPacketServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorLidarPacketServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorLidarPacketServiceInterfaceClient>> sensorLidarPacketServiceInterfaceClientInsMap_;
    std::mutex sensorLidarPacketServiceInterfaceClientInsMapMtx_;

};
}  /* namespace driver_dev_null */
}  /* namespace mdc */

#endif  /* MDC_DRIVERDEVNULLSWC_H */