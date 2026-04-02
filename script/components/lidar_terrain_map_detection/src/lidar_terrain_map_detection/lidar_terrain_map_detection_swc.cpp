/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/lidar_terrain_map_detection/lidar_terrain_map_detection_swc.h"

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
#include "mdc/lidar_terrain_map_detection_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/lidar_terrain_map_detection/perceptionstaticenvserviceinterface_server.h"
#include "ara/adsfi/perceptionstaticenvserviceinterface_skeleton.h"
#include "mdc/lidar_terrain_map_detection/sensorlidarpointcloudserviceinterface_client.h"
#include "mdc/lidar_terrain_map_detection/locationinfointf_client.h"

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
namespace lidar_terrain_map_detection {
using ara::core::String;
using LOG_SPACE = mdc::lidar_terrain_map_detection::LidarTerrainMapDetectionLogger;

LidarTerrainMapDetectionSwC::LidarTerrainMapDetectionSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

LidarTerrainMapDetectionSwC::~LidarTerrainMapDetectionSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool LidarTerrainMapDetectionSwC::Init()
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
    for (auto id : perceptionStaticEnvServiceInterfaceServerIdMap_) {
        auto resultToken = ara::adsfi::skeleton::PerceptionStaticEnvServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            perceptionStaticEnvServiceInterfaceServerInsMap_[id.first] = std::make_shared<PerceptionStaticEnvServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (perceptionStaticEnvServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "PerceptionStaticEnvServiceInterfaceSkeleton",
            id.second);
    }
    /* client agent */
    for (auto id : sensorLidarPointCloudServiceInterfaceClientIdMap_) {
        sensorLidarPointCloudServiceInterfaceClientInsMap_[id.first] = std::make_shared<SensorLidarPointCloudServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorLidarPointCloudServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "SensorLidarPointCloudServiceInterfaceProxy",
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

    if ((initHandler_) && (!initHandler_())) {
        return false;
    }

    initFlag_ = true;
    return true;
}

void LidarTerrainMapDetectionSwC::Stop()
{
    workFlag_ = false;
    /* server agent */
    for (auto id : perceptionStaticEnvServiceInterfaceServerIdMap_) {
        if (perceptionStaticEnvServiceInterfaceServerInsMap_[id.first]) {
            perceptionStaticEnvServiceInterfaceServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    /* client agent */
    for (auto id : sensorLidarPointCloudServiceInterfaceClientIdMap_) {
        if (sensorLidarPointCloudServiceInterfaceClientInsMap_[id.first]) {
            sensorLidarPointCloudServiceInterfaceClientInsMap_[id.first]->Stop();
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

    if (stopHandler_) {
        stopHandler_();
    }
    return;
}


std::shared_ptr<PerceptionStaticEnvServiceInterfaceServer> LidarTerrainMapDetectionSwC::GetPerceptionStaticEnvServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(perceptionStaticEnvServiceInterfaceServerInsMapMtx_);
    if (perceptionStaticEnvServiceInterfaceServerInsMap_.find(portName) == perceptionStaticEnvServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(perceptionStaticEnvServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return perceptionStaticEnvServiceInterfaceServerInsMap_[portName];
}
std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient> LidarTerrainMapDetectionSwC::GetSensorLidarPointCloudServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorLidarPointCloudServiceInterfaceClientInsMapMtx_);
    if (sensorLidarPointCloudServiceInterfaceClientInsMap_.find(portName) == sensorLidarPointCloudServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorLidarPointCloudServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorLidarPointCloudServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<LocationInfoIntfClient> LidarTerrainMapDetectionSwC::GetLocationInfoIntfClient(const ara::core::String& portName)
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

bool LidarTerrainMapDetectionSwC::LoadConfig()
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

bool LidarTerrainMapDetectionSwC::DoesFileExist(const ara::core::String& filePath)
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

bool LidarTerrainMapDetectionSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool LidarTerrainMapDetectionSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("lidar_terrain_map_detection")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find lidar_terrain_map_detection in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["lidar_terrain_map_detection"];
    /* server agent */
    if (!swcConfig.HasKeyValue("sendInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find sendInstanceID in config file!";
        return false;
    }
    swcConfig["sendInstanceID"]["PerceptionStaticEnvServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", perceptionStaticEnvServiceInterfaceServerIdVec_);
    StableUnique(perceptionStaticEnvServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["PerceptionStaticEnvServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", perceptionStaticEnvServiceInterfaceServerPortVec_);
    StableUnique(perceptionStaticEnvServiceInterfaceServerPortVec_);
    if (perceptionStaticEnvServiceInterfaceServerIdVec_.size() != perceptionStaticEnvServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < perceptionStaticEnvServiceInterfaceServerIdVec_.size(); ++i) {
        perceptionStaticEnvServiceInterfaceServerIdMap_[perceptionStaticEnvServiceInterfaceServerPortVec_[i]] = perceptionStaticEnvServiceInterfaceServerIdVec_[i];
    }

    /* client agent */
    if (!swcConfig.HasKeyValue("recvInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find recvInstanceID in config file!";
        return false;
    }
    swcConfig["recvInstanceID"]["SensorLidarPointCloudServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorLidarPointCloudServiceInterfaceClientIdVec_);
    StableUnique(sensorLidarPointCloudServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["SensorLidarPointCloudServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorLidarPointCloudServiceInterfaceClientPortVec_);
    StableUnique(sensorLidarPointCloudServiceInterfaceClientPortVec_);
    if (sensorLidarPointCloudServiceInterfaceClientIdVec_.size() != sensorLidarPointCloudServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorLidarPointCloudServiceInterfaceClientIdVec_.size(); ++i) {
        sensorLidarPointCloudServiceInterfaceClientIdMap_[sensorLidarPointCloudServiceInterfaceClientPortVec_[i]] = sensorLidarPointCloudServiceInterfaceClientIdVec_[i];
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
    return true;
}
}  /* namespace lidar_terrain_map_detection */
}  /* namespace mdc */