/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/bd21_imu/bd21_imu_swc.h"

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
#include "mdc/bd21_imu_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/bd21_imu/locationserviceinterface_server.h"
#include "mdc/location/locationserviceinterface_skeleton.h"
#include "mdc/bd21_imu/sensorwheelspeedserviceinterface_server.h"
#include "ara/adsfi/sensorwheelspeedserviceinterface_skeleton.h"
#include "mdc/bd21_imu/sensorgpsserviceinterface_server.h"
#include "ara/adsfi/sensorgpsserviceinterface_skeleton.h"
#include "mdc/bd21_imu/sensorimuserviceinterface_server.h"
#include "ara/adsfi/sensorimuserviceinterface_skeleton.h"

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
namespace bd21_imu {
using ara::core::String;
using LOG_SPACE = mdc::bd21_imu::Bd21ImuLogger;

Bd21ImuSwC::Bd21ImuSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

Bd21ImuSwC::~Bd21ImuSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool Bd21ImuSwC::Init()
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
    for (auto id : locationServiceInterfaceServerIdMap_) {
        auto resultToken = mdc::location::skeleton::LocationServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            locationServiceInterfaceServerInsMap_[id.first] = std::make_shared<LocationServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (locationServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "LocationServiceInterfaceSkeleton",
            id.second);
    }
    /* server agent */
    for (auto id : sensorWheelSpeedServiceInterfaceServerIdMap_) {
        auto resultToken = ara::adsfi::skeleton::SensorWheelSpeedServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            sensorWheelSpeedServiceInterfaceServerInsMap_[id.first] = std::make_shared<SensorWheelSpeedServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorWheelSpeedServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "SensorWheelSpeedServiceInterfaceSkeleton",
            id.second);
    }
    /* server agent */
    for (auto id : sensorGpsServiceInterfaceServerIdMap_) {
        auto resultToken = ara::adsfi::skeleton::SensorGpsServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            sensorGpsServiceInterfaceServerInsMap_[id.first] = std::make_shared<SensorGpsServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorGpsServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "SensorGpsServiceInterfaceSkeleton",
            id.second);
    }
    /* server agent */
    for (auto id : sensorImuServiceInterfaceServerIdMap_) {
        auto resultToken = ara::adsfi::skeleton::SensorImuServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            sensorImuServiceInterfaceServerInsMap_[id.first] = std::make_shared<SensorImuServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorImuServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "SensorImuServiceInterfaceSkeleton",
            id.second);
    }

    if ((initHandler_) && (!initHandler_())) {
        return false;
    }

    initFlag_ = true;
    return true;
}

void Bd21ImuSwC::Stop()
{
    workFlag_ = false;
    /* server agent */
    for (auto id : locationServiceInterfaceServerIdMap_) {
        if (locationServiceInterfaceServerInsMap_[id.first]) {
            locationServiceInterfaceServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : sensorWheelSpeedServiceInterfaceServerIdMap_) {
        if (sensorWheelSpeedServiceInterfaceServerInsMap_[id.first]) {
            sensorWheelSpeedServiceInterfaceServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : sensorGpsServiceInterfaceServerIdMap_) {
        if (sensorGpsServiceInterfaceServerInsMap_[id.first]) {
            sensorGpsServiceInterfaceServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : sensorImuServiceInterfaceServerIdMap_) {
        if (sensorImuServiceInterfaceServerInsMap_[id.first]) {
            sensorImuServiceInterfaceServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    /* client agent */

    if (stopHandler_) {
        stopHandler_();
    }
    return;
}


std::shared_ptr<LocationServiceInterfaceServer> Bd21ImuSwC::GetLocationServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(locationServiceInterfaceServerInsMapMtx_);
    if (locationServiceInterfaceServerInsMap_.find(portName) == locationServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(locationServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return locationServiceInterfaceServerInsMap_[portName];
}
std::shared_ptr<SensorWheelSpeedServiceInterfaceServer> Bd21ImuSwC::GetSensorWheelSpeedServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorWheelSpeedServiceInterfaceServerInsMapMtx_);
    if (sensorWheelSpeedServiceInterfaceServerInsMap_.find(portName) == sensorWheelSpeedServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorWheelSpeedServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorWheelSpeedServiceInterfaceServerInsMap_[portName];
}
std::shared_ptr<SensorGpsServiceInterfaceServer> Bd21ImuSwC::GetSensorGpsServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorGpsServiceInterfaceServerInsMapMtx_);
    if (sensorGpsServiceInterfaceServerInsMap_.find(portName) == sensorGpsServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorGpsServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorGpsServiceInterfaceServerInsMap_[portName];
}
std::shared_ptr<SensorImuServiceInterfaceServer> Bd21ImuSwC::GetSensorImuServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorImuServiceInterfaceServerInsMapMtx_);
    if (sensorImuServiceInterfaceServerInsMap_.find(portName) == sensorImuServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorImuServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorImuServiceInterfaceServerInsMap_[portName];
}

bool Bd21ImuSwC::LoadConfig()
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

bool Bd21ImuSwC::DoesFileExist(const ara::core::String& filePath)
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

bool Bd21ImuSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool Bd21ImuSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("bd21_imu")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find bd21_imu in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["bd21_imu"];
    /* server agent */
    if (!swcConfig.HasKeyValue("sendInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find sendInstanceID in config file!";
        return false;
    }
    swcConfig["sendInstanceID"]["LocationServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", locationServiceInterfaceServerIdVec_);
    StableUnique(locationServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["LocationServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", locationServiceInterfaceServerPortVec_);
    StableUnique(locationServiceInterfaceServerPortVec_);
    if (locationServiceInterfaceServerIdVec_.size() != locationServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < locationServiceInterfaceServerIdVec_.size(); ++i) {
        locationServiceInterfaceServerIdMap_[locationServiceInterfaceServerPortVec_[i]] = locationServiceInterfaceServerIdVec_[i];
    }
    swcConfig["sendInstanceID"]["SensorWheelSpeedServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorWheelSpeedServiceInterfaceServerIdVec_);
    StableUnique(sensorWheelSpeedServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["SensorWheelSpeedServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorWheelSpeedServiceInterfaceServerPortVec_);
    StableUnique(sensorWheelSpeedServiceInterfaceServerPortVec_);
    if (sensorWheelSpeedServiceInterfaceServerIdVec_.size() != sensorWheelSpeedServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorWheelSpeedServiceInterfaceServerIdVec_.size(); ++i) {
        sensorWheelSpeedServiceInterfaceServerIdMap_[sensorWheelSpeedServiceInterfaceServerPortVec_[i]] = sensorWheelSpeedServiceInterfaceServerIdVec_[i];
    }
    swcConfig["sendInstanceID"]["SensorGpsServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorGpsServiceInterfaceServerIdVec_);
    StableUnique(sensorGpsServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["SensorGpsServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorGpsServiceInterfaceServerPortVec_);
    StableUnique(sensorGpsServiceInterfaceServerPortVec_);
    if (sensorGpsServiceInterfaceServerIdVec_.size() != sensorGpsServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorGpsServiceInterfaceServerIdVec_.size(); ++i) {
        sensorGpsServiceInterfaceServerIdMap_[sensorGpsServiceInterfaceServerPortVec_[i]] = sensorGpsServiceInterfaceServerIdVec_[i];
    }
    swcConfig["sendInstanceID"]["SensorImuServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorImuServiceInterfaceServerIdVec_);
    StableUnique(sensorImuServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["SensorImuServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorImuServiceInterfaceServerPortVec_);
    StableUnique(sensorImuServiceInterfaceServerPortVec_);
    if (sensorImuServiceInterfaceServerIdVec_.size() != sensorImuServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorImuServiceInterfaceServerIdVec_.size(); ++i) {
        sensorImuServiceInterfaceServerIdMap_[sensorImuServiceInterfaceServerPortVec_[i]] = sensorImuServiceInterfaceServerIdVec_[i];
    }

    /* client agent */
    if (!swcConfig.HasKeyValue("recvInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find recvInstanceID in config file!";
        return false;
    }
    return true;
}
}  /* namespace bd21_imu */
}  /* namespace mdc */