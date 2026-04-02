/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_LIDARDETBASESWC_H
#define MDC_LIDARDETBASESWC_H

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
class Object3dArrayIntfServer;
}
namespace ara {
namespace adsfi {
class SensorLidarPointCloudServiceInterfaceClient;
}
}
namespace mdc {
namespace lidar_det_base {
namespace {
using adsfi::Object3dArrayIntfServer;
using ara::adsfi::SensorLidarPointCloudServiceInterfaceClient;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class LidarDetBaseSwC {
public:
    explicit LidarDetBaseSwC();

    virtual ~LidarDetBaseSwC();

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

    ara::core::Vector<std::string> GetObject3dArrayIntfServerVec()
    {
        std::lock_guard<std::mutex> lck(object3dArrayIntfServerPortVecMtx_);
        return object3dArrayIntfServerPortVec_;
    }
    ara::core::Vector<std::string> GetSensorLidarPointCloudServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(sensorLidarPointCloudServiceInterfaceClientPortVecMtx_);
        return sensorLidarPointCloudServiceInterfaceClientPortVec_;
    }
    

    std::shared_ptr<Object3dArrayIntfServer> GetObject3dArrayIntfServer(const ara::core::String& portName);
    std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient> GetSensorLidarPointCloudServiceInterfaceClient(const ara::core::String& portName);
    

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
    ara::core::Vector<uint32_t> object3dArrayIntfServerIdVec_;
    ara::core::Vector<std::string> object3dArrayIntfServerPortVec_;
    std::mutex object3dArrayIntfServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> object3dArrayIntfServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<Object3dArrayIntfServer>> object3dArrayIntfServerInsMap_;
    std::mutex object3dArrayIntfServerInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> sensorLidarPointCloudServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> sensorLidarPointCloudServiceInterfaceClientPortVec_;
    std::mutex sensorLidarPointCloudServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorLidarPointCloudServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient>> sensorLidarPointCloudServiceInterfaceClientInsMap_;
    std::mutex sensorLidarPointCloudServiceInterfaceClientInsMapMtx_;

};
}  /* namespace lidar_det_base */
}  /* namespace mdc */

#endif  /* MDC_LIDARDETBASESWC_H */