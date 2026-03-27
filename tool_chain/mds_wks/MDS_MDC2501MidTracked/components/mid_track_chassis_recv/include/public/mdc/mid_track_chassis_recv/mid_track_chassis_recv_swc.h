/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_MIDTRACKCHASSISRECVSWC_H
#define MDC_MIDTRACKCHASSISRECVSWC_H

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
class VehicleInfomationServiceInterfaceServer;
}
}
namespace mdc {
namespace mid_track_chassis_recv {
namespace {
using ara::adsfi::VehicleInfomationServiceInterfaceServer;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class MidTrackChassisRecvSwC {
public:
    explicit MidTrackChassisRecvSwC();

    virtual ~MidTrackChassisRecvSwC();

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

    ara::core::Vector<std::string> GetVehicleInfomationServiceInterfaceServerVec()
    {
        std::lock_guard<std::mutex> lck(vehicleInfomationServiceInterfaceServerPortVecMtx_);
        return vehicleInfomationServiceInterfaceServerPortVec_;
    }
    

    std::shared_ptr<VehicleInfomationServiceInterfaceServer> GetVehicleInfomationServiceInterfaceServer(const ara::core::String& portName);
    

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
    ara::core::Vector<uint32_t> vehicleInfomationServiceInterfaceServerIdVec_;
    ara::core::Vector<std::string> vehicleInfomationServiceInterfaceServerPortVec_;
    std::mutex vehicleInfomationServiceInterfaceServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> vehicleInfomationServiceInterfaceServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<VehicleInfomationServiceInterfaceServer>> vehicleInfomationServiceInterfaceServerInsMap_;
    std::mutex vehicleInfomationServiceInterfaceServerInsMapMtx_;

};
}  /* namespace mid_track_chassis_recv */
}  /* namespace mdc */

#endif  /* MDC_MIDTRACKCHASSISRECVSWC_H */