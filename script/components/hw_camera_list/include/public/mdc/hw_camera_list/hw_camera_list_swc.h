/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_HWCAMERALISTSWC_H
#define MDC_HWCAMERALISTSWC_H

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
class MsgImageDataListServiceInterfaceServer;
}
}
namespace ara {
namespace adsfi {
class SensorCompressedImageListServiceInterfaceServer;
}
}
namespace mdc {
namespace hw_camera_list {
namespace {
using ara::adsfi::MsgImageDataListServiceInterfaceServer;
using ara::adsfi::SensorCompressedImageListServiceInterfaceServer;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class HwCameraListSwC {
public:
    explicit HwCameraListSwC();

    virtual ~HwCameraListSwC();

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

    ara::core::Vector<std::string> GetMsgImageDataListServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(msgImageDataListServiceInterfaceServerPortVecMtx_);
        return msgImageDataListServiceInterfaceServerPortVec_;
    }
    ara::core::Vector<std::string> GetSensorCompressedImageListServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(sensorCompressedImageListServiceInterfaceServerPortVecMtx_);
        return sensorCompressedImageListServiceInterfaceServerPortVec_;
    }
    

    std::shared_ptr<MsgImageDataListServiceInterfaceServer> GetMsgImageDataListServiceInterfaceServer(const ara::core::String& portName);
    std::shared_ptr<SensorCompressedImageListServiceInterfaceServer> GetSensorCompressedImageListServiceInterfaceServer(const ara::core::String& portName);
    

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
    ara::core::Vector<uint32_t> msgImageDataListServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> msgImageDataListServiceInterfaceServerPortVec_;
    std::mutex msgImageDataListServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> msgImageDataListServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<MsgImageDataListServiceInterfaceServer>> msgImageDataListServiceInterfaceServerInsMap_;
    std::mutex msgImageDataListServiceInterfaceServerInsMapMtx_;
    /* server agent */
    ara::core::Vector<uint32_t> sensorCompressedImageListServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> sensorCompressedImageListServiceInterfaceServerPortVec_;
    std::mutex sensorCompressedImageListServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorCompressedImageListServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorCompressedImageListServiceInterfaceServer>> sensorCompressedImageListServiceInterfaceServerInsMap_;
    std::mutex sensorCompressedImageListServiceInterfaceServerInsMapMtx_;

};
}  /* namespace hw_camera_list */
}  /* namespace mdc */

#endif  /* MDC_HWCAMERALISTSWC_H */