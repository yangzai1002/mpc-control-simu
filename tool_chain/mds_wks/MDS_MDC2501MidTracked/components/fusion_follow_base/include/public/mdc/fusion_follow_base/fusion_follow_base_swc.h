/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_FUSIONFOLLOWBASESWC_H
#define MDC_FUSIONFOLLOWBASESWC_H

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
class FusionObjectArrayIntfServer;
}
namespace ara {
namespace adsfi {
class SensorLidarPointCloudServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class RadarPointCloudServiceInterfaceClient;
}
}
namespace adsfi {
class LocationInfoIntfClient;
}
namespace adsfi {
class Object3dArrayIntfClient;
}
namespace ara {
namespace adsfi {
class VisionSegmentServiceInterfaceClient;
}
}
namespace mdc {
namespace fusion_follow_base {
namespace {
using adsfi::FusionObjectArrayIntfServer;
using ara::adsfi::SensorLidarPointCloudServiceInterfaceClient;
using ara::adsfi::RadarPointCloudServiceInterfaceClient;
using adsfi::LocationInfoIntfClient;
using adsfi::Object3dArrayIntfClient;
using ara::adsfi::VisionSegmentServiceInterfaceClient;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class FusionFollowBaseSwC {
public:
    explicit FusionFollowBaseSwC();

    virtual ~FusionFollowBaseSwC();

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

    ara::core::Vector<std::string> GetFusionObjectArrayIntfServerVec()
    {
        std::lock_guard<std::mutex> lck(fusionObjectArrayIntfServerPortVecMtx_);
        return fusionObjectArrayIntfServerPortVec_;
    }
    ara::core::Vector<std::string> GetSensorLidarPointCloudServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(sensorLidarPointCloudServiceInterfaceClientPortVecMtx_);
        return sensorLidarPointCloudServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetRadarPointCloudServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(radarPointCloudServiceInterfaceClientPortVecMtx_);
        return radarPointCloudServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetLocationInfoIntfClientVec()
    {
        std::lock_guard<std::mutex> lck(locationInfoIntfClientPortVecMtx_);
        return locationInfoIntfClientPortVec_;
    }
    ara::core::Vector<std::string> GetObject3dArrayIntfClientVec()
    {
        std::lock_guard<std::mutex> lck(object3dArrayIntfClientPortVecMtx_);
        return object3dArrayIntfClientPortVec_;
    }
    ara::core::Vector<std::string> GetVisionSegmentServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(visionSegmentServiceInterfaceClientPortVecMtx_);
        return visionSegmentServiceInterfaceClientPortVec_;
    }
    

    std::shared_ptr<FusionObjectArrayIntfServer> GetFusionObjectArrayIntfServer(const ara::core::String& portName);
    std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient> GetSensorLidarPointCloudServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<RadarPointCloudServiceInterfaceClient> GetRadarPointCloudServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<LocationInfoIntfClient> GetLocationInfoIntfClient(const ara::core::String& portName);
    std::shared_ptr<Object3dArrayIntfClient> GetObject3dArrayIntfClient(const ara::core::String& portName);
    std::shared_ptr<VisionSegmentServiceInterfaceClient> GetVisionSegmentServiceInterfaceClient(const ara::core::String& portName);
    

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
    ara::core::Vector<uint32_t> fusionObjectArrayIntfServerIdVec_;
    ara::core::Vector<std::string> fusionObjectArrayIntfServerPortVec_;
    std::mutex fusionObjectArrayIntfServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> fusionObjectArrayIntfServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<FusionObjectArrayIntfServer>> fusionObjectArrayIntfServerInsMap_;
    std::mutex fusionObjectArrayIntfServerInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> sensorLidarPointCloudServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> sensorLidarPointCloudServiceInterfaceClientPortVec_;
    std::mutex sensorLidarPointCloudServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorLidarPointCloudServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient>> sensorLidarPointCloudServiceInterfaceClientInsMap_;
    std::mutex sensorLidarPointCloudServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> radarPointCloudServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> radarPointCloudServiceInterfaceClientPortVec_;
    std::mutex radarPointCloudServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> radarPointCloudServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<RadarPointCloudServiceInterfaceClient>> radarPointCloudServiceInterfaceClientInsMap_;
    std::mutex radarPointCloudServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> locationInfoIntfClientIdVec_;
    ara::core::Vector<std::string> locationInfoIntfClientPortVec_;
    std::mutex locationInfoIntfClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> locationInfoIntfClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<LocationInfoIntfClient>> locationInfoIntfClientInsMap_;
    std::mutex locationInfoIntfClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> object3dArrayIntfClientIdVec_;
    ara::core::Vector<std::string> object3dArrayIntfClientPortVec_;
    std::mutex object3dArrayIntfClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> object3dArrayIntfClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<Object3dArrayIntfClient>> object3dArrayIntfClientInsMap_;
    std::mutex object3dArrayIntfClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> visionSegmentServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> visionSegmentServiceInterfaceClientPortVec_;
    std::mutex visionSegmentServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> visionSegmentServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<VisionSegmentServiceInterfaceClient>> visionSegmentServiceInterfaceClientInsMap_;
    std::mutex visionSegmentServiceInterfaceClientInsMapMtx_;

};
}  /* namespace fusion_follow_base */
}  /* namespace mdc */

#endif  /* MDC_FUSIONFOLLOWBASESWC_H */