/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#ifndef MDC_XVEHICLESTATUSRECORDERBASESWC_H
#define MDC_XVEHICLESTATUSRECORDERBASESWC_H

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
class RemoteControlServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class BodyCommandServiceInterfaceClient;
}
}
namespace adsfi {
class LocationInfoIntfClient;
}
namespace ara {
namespace adsfi {
class SensorGpsServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class VehicleInfomationServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class FaultDataServiceInterfaceClient;
}
}
namespace ara {
namespace adsfi {
class MsgImageDataListServiceInterfaceClient;
}
}
namespace mdc {
namespace xvehicle_status_recorder_base {
namespace {
using ara::adsfi::RemoteControlServiceInterfaceClient;
using ara::adsfi::BodyCommandServiceInterfaceClient;
using adsfi::LocationInfoIntfClient;
using ara::adsfi::SensorGpsServiceInterfaceClient;
using ara::adsfi::VehicleInfomationServiceInterfaceClient;
using ara::adsfi::FaultDataServiceInterfaceClient;
using ara::adsfi::MsgImageDataListServiceInterfaceClient;
using InitHandleType = std::function<bool()>;
using StopHandleType = std::function<void()>;
}

class XvehicleStatusRecorderBaseSwC {
public:
    explicit XvehicleStatusRecorderBaseSwC();

    virtual ~XvehicleStatusRecorderBaseSwC();

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

    ara::core::Vector<std::string> GetRemoteControlServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(remoteControlServiceInterfaceClientPortVecMtx_);
        return remoteControlServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetBodyCommandServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(bodyCommandServiceInterfaceClientPortVecMtx_);
        return bodyCommandServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetLocationInfoIntfClientVec()
    {
        std::lock_guard<std::mutex> lck(locationInfoIntfClientPortVecMtx_);
        return locationInfoIntfClientPortVec_;
    }
    ara::core::Vector<std::string> GetSensorGpsServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(sensorGpsServiceInterfaceClientPortVecMtx_);
        return sensorGpsServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetVehicleInfomationServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(vehicleInfomationServiceInterfaceClientPortVecMtx_);
        return vehicleInfomationServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetFaultDataServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(faultDataServiceInterfaceClientPortVecMtx_);
        return faultDataServiceInterfaceClientPortVec_;
    }
    ara::core::Vector<std::string> GetMsgImageDataListServiceInterfaceClientVec()
    {
        std::lock_guard<std::mutex> lck(msgImageDataListServiceInterfaceClientPortVecMtx_);
        return msgImageDataListServiceInterfaceClientPortVec_;
    }
    

    std::shared_ptr<RemoteControlServiceInterfaceClient> GetRemoteControlServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<BodyCommandServiceInterfaceClient> GetBodyCommandServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<LocationInfoIntfClient> GetLocationInfoIntfClient(const ara::core::String& portName);
    std::shared_ptr<SensorGpsServiceInterfaceClient> GetSensorGpsServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<VehicleInfomationServiceInterfaceClient> GetVehicleInfomationServiceInterfaceClient(const ara::core::String& portName);
    std::shared_ptr<FaultDataServiceInterfaceClient> GetFaultDataServiceInterfaceClient(const ara::core::String& portName);
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
    ara::core::Vector<uint32_t> remoteControlServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> remoteControlServiceInterfaceClientPortVec_;
    std::mutex remoteControlServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> remoteControlServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<RemoteControlServiceInterfaceClient>> remoteControlServiceInterfaceClientInsMap_;
    std::mutex remoteControlServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> bodyCommandServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> bodyCommandServiceInterfaceClientPortVec_;
    std::mutex bodyCommandServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> bodyCommandServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<BodyCommandServiceInterfaceClient>> bodyCommandServiceInterfaceClientInsMap_;
    std::mutex bodyCommandServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> locationInfoIntfClientIdVec_;
    ara::core::Vector<std::string> locationInfoIntfClientPortVec_;
    std::mutex locationInfoIntfClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> locationInfoIntfClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<LocationInfoIntfClient>> locationInfoIntfClientInsMap_;
    std::mutex locationInfoIntfClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> sensorGpsServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> sensorGpsServiceInterfaceClientPortVec_;
    std::mutex sensorGpsServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> sensorGpsServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<SensorGpsServiceInterfaceClient>> sensorGpsServiceInterfaceClientInsMap_;
    std::mutex sensorGpsServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> vehicleInfomationServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> vehicleInfomationServiceInterfaceClientPortVec_;
    std::mutex vehicleInfomationServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> vehicleInfomationServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<VehicleInfomationServiceInterfaceClient>> vehicleInfomationServiceInterfaceClientInsMap_;
    std::mutex vehicleInfomationServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> faultDataServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> faultDataServiceInterfaceClientPortVec_;
    std::mutex faultDataServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> faultDataServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<FaultDataServiceInterfaceClient>> faultDataServiceInterfaceClientInsMap_;
    std::mutex faultDataServiceInterfaceClientInsMapMtx_;
    /* client agent */
    ara::core::Vector<uint32_t> msgImageDataListServiceInterfaceClientIdVec_;
    ara::core::Vector<std::string> msgImageDataListServiceInterfaceClientPortVec_;
    std::mutex msgImageDataListServiceInterfaceClientPortVecMtx_;
    ara::core::Map<std::string, uint32_t> msgImageDataListServiceInterfaceClientIdMap_;
    ara::core::Map<std::string, std::shared_ptr<MsgImageDataListServiceInterfaceClient>> msgImageDataListServiceInterfaceClientInsMap_;
    std::mutex msgImageDataListServiceInterfaceClientInsMapMtx_;

};
}  /* namespace xvehicle_status_recorder_base */
}  /* namespace mdc */

#endif  /* MDC_XVEHICLESTATUSRECORDERBASESWC_H */