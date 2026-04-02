/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_SENSORVIUALIZEBASESWC_H
#define MDC_SENSORVIUALIZEBASESWC_H

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
class SensorLidarPointCloudServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class MsgImageDataListServiceInterfaceClient;
}
}
namespace mdc {
namespace sensor_viualize_base {
namespace {
using ara::adsfi::SensorLidarPointCloudServiceInterfaceClient;
using ara::adsfi::MsgImageDataListServiceInterfaceClient;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class SensorViualizeBaseSwC {
public:
    explicit SensorViualizeBaseSwC();

    virtual ~SensorViualizeBaseSwC();

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

    ara::core::Vector<std::string> GetSensorLidarPointCloudServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(sensorLidarPointCloudServiceInterfaceClientPortVecMtx_);
        return sensorLidarPointCloudServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetMsgImageDataListServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(msgImageDataListServiceInterfaceClientPortVecMtx_);
        return msgImageDataListServiceInterfaceClientPortVec_;
    }
    

    std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient> GetSensorLidarPointCloudServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<MsgImageDataListServiceInterfaceClient> GetMsgImageDataListServiceInterfaceClient(const ara::core::String& portName);
    

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
    ara::core::Vector<uint32_t> sensorLidarPointCloudServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> sensorLidarPointCloudServiceInterfaceClientPortVec_;
    std::mutex sensorLidarPointCloudServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorLidarPointCloudServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient>> sensorLidarPointCloudServiceInterfaceClientInsMap_;
    std::mutex sensorLidarPointCloudServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> msgImageDataListServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> msgImageDataListServiceInterfaceClientPortVec_;
    std::mutex msgImageDataListServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> msgImageDataListServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<MsgImageDataListServiceInterfaceClient>> msgImageDataListServiceInterfaceClientInsMap_;
    std::mutex msgImageDataListServiceInterfaceClientInsMapMtx_;

};
}  /* namespace sensor_viualize_base */
}  /* namespace mdc */

#endif  /* MDC_SENSORVIUALIZEBASESWC_H */