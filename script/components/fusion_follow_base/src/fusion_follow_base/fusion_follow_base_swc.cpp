/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/fusion_follow_base/fusion_follow_base_swc.h"

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
#include "mdc/fusion_follow_base_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/fusion_follow_base/fusionobjectarrayintf_server.h"
#include "adsfi/fusionobjectarrayintf_skeleton.h"
#include "mdc/fusion_follow_base/sensorlidarpointcloudserviceinterface_client.h"
#include "mdc/fusion_follow_base/radarpointcloudserviceinterface_client.h"
#include "mdc/fusion_follow_base/locationinfointf_client.h"
#include "mdc/fusion_follow_base/object3darrayintf_client.h"
#include "mdc/fusion_follow_base/visionsegmentserviceinterface_client.h"

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
namespace fusion_follow_base {
using ara::core::String;
using LOG_SPACE = mdc::fusion_follow_base::FusionFollowBaseLogger;

FusionFollowBaseSwC::FusionFollowBaseSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

FusionFollowBaseSwC::~FusionFollowBaseSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool FusionFollowBaseSwC::Init()
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
    for (auto id : fusionObjectArrayIntfServerIdMap_) {
        auto resultToken = adsfi::skeleton::FusionObjectArrayIntfSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            fusionObjectArrayIntfServerInsMap_[id.first] = std::make_shared<FusionObjectArrayIntfServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (fusionObjectArrayIntfServerInsMap_[id.first]!= nullptr),
            "FusionObjectArrayIntfSkeleton",
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
    for (auto id : radarPointCloudServiceInterfaceClientIdMap_) {
        radarPointCloudServiceInterfaceClientInsMap_[id.first] = std::make_shared<RadarPointCloudServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (radarPointCloudServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "RadarPointCloudServiceInterfaceProxy",
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
    for (auto id : object3dArrayIntfClientIdMap_) {
        object3dArrayIntfClientInsMap_[id.first] = std::make_shared<Object3dArrayIntfClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (object3dArrayIntfClientInsMap_[id.first]!= nullptr),
            "Object3dArrayIntfProxy",
            id.second);
    }
    /* client agent */
    for (auto id : visionSegmentServiceInterfaceClientIdMap_) {
        visionSegmentServiceInterfaceClientInsMap_[id.first] = std::make_shared<VisionSegmentServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (visionSegmentServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "VisionSegmentServiceInterfaceProxy",
            id.second);
    }

    if ((initHandler_) && (!initHandler_())) {
        return false;
    }

    initFlag_ = true;
    return true;
}

void FusionFollowBaseSwC::Stop()
{
    workFlag_ = false;
    /* server agent */
    for (auto id : fusionObjectArrayIntfServerIdMap_) {
        if (fusionObjectArrayIntfServerInsMap_[id.first]) {
            fusionObjectArrayIntfServerInsMap_[id.first]->Stop();
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
    for (auto id : radarPointCloudServiceInterfaceClientIdMap_) {
        if (radarPointCloudServiceInterfaceClientInsMap_[id.first]) {
            radarPointCloudServiceInterfaceClientInsMap_[id.first]->Stop();
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
    for (auto id : object3dArrayIntfClientIdMap_) {
        if (object3dArrayIntfClientInsMap_[id.first]) {
            object3dArrayIntfClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : visionSegmentServiceInterfaceClientIdMap_) {
        if (visionSegmentServiceInterfaceClientInsMap_[id.first]) {
            visionSegmentServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    if (stopHandler_) {
        stopHandler_();
    }
    return;
}


std::shared_ptr<FusionObjectArrayIntfServer> FusionFollowBaseSwC::GetFusionObjectArrayIntfServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(fusionObjectArrayIntfServerInsMapMtx_);
    if (fusionObjectArrayIntfServerInsMap_.find(portName) == fusionObjectArrayIntfServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(fusionObjectArrayIntfServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return fusionObjectArrayIntfServerInsMap_[portName];
}
std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient> FusionFollowBaseSwC::GetSensorLidarPointCloudServiceInterfaceClient(const ara::core::String& portName)
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
std::shared_ptr<RadarPointCloudServiceInterfaceClient> FusionFollowBaseSwC::GetRadarPointCloudServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(radarPointCloudServiceInterfaceClientInsMapMtx_);
    if (radarPointCloudServiceInterfaceClientInsMap_.find(portName) == radarPointCloudServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(radarPointCloudServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return radarPointCloudServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<LocationInfoIntfClient> FusionFollowBaseSwC::GetLocationInfoIntfClient(const ara::core::String& portName)
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
std::shared_ptr<Object3dArrayIntfClient> FusionFollowBaseSwC::GetObject3dArrayIntfClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(object3dArrayIntfClientInsMapMtx_);
    if (object3dArrayIntfClientInsMap_.find(portName) == object3dArrayIntfClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(object3dArrayIntfClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return object3dArrayIntfClientInsMap_[portName];
}
std::shared_ptr<VisionSegmentServiceInterfaceClient> FusionFollowBaseSwC::GetVisionSegmentServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(visionSegmentServiceInterfaceClientInsMapMtx_);
    if (visionSegmentServiceInterfaceClientInsMap_.find(portName) == visionSegmentServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(visionSegmentServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return visionSegmentServiceInterfaceClientInsMap_[portName];
}

bool FusionFollowBaseSwC::LoadConfig()
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

bool FusionFollowBaseSwC::DoesFileExist(const ara::core::String& filePath)
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

bool FusionFollowBaseSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool FusionFollowBaseSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("fusion_follow_base")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find fusion_follow_base in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["fusion_follow_base"];
    /* server agent */
    if (!swcConfig.HasKeyValue("sendInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find sendInstanceID in config file!";
        return false;
    }
    swcConfig["sendInstanceID"]["FusionObjectArrayIntf"].GetValue<std::vector<uint32_t>>(
        "instanceId", fusionObjectArrayIntfServerIdVec_);
    StableUnique(fusionObjectArrayIntfServerIdVec_);
    swcConfig["sendInstanceID"]["FusionObjectArrayIntf"].GetValue<std::vector<std::string>>(
        "portName", fusionObjectArrayIntfServerPortVec_);
    StableUnique(fusionObjectArrayIntfServerPortVec_);
    if (fusionObjectArrayIntfServerIdVec_.size() != fusionObjectArrayIntfServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < fusionObjectArrayIntfServerIdVec_.size(); ++i) {
        fusionObjectArrayIntfServerIdMap_[fusionObjectArrayIntfServerPortVec_[i]] = fusionObjectArrayIntfServerIdVec_[i];
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
    swcConfig["recvInstanceID"]["RadarPointCloudServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", radarPointCloudServiceInterfaceClientIdVec_);
    StableUnique(radarPointCloudServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["RadarPointCloudServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", radarPointCloudServiceInterfaceClientPortVec_);
    StableUnique(radarPointCloudServiceInterfaceClientPortVec_);
    if (radarPointCloudServiceInterfaceClientIdVec_.size() != radarPointCloudServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < radarPointCloudServiceInterfaceClientIdVec_.size(); ++i) {
        radarPointCloudServiceInterfaceClientIdMap_[radarPointCloudServiceInterfaceClientPortVec_[i]] = radarPointCloudServiceInterfaceClientIdVec_[i];
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
    swcConfig["recvInstanceID"]["Object3dArrayIntf"].GetValue<std::vector<uint32_t>>(
        "instanceId", object3dArrayIntfClientIdVec_);
    StableUnique(object3dArrayIntfClientIdVec_);
    swcConfig["recvInstanceID"]["Object3dArrayIntf"].GetValue<std::vector<std::string>>(
        "portName", object3dArrayIntfClientPortVec_);
    StableUnique(object3dArrayIntfClientPortVec_);
    if (object3dArrayIntfClientIdVec_.size() != object3dArrayIntfClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < object3dArrayIntfClientIdVec_.size(); ++i) {
        object3dArrayIntfClientIdMap_[object3dArrayIntfClientPortVec_[i]] = object3dArrayIntfClientIdVec_[i];
    }
    swcConfig["recvInstanceID"]["VisionSegmentServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", visionSegmentServiceInterfaceClientIdVec_);
    StableUnique(visionSegmentServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["VisionSegmentServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", visionSegmentServiceInterfaceClientPortVec_);
    StableUnique(visionSegmentServiceInterfaceClientPortVec_);
    if (visionSegmentServiceInterfaceClientIdVec_.size() != visionSegmentServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < visionSegmentServiceInterfaceClientIdVec_.size(); ++i) {
        visionSegmentServiceInterfaceClientIdMap_[visionSegmentServiceInterfaceClientPortVec_[i]] = visionSegmentServiceInterfaceClientIdVec_[i];
    }
    return true;
}
}  /* namespace fusion_follow_base */
}  /* namespace mdc */