/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_PLANNINGBASESWC_H
#define MDC_PLANNINGBASESWC_H

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
class PlanningResultIntfServer;
}
namespace ara {
namespace adsfi {
class PerceptionStaticEnvServiceInterfaceClient;
}
}
namespace adsfi {
class FusionObjectArrayIntfClient;
}
namespace adsfi {
class LocationInfoIntfClient;
}
namespace ara {
namespace adsfi {
class VehicleInfomationServiceInterfaceClient;
}
}
namespace mdc {
namespace planning_base {
namespace {
using adsfi::PlanningResultIntfServer;
using ara::adsfi::PerceptionStaticEnvServiceInterfaceClient;
using adsfi::FusionObjectArrayIntfClient;
using adsfi::LocationInfoIntfClient;
using ara::adsfi::VehicleInfomationServiceInterfaceClient;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class PlanningBaseSwC {
public:
    explicit PlanningBaseSwC();

    virtual ~PlanningBaseSwC();

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

    ara::core::Vector<std::string> GetPlanningResultIntfServerVec()
    {
        std::lock_guard<std::mutex> lck(planningResultIntfServerPortVecMtx_);
        return planningResultIntfServerPortVec_;
    }
    ara::core::Vector<std::string> GetPerceptionStaticEnvServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(perceptionStaticEnvServiceInterfaceClientPortVecMtx_);
        return perceptionStaticEnvServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetFusionObjectArrayIntfClientVec()
    {
        std::lock_guard<std::mutex> lck(fusionObjectArrayIntfClientPortVecMtx_);
        return fusionObjectArrayIntfClientPortVec_;
    }
    ara::core::Vector<std::string> GetLocationInfoIntfClientVec()
    {
        std::lock_guard<std::mutex> lck(locationInfoIntfClientPortVecMtx_);
        return locationInfoIntfClientPortVec_;
    }
    ara::core::Vector<std::string> GetVehicleInfomationServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(vehicleInfomationServiceInterfaceClientPortVecMtx_);
        return vehicleInfomationServiceInterfaceClientPortVec_;
    }
    

    std::shared_ptr<PlanningResultIntfServer> GetPlanningResultIntfServer(const ara::core::String& portName);
    std::shared_ptr<PerceptionStaticEnvServiceInterfaceClient> GetPerceptionStaticEnvServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<FusionObjectArrayIntfClient> GetFusionObjectArrayIntfClient(const ara::core::String& portName);
    std::shared_ptr<LocationInfoIntfClient> GetLocationInfoIntfClient(const ara::core::String& portName);
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
    ara::core::Vector<uint32_t> planningResultIntfServerIdVec_;
    ara::core::Vector<std::string> planningResultIntfServerPortVec_;
    std::mutex planningResultIntfServerPortVecMtx_;
    ara::core::Map<std::string, uint32_t> planningResultIntfServerIdMap_;
    ara::core::Map<std::string, std::shared_ptr<PlanningResultIntfServer>> planningResultIntfServerInsMap_;
    std::mutex planningResultIntfServerInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> perceptionStaticEnvServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> perceptionStaticEnvServiceInterfaceClientPortVec_;
    std::mutex perceptionStaticEnvServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> perceptionStaticEnvServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<PerceptionStaticEnvServiceInterfaceClient>> perceptionStaticEnvServiceInterfaceClientInsMap_;
    std::mutex perceptionStaticEnvServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> fusionObjectArrayIntfClientIdVec_;
    ara::core::Vector<std::string> fusionObjectArrayIntfClientPortVec_;
    std::mutex fusionObjectArrayIntfClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> fusionObjectArrayIntfClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<FusionObjectArrayIntfClient>> fusionObjectArrayIntfClientInsMap_;
    std::mutex fusionObjectArrayIntfClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> locationInfoIntfClientIdVec_;
    ara::core::Vector<std::string> locationInfoIntfClientPortVec_;
    std::mutex locationInfoIntfClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> locationInfoIntfClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<LocationInfoIntfClient>> locationInfoIntfClientInsMap_;
    std::mutex locationInfoIntfClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> vehicleInfomationServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> vehicleInfomationServiceInterfaceClientPortVec_;
    std::mutex vehicleInfomationServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> vehicleInfomationServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<VehicleInfomationServiceInterfaceClient>> vehicleInfomationServiceInterfaceClientInsMap_;
    std::mutex vehicleInfomationServiceInterfaceClientInsMapMtx_;

};
}  /* namespace planning_base */
}  /* namespace mdc */

#endif  /* MDC_PLANNINGBASESWC_H */