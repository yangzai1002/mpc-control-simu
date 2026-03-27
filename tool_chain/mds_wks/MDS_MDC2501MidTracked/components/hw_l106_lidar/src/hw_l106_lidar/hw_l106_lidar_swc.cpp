/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/hw_l106_lidar/hw_l106_lidar_swc.h"

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
#include "mdc/hw_l106_lidar_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/hw_l106_lidar/sensorlidarpointcloudserviceinterface_server.h"
#include "ara/adsfi/sensorlidarpointcloudserviceinterface_skeleton.h"
#include "mdc/hw_l106_lidar/sensorlidarpacketserviceinterface_server.h"
#include "ara/adsfi/sensorlidarpacketserviceinterface_skeleton.h"

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
namespace hw_l106_lidar {
using ara::core::String;
using LOG_SPACE = mdc::hw_l106_lidar::HwL106LidarLogger;

HwL106LidarSwC::HwL106LidarSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

HwL106LidarSwC::~HwL106LidarSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool HwL106LidarSwC::Init()
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
    for (auto id : sensorLidarPointCloudServiceInterfaceServerIdMap_) {
        auto resultToken = ara::adsfi::skeleton::SensorLidarPointCloudServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            sensorLidarPointCloudServiceInterfaceServerInsMap_[id.first] = std::make_shared<SensorLidarPointCloudServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorLidarPointCloudServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "SensorLidarPointCloudServiceInterfaceSkeleton",
            id.second);
    }
    /* server agent */
    for (auto id : sensorLidarPacketServiceInterfaceServerIdMap_) {
        auto resultToken = ara::adsfi::skeleton::SensorLidarPacketServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            sensorLidarPacketServiceInterfaceServerInsMap_[id.first] = std::make_shared<SensorLidarPacketServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorLidarPacketServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "SensorLidarPacketServiceInterfaceSkeleton",
            id.second);
    }

    if ((initHandler_) && (!initHandler_())) {
        return false;
    }

    initFlag_ = true;
    return true;
}

void HwL106LidarSwC::Stop()
{
    workFlag_ = false;
    /* server agent */
    for (auto id : sensorLidarPointCloudServiceInterfaceServerIdMap_) {
        if (sensorLidarPointCloudServiceInterfaceServerInsMap_[id.first]) {
            sensorLidarPointCloudServiceInterfaceServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : sensorLidarPacketServiceInterfaceServerIdMap_) {
        if (sensorLidarPacketServiceInterfaceServerInsMap_[id.first]) {
            sensorLidarPacketServiceInterfaceServerInsMap_[id.first]->Stop();
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


std::shared_ptr<SensorLidarPointCloudServiceInterfaceServer> HwL106LidarSwC::GetSensorLidarPointCloudServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorLidarPointCloudServiceInterfaceServerInsMapMtx_);
    if (sensorLidarPointCloudServiceInterfaceServerInsMap_.find(portName) == sensorLidarPointCloudServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorLidarPointCloudServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorLidarPointCloudServiceInterfaceServerInsMap_[portName];
}
std::shared_ptr<SensorLidarPacketServiceInterfaceServer> HwL106LidarSwC::GetSensorLidarPacketServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorLidarPacketServiceInterfaceServerInsMapMtx_);
    if (sensorLidarPacketServiceInterfaceServerInsMap_.find(portName) == sensorLidarPacketServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorLidarPacketServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorLidarPacketServiceInterfaceServerInsMap_[portName];
}

bool HwL106LidarSwC::LoadConfig()
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

bool HwL106LidarSwC::DoesFileExist(const ara::core::String& filePath)
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

bool HwL106LidarSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool HwL106LidarSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("hw_l106_lidar")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find hw_l106_lidar in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["hw_l106_lidar"];
    /* server agent */
    if (!swcConfig.HasKeyValue("sendInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find sendInstanceID in config file!";
        return false;
    }
    swcConfig["sendInstanceID"]["SensorLidarPointCloudServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorLidarPointCloudServiceInterfaceServerIdVec_);
    StableUnique(sensorLidarPointCloudServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["SensorLidarPointCloudServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorLidarPointCloudServiceInterfaceServerPortVec_);
    StableUnique(sensorLidarPointCloudServiceInterfaceServerPortVec_);
    if (sensorLidarPointCloudServiceInterfaceServerIdVec_.size() != sensorLidarPointCloudServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorLidarPointCloudServiceInterfaceServerIdVec_.size(); ++i) {
        sensorLidarPointCloudServiceInterfaceServerIdMap_[sensorLidarPointCloudServiceInterfaceServerPortVec_[i]] = sensorLidarPointCloudServiceInterfaceServerIdVec_[i];
    }
    swcConfig["sendInstanceID"]["SensorLidarPacketServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorLidarPacketServiceInterfaceServerIdVec_);
    StableUnique(sensorLidarPacketServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["SensorLidarPacketServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorLidarPacketServiceInterfaceServerPortVec_);
    StableUnique(sensorLidarPacketServiceInterfaceServerPortVec_);
    if (sensorLidarPacketServiceInterfaceServerIdVec_.size() != sensorLidarPacketServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorLidarPacketServiceInterfaceServerIdVec_.size(); ++i) {
        sensorLidarPacketServiceInterfaceServerIdMap_[sensorLidarPacketServiceInterfaceServerPortVec_[i]] = sensorLidarPacketServiceInterfaceServerIdVec_[i];
    }

    /* client agent */
    if (!swcConfig.HasKeyValue("recvInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find recvInstanceID in config file!";
        return false;
    }
    return true;
}
}  /* namespace hw_l106_lidar */
}  /* namespace mdc */