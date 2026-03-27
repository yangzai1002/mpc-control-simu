/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_MULTYCAMERADETBASESWC_H
#define MDC_MULTYCAMERADETBASESWC_H

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
class VisionSegmentServiceInterfaceServer;
}
}
namespace ara {
namespace adsfi {
class MsgImageDataListServiceInterfaceClient;
}
}
namespace mdc {
namespace multy_camera_det_base {
namespace {
using ara::adsfi::VisionSegmentServiceInterfaceServer;
using ara::adsfi::MsgImageDataListServiceInterfaceClient;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class MultyCameraDetBaseSwC {
public:
    explicit MultyCameraDetBaseSwC();

    virtual ~MultyCameraDetBaseSwC();

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

    ara::core::Vector<std::string> GetVisionSegmentServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(visionSegmentServiceInterfaceServerPortVecMtx_);
        return visionSegmentServiceInterfaceServerPortVec_;
    }
    ara::core::Vector<std::string> GetMsgImageDataListServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(msgImageDataListServiceInterfaceClientPortVecMtx_);
        return msgImageDataListServiceInterfaceClientPortVec_;
    }
    

    std::shared_ptr<VisionSegmentServiceInterfaceServer> GetVisionSegmentServiceInterfaceServer(const ara::core::String& portName);
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
    /* server agent */
    ara::core::Vector<uint32_t> visionSegmentServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> visionSegmentServiceInterfaceServerPortVec_;
    std::mutex visionSegmentServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> visionSegmentServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<VisionSegmentServiceInterfaceServer>> visionSegmentServiceInterfaceServerInsMap_;
    std::mutex visionSegmentServiceInterfaceServerInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> msgImageDataListServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> msgImageDataListServiceInterfaceClientPortVec_;
    std::mutex msgImageDataListServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> msgImageDataListServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<MsgImageDataListServiceInterfaceClient>> msgImageDataListServiceInterfaceClientInsMap_;
    std::mutex msgImageDataListServiceInterfaceClientInsMapMtx_;

};
}  /* namespace multy_camera_det_base */
}  /* namespace mdc */

#endif  /* MDC_MULTYCAMERADETBASESWC_H */