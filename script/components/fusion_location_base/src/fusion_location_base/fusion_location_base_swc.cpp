/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/fusion_location_base/fusion_location_base_swc.h"

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
#include "mdc/fusion_location_base_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/fusion_location_base/locationinfointf_server.h"
#include "adsfi/locationinfointf_skeleton.h"
#include "mdc/fusion_location_base/locationinfointf_client.h"
#include "mdc/fusion_location_base/sensorlidarpointcloudserviceinterface_client.h"
#include "mdc/fusion_location_base/sensorimuserviceinterface_client.h"
#include "mdc/fusion_location_base/locationserviceinterface_client.h"
#include "mdc/fusion_location_base/sensorwheelspeedserviceinterface_client.h"

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
namespace fusion_location_base {
using ara::core::String;
using LOG_SPACE = mdc::fusion_location_base::FusionLocationBaseLogger;

FusionLocationBaseSwC::FusionLocationBaseSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

FusionLocationBaseSwC::~FusionLocationBaseSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool FusionLocationBaseSwC::Init()
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
    for (auto id : locationInfoIntfServerIdMap_) {
        auto resultToken = adsfi::skeleton::LocationInfoIntfSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            locationInfoIntfServerInsMap_[id.first] = std::make_shared<LocationInfoIntfServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (locationInfoIntfServerInsMap_[id.first]!= nullptr),
            "LocationInfoIntfSkeleton",
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
    for (auto id : sensorLidarPointCloudServiceInterfaceClientIdMap_) {
        sensorLidarPointCloudServiceInterfaceClientInsMap_[id.first] = std::make_shared<SensorLidarPointCloudServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorLidarPointCloudServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "SensorLidarPointCloudServiceInterfaceProxy",
            id.second);
    }
    /* client agent */
    for (auto id : sensorImuServiceInterfaceClientIdMap_) {
        sensorImuServiceInterfaceClientInsMap_[id.first] = std::make_shared<SensorImuServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorImuServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "SensorImuServiceInterfaceProxy",
            id.second);
    }
    /* client agent */
    for (auto id : locationServiceInterfaceClientIdMap_) {
        locationServiceInterfaceClientInsMap_[id.first] = std::make_shared<LocationServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (locationServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "LocationServiceInterfaceProxy",
            id.second);
    }
    /* client agent */
    for (auto id : sensorWheelSpeedServiceInterfaceClientIdMap_) {
        sensorWheelSpeedServiceInterfaceClientInsMap_[id.first] = std::make_shared<SensorWheelSpeedServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorWheelSpeedServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "SensorWheelSpeedServiceInterfaceProxy",
            id.second);
    }

    if ((initHandler_) && (!initHandler_())) {
        return false;
    }

    initFlag_ = true;
    return true;
}

void FusionLocationBaseSwC::Stop()
{
    workFlag_ = false;
    /* server agent */
    for (auto id : locationInfoIntfServerIdMap_) {
        if (locationInfoIntfServerInsMap_[id.first]) {
            locationInfoIntfServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    /* client agent */
    for (auto id : locationInfoIntfClientIdMap_) {
        if (locationInfoIntfClientInsMap_[id.first]) {
            locationInfoIntfClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : sensorLidarPointCloudServiceInterfaceClientIdMap_) {
        if (sensorLidarPointCloudServiceInterfaceClientInsMap_[id.first]) {
            sensorLidarPointCloudServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : sensorImuServiceInterfaceClientIdMap_) {
        if (sensorImuServiceInterfaceClientInsMap_[id.first]) {
            sensorImuServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : locationServiceInterfaceClientIdMap_) {
        if (locationServiceInterfaceClientInsMap_[id.first]) {
            locationServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : sensorWheelSpeedServiceInterfaceClientIdMap_) {
        if (sensorWheelSpeedServiceInterfaceClientInsMap_[id.first]) {
            sensorWheelSpeedServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    if (stopHandler_) {
        stopHandler_();
    }
    return;
}


std::shared_ptr<LocationInfoIntfServer> FusionLocationBaseSwC::GetLocationInfoIntfServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(locationInfoIntfServerInsMapMtx_);
    if (locationInfoIntfServerInsMap_.find(portName) == locationInfoIntfServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(locationInfoIntfServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return locationInfoIntfServerInsMap_[portName];
}
std::shared_ptr<LocationInfoIntfClient> FusionLocationBaseSwC::GetLocationInfoIntfClient(const ara::core::String& portName)
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
std::shared_ptr<SensorLidarPointCloudServiceInterfaceClient> FusionLocationBaseSwC::GetSensorLidarPointCloudServiceInterfaceClient(const ara::core::String& portName)
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
std::shared_ptr<SensorImuServiceInterfaceClient> FusionLocationBaseSwC::GetSensorImuServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorImuServiceInterfaceClientInsMapMtx_);
    if (sensorImuServiceInterfaceClientInsMap_.find(portName) == sensorImuServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorImuServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorImuServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<LocationServiceInterfaceClient> FusionLocationBaseSwC::GetLocationServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(locationServiceInterfaceClientInsMapMtx_);
    if (locationServiceInterfaceClientInsMap_.find(portName) == locationServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(locationServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return locationServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<SensorWheelSpeedServiceInterfaceClient> FusionLocationBaseSwC::GetSensorWheelSpeedServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorWheelSpeedServiceInterfaceClientInsMapMtx_);
    if (sensorWheelSpeedServiceInterfaceClientInsMap_.find(portName) == sensorWheelSpeedServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorWheelSpeedServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorWheelSpeedServiceInterfaceClientInsMap_[portName];
}

bool FusionLocationBaseSwC::LoadConfig()
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

bool FusionLocationBaseSwC::DoesFileExist(const ara::core::String& filePath)
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

bool FusionLocationBaseSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool FusionLocationBaseSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("fusion_location_base")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find fusion_location_base in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["fusion_location_base"];
    /* server agent */
    if (!swcConfig.HasKeyValue("sendInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find sendInstanceID in config file!";
        return false;
    }
    swcConfig["sendInstanceID"]["LocationInfoIntf"].GetValue<std::vector<uint32_t>>(
        "instanceId", locationInfoIntfServerIdVec_);
    StableUnique(locationInfoIntfServerIdVec_);
    swcConfig["sendInstanceID"]["LocationInfoIntf"].GetValue<std::vector<std::string>>(
        "portName", locationInfoIntfServerPortVec_);
    StableUnique(locationInfoIntfServerPortVec_);
    if (locationInfoIntfServerIdVec_.size() != locationInfoIntfServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < locationInfoIntfServerIdVec_.size(); ++i) {
        locationInfoIntfServerIdMap_[locationInfoIntfServerPortVec_[i]] = locationInfoIntfServerIdVec_[i];
    }

    /* client agent */
    if (!swcConfig.HasKeyValue("recvInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find recvInstanceID in config file!";
        return false;
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
    swcConfig["recvInstanceID"]["SensorImuServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorImuServiceInterfaceClientIdVec_);
    StableUnique(sensorImuServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["SensorImuServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorImuServiceInterfaceClientPortVec_);
    StableUnique(sensorImuServiceInterfaceClientPortVec_);
    if (sensorImuServiceInterfaceClientIdVec_.size() != sensorImuServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorImuServiceInterfaceClientIdVec_.size(); ++i) {
        sensorImuServiceInterfaceClientIdMap_[sensorImuServiceInterfaceClientPortVec_[i]] = sensorImuServiceInterfaceClientIdVec_[i];
    }
    swcConfig["recvInstanceID"]["LocationServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", locationServiceInterfaceClientIdVec_);
    StableUnique(locationServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["LocationServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", locationServiceInterfaceClientPortVec_);
    StableUnique(locationServiceInterfaceClientPortVec_);
    if (locationServiceInterfaceClientIdVec_.size() != locationServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < locationServiceInterfaceClientIdVec_.size(); ++i) {
        locationServiceInterfaceClientIdMap_[locationServiceInterfaceClientPortVec_[i]] = locationServiceInterfaceClientIdVec_[i];
    }
    swcConfig["recvInstanceID"]["SensorWheelSpeedServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorWheelSpeedServiceInterfaceClientIdVec_);
    StableUnique(sensorWheelSpeedServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["SensorWheelSpeedServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorWheelSpeedServiceInterfaceClientPortVec_);
    StableUnique(sensorWheelSpeedServiceInterfaceClientPortVec_);
    if (sensorWheelSpeedServiceInterfaceClientIdVec_.size() != sensorWheelSpeedServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorWheelSpeedServiceInterfaceClientIdVec_.size(); ++i) {
        sensorWheelSpeedServiceInterfaceClientIdMap_[sensorWheelSpeedServiceInterfaceClientPortVec_[i]] = sensorWheelSpeedServiceInterfaceClientIdVec_[i];
    }
    return true;
}
}  /* namespace fusion_location_base */
}  /* namespace mdc */