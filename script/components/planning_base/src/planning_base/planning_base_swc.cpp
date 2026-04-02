/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/planning_base/planning_base_swc.h"

#include <cstring>
#include <fstream>
#include <climits>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#ifndef SCFI_SDK_X86
#include "ara/rm/rm.h"
#include "driver/ascend_hal.h"
#endif
#include "mdc/planning_base_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/planning_base/planningresultintf_server.h"
#include "adsfi/planningresultintf_skeleton.h"
#include "mdc/planning_base/perceptionstaticenvserviceinterface_client.h"
#include "mdc/planning_base/fusionobjectarrayintf_client.h"
#include "mdc/planning_base/locationinfointf_client.h"
#include "mdc/planning_base/vehicleinfomationserviceinterface_client.h"

#define RETURN_IF_ERROR(ret, str) \
if (!(ret)) { \
    LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find " << (str) << " in Config File!"; \
    return (HAF_ERROR); \
}

#define COUT_IF_ERROR(ret, str) \
if (!(ret)) { \
    LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find " << (str) << " in Config File!"; \
    return (HAF_ERROR); \
}

#define RETURN_IF_INIT_SERVICE_FAILED(ret, str, id) \
if (!(ret)) { \
    LOG_SPACE::GetLoggerIns("SWC")->LogError() << (str) << " create failed, instance ID: " << (id); \
    return false; \
}

namespace {
    /**
     * @brief 删除重复元素, 剩余元素相对位置不变
     *
     */
    template <class T>
    void StableUnique(ara::core::Vector<T>& vv)
    {
        std::unordered_map<T, bool> flag;
        for (auto it = vv.begin(); it != vv.end();) {
            if (!flag[*it]) {
                flag[*it] = true;
                ++it;
            } else {
                it = vv.erase(it);
            }
        }
    }

    /**
     * @brief 将pod数据类型装换为string以便于打印
     *
     */
    template <class T>
    ara::core::String Vec2Str(const ara::core::Vector<T>& vec)
    {
        static_assert(std::is_pod<T>::value, "Template T must be plain old data (POD) type");
        std::ostringstream oss;
        if (!vec.empty()) {
            std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T>(oss, ", "));
            oss << vec.back();
        }
        return oss.str();
    }
}

namespace mdc {
namespace planning_base {
using ara::core::String;
using LOG_SPACE = mdc::planning_base::PlanningBaseLogger;

PlanningBaseSwC::PlanningBaseSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

PlanningBaseSwC::~PlanningBaseSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool PlanningBaseSwC::Init()
{
    
    if (initFlag_) {
        LOG_SPACE::GetLoggerIns("SWC")->LogWarn()<< "Service has been initialized";
        return false;
    }

    if (!LoadConfig()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Load config failed. Program will now exit.";
        return false;
    }


    #ifndef SCFI_SDK_X86
    BuffCfg bufCfg{};
    const int32_t halRet = halBuffInit(&bufCfg);
    if ((DRV_ERROR_NONE != halRet) && (DRV_ERROR_REPEATED_INIT != halRet)) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Hal buffer init failed, hal error code: " << halRet;
        return false;
    }

    if (!ara::rm::RegisterHisiResource()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Register process to RM failed! Check if RM has been started.";
        return false;
    }
    #endif
    /* server agent */
    for (auto id : planningResultIntfServerIdMap_) {
        auto resultToken = adsfi::skeleton::PlanningResultIntfSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            planningResultIntfServerInsMap_[id.first] = std::make_shared<PlanningResultIntfServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (planningResultIntfServerInsMap_[id.first]!= nullptr),
            "PlanningResultIntfSkeleton",
            id.second);
    }
    /* client agent */
    for (auto id : perceptionStaticEnvServiceInterfaceClientIdMap_) {
        perceptionStaticEnvServiceInterfaceClientInsMap_[id.first] = std::make_shared<PerceptionStaticEnvServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (perceptionStaticEnvServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "PerceptionStaticEnvServiceInterfaceProxy",
            id.second);
    }
    /* client agent */
    for (auto id : fusionObjectArrayIntfClientIdMap_) {
        fusionObjectArrayIntfClientInsMap_[id.first] = std::make_shared<FusionObjectArrayIntfClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (fusionObjectArrayIntfClientInsMap_[id.first]!= nullptr),
            "FusionObjectArrayIntfProxy",
            id.second);
    }
    /* client agent */
    for (auto id : locationInfoIntfClientIdMap_) {
        locationInfoIntfClientInsMap_[id.first] = std::make_shared<LocationInfoIntfClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (locationInfoIntfClientInsMap_[id.first]!= nullptr),
            "LocationInfoIntfProxy",
            id.second);
    }
    /* client agent */
    for (auto id : vehicleInfomationServiceInterfaceClientIdMap_) {
        vehicleInfomationServiceInterfaceClientInsMap_[id.first] = std::make_shared<VehicleInfomationServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (vehicleInfomationServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "VehicleInfomationServiceInterfaceProxy",
            id.second);
    }

    if ((initHandler_) && (!initHandler_())) {
        return false;
    }

    initFlag_ = true;
    return true;
}

void PlanningBaseSwC::Stop()
{
    workFlag_ = false;
    /* server agent */
    for (auto id : planningResultIntfServerIdMap_) {
        if (planningResultIntfServerInsMap_[id.first]) {
            planningResultIntfServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    /* client agent */
    for (auto id : perceptionStaticEnvServiceInterfaceClientIdMap_) {
        if (perceptionStaticEnvServiceInterfaceClientInsMap_[id.first]) {
            perceptionStaticEnvServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : fusionObjectArrayIntfClientIdMap_) {
        if (fusionObjectArrayIntfClientInsMap_[id.first]) {
            fusionObjectArrayIntfClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : locationInfoIntfClientIdMap_) {
        if (locationInfoIntfClientInsMap_[id.first]) {
            locationInfoIntfClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : vehicleInfomationServiceInterfaceClientIdMap_) {
        if (vehicleInfomationServiceInterfaceClientInsMap_[id.first]) {
            vehicleInfomationServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    if (stopHandler_) {
        stopHandler_();
    }
    return;
}


std::shared_ptr<PlanningResultIntfServer> PlanningBaseSwC::GetPlanningResultIntfServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(planningResultIntfServerInsMapMtx_);
    if (planningResultIntfServerInsMap_.find(portName) == planningResultIntfServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(planningResultIntfServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return planningResultIntfServerInsMap_[portName];
}
std::shared_ptr<PerceptionStaticEnvServiceInterfaceClient> PlanningBaseSwC::GetPerceptionStaticEnvServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(perceptionStaticEnvServiceInterfaceClientInsMapMtx_);
    if (perceptionStaticEnvServiceInterfaceClientInsMap_.find(portName) == perceptionStaticEnvServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(perceptionStaticEnvServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return perceptionStaticEnvServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<FusionObjectArrayIntfClient> PlanningBaseSwC::GetFusionObjectArrayIntfClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(fusionObjectArrayIntfClientInsMapMtx_);
    if (fusionObjectArrayIntfClientInsMap_.find(portName) == fusionObjectArrayIntfClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(fusionObjectArrayIntfClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return fusionObjectArrayIntfClientInsMap_[portName];
}
std::shared_ptr<LocationInfoIntfClient> PlanningBaseSwC::GetLocationInfoIntfClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(locationInfoIntfClientInsMapMtx_);
    if (locationInfoIntfClientInsMap_.find(portName) == locationInfoIntfClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(locationInfoIntfClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return locationInfoIntfClientInsMap_[portName];
}
std::shared_ptr<VehicleInfomationServiceInterfaceClient> PlanningBaseSwC::GetVehicleInfomationServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(vehicleInfomationServiceInterfaceClientInsMapMtx_);
    if (vehicleInfomationServiceInterfaceClientInsMap_.find(portName) == vehicleInfomationServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(vehicleInfomationServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return vehicleInfomationServiceInterfaceClientInsMap_[portName];
}

bool PlanningBaseSwC::LoadConfig()
{
    /* 通过环境变量获取配置文件路径 */
    char* path = secure_getenv("CM_CONFIG_FILE_PATH");
    if (path == nullptr) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Empty process config path!";
        return false;
    }
    char fileRealPath[PATH_MAX + 1] = {0};
    if (realpath(path, &fileRealPath[0]) == nullptr) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Invalid process config path!";
        return false;
    }

    configFile_ = path;
    configFile_ += "/scfi_mapping.yaml";

    /* 配置文件路径为空或非yaml文件 */
    if (configFile_.empty()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "config path must be a valid yaml file!";
        return false;
    }

    if (!DoesFileExist(configFile_)) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << configFile_ << " does not exist.";
        return false;
    }

    auto config = std::make_unique<MdcYamlNode>(configFile_);
    if (!config->GetValue<std::string>("identifier", identifier_)) {
        LOG_SPACE::GetLoggerIns("SWC")->LogWarn() << "Warning: swc identifier not specified, use default config.";
    }

    if (!ParsingInstanceId(config)) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Failed to parse instance id!";
        return false;
    }
    return true;
}

bool PlanningBaseSwC::DoesFileExist(const ara::core::String& filePath)
{
    struct stat fileInfo = {};
    if (memset_s(&fileInfo, sizeof(fileInfo), 0, sizeof(fileInfo)) != 0) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "DoesFileExist Initialization of fileInfo failed.";
        return false;
    }
    if (lstat(filePath.c_str(), &fileInfo) != 0) {
        return false;
    }
    if (!IsDirType(fileInfo.st_mode)) {
        return true;
    }
    return false;
}

bool PlanningBaseSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool PlanningBaseSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("planning_base")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find planning_base in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["planning_base"];
    /* server agent */
    if (!swcConfig.HasKeyValue("sendInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find sendInstanceID in config file!";
        return false;
    }
    swcConfig["sendInstanceID"]["PlanningResultIntf"].GetValue<std::vector<uint32_t>>(
        "instanceId", planningResultIntfServerIdVec_);
    StableUnique(planningResultIntfServerIdVec_);
    swcConfig["sendInstanceID"]["PlanningResultIntf"].GetValue<std::vector<std::string>>(
        "portName", planningResultIntfServerPortVec_);
    StableUnique(planningResultIntfServerPortVec_);
    if (planningResultIntfServerIdVec_.size() != planningResultIntfServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < planningResultIntfServerIdVec_.size(); ++i) {
        planningResultIntfServerIdMap_[planningResultIntfServerPortVec_[i]] = planningResultIntfServerIdVec_[i];
    }

    /* client agent */
    if (!swcConfig.HasKeyValue("recvInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find recvInstanceID in config file!";
        return false;
    }
    swcConfig["recvInstanceID"]["PerceptionStaticEnvServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", perceptionStaticEnvServiceInterfaceClientIdVec_);
    StableUnique(perceptionStaticEnvServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["PerceptionStaticEnvServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", perceptionStaticEnvServiceInterfaceClientPortVec_);
    StableUnique(perceptionStaticEnvServiceInterfaceClientPortVec_);
    if (perceptionStaticEnvServiceInterfaceClientIdVec_.size() != perceptionStaticEnvServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < perceptionStaticEnvServiceInterfaceClientIdVec_.size(); ++i) {
        perceptionStaticEnvServiceInterfaceClientIdMap_[perceptionStaticEnvServiceInterfaceClientPortVec_[i]] = perceptionStaticEnvServiceInterfaceClientIdVec_[i];
    }
    swcConfig["recvInstanceID"]["FusionObjectArrayIntf"].GetValue<std::vector<uint32_t>>(
        "instanceId", fusionObjectArrayIntfClientIdVec_);
    StableUnique(fusionObjectArrayIntfClientIdVec_);
    swcConfig["recvInstanceID"]["FusionObjectArrayIntf"].GetValue<std::vector<std::string>>(
        "portName", fusionObjectArrayIntfClientPortVec_);
    StableUnique(fusionObjectArrayIntfClientPortVec_);
    if (fusionObjectArrayIntfClientIdVec_.size() != fusionObjectArrayIntfClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < fusionObjectArrayIntfClientIdVec_.size(); ++i) {
        fusionObjectArrayIntfClientIdMap_[fusionObjectArrayIntfClientPortVec_[i]] = fusionObjectArrayIntfClientIdVec_[i];
    }
    swcConfig["recvInstanceID"]["LocationInfoIntf"].GetValue<std::vector<uint32_t>>(
        "instanceId", locationInfoIntfClientIdVec_);
    StableUnique(locationInfoIntfClientIdVec_);
    swcConfig["recvInstanceID"]["LocationInfoIntf"].GetValue<std::vector<std::string>>(
        "portName", locationInfoIntfClientPortVec_);
    StableUnique(locationInfoIntfClientPortVec_);
    if (locationInfoIntfClientIdVec_.size() != locationInfoIntfClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < locationInfoIntfClientIdVec_.size(); ++i) {
        locationInfoIntfClientIdMap_[locationInfoIntfClientPortVec_[i]] = locationInfoIntfClientIdVec_[i];
    }
    swcConfig["recvInstanceID"]["VehicleInfomationServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", vehicleInfomationServiceInterfaceClientIdVec_);
    StableUnique(vehicleInfomationServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["VehicleInfomationServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", vehicleInfomationServiceInterfaceClientPortVec_);
    StableUnique(vehicleInfomationServiceInterfaceClientPortVec_);
    if (vehicleInfomationServiceInterfaceClientIdVec_.size() != vehicleInfomationServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < vehicleInfomationServiceInterfaceClientIdVec_.size(); ++i) {
        vehicleInfomationServiceInterfaceClientIdMap_[vehicleInfomationServiceInterfaceClientPortVec_[i]] = vehicleInfomationServiceInterfaceClientIdVec_[i];
    }
    return true;
}
}  /* namespace planning_base */
}  /* namespace mdc */