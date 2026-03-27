/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_XFAULTHANDLERBASESWC_H
#define MDC_XFAULTHANDLERBASESWC_H

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
class FaultDataServiceInterfaceServer;
}
}
namespace ara {
namespace adsfi {
class RemoteControlServiceInterfaceClient;
}
}
namespace mdc {
namespace xfault_handler_base {
namespace {
using ara::adsfi::FaultDataServiceInterfaceServer;
using ara::adsfi::RemoteControlServiceInterfaceClient;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class XfaultHandlerBaseSwC {
public:
    explicit XfaultHandlerBaseSwC();

    virtual ~XfaultHandlerBaseSwC();

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

    ara::core::Vector<std::string> GetFaultDataServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(faultDataServiceInterfaceServerPortVecMtx_);
        return faultDataServiceInterfaceServerPortVec_;
    }
    ara::core::Vector<std::string> GetRemoteControlServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(remoteControlServiceInterfaceClientPortVecMtx_);
        return remoteControlServiceInterfaceClientPortVec_;
    }
    

    std::shared_ptr<FaultDataServiceInterfaceServer> GetFaultDataServiceInterfaceServer(const ara::core::String& portName);
    std::shared_ptr<RemoteControlServiceInterfaceClient> GetRemoteControlServiceInterfaceClient(const ara::core::String& portName);
    

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
    ara::core::Vector<uint32_t> faultDataServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> faultDataServiceInterfaceServerPortVec_;
    std::mutex faultDataServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> faultDataServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<FaultDataServiceInterfaceServer>> faultDataServiceInterfaceServerInsMap_;
    std::mutex faultDataServiceInterfaceServerInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> remoteControlServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> remoteControlServiceInterfaceClientPortVec_;
    std::mutex remoteControlServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> remoteControlServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<RemoteControlServiceInterfaceClient>> remoteControlServiceInterfaceClientInsMap_;
    std::mutex remoteControlServiceInterfaceClientInsMapMtx_;

};
}  /* namespace xfault_handler_base */
}  /* namespace mdc */

#endif  /* MDC_XFAULTHANDLERBASESWC_H */