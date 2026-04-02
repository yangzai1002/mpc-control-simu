/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/driver_dev_null/driver_dev_null_swc.h"

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
#include "mdc/driver_dev_null_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/driver_dev_null/sensorcompressedimagelistserviceinterface_client.h"
#include "mdc/driver_dev_null/sensorradarpacketserviceinterface_client.h"
#include "mdc/driver_dev_null/sensorlidarpacketserviceinterface_client.h"

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
namespace driver_dev_null {
using ara::core::String;
using LOG_SPACE = mdc::driver_dev_null::DriverDevNullLogger;

DriverDevNullSwC::DriverDevNullSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

DriverDevNullSwC::~DriverDevNullSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool DriverDevNullSwC::Init()
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
    /* client agent */
    for (auto id : sensorCompressedImageListServiceInterfaceClientIdMap_) {
        sensorCompressedImageListServiceInterfaceClientInsMap_[id.first] = std::make_shared<SensorCompressedImageListServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorCompressedImageListServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "SensorCompressedImageListServiceInterfaceProxy",
            id.second);
    }
    /* client agent */
    for (auto id : sensorRadarPacketServiceInterfaceClientIdMap_) {
        sensorRadarPacketServiceInterfaceClientInsMap_[id.first] = std::make_shared<SensorRadarPacketServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorRadarPacketServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "SensorRadarPacketServiceInterfaceProxy",
            id.second);
    }
    /* client agent */
    for (auto id : sensorLidarPacketServiceInterfaceClientIdMap_) {
        sensorLidarPacketServiceInterfaceClientInsMap_[id.first] = std::make_shared<SensorLidarPacketServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorLidarPacketServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "SensorLidarPacketServiceInterfaceProxy",
            id.second);
    }

    if ((initHandler_) && (!initHandler_())) {
        return false;
    }

    initFlag_ = true;
    return true;
}

void DriverDevNullSwC::Stop()
{
    workFlag_ = false;
    /* server agent */

    /* client agent */
    for (auto id : sensorCompressedImageListServiceInterfaceClientIdMap_) {
        if (sensorCompressedImageListServiceInterfaceClientInsMap_[id.first]) {
            sensorCompressedImageListServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : sensorRadarPacketServiceInterfaceClientIdMap_) {
        if (sensorRadarPacketServiceInterfaceClientInsMap_[id.first]) {
            sensorRadarPacketServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : sensorLidarPacketServiceInterfaceClientIdMap_) {
        if (sensorLidarPacketServiceInterfaceClientInsMap_[id.first]) {
            sensorLidarPacketServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    if (stopHandler_) {
        stopHandler_();
    }
    return;
}


std::shared_ptr<SensorCompressedImageListServiceInterfaceClient> DriverDevNullSwC::GetSensorCompressedImageListServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorCompressedImageListServiceInterfaceClientInsMapMtx_);
    if (sensorCompressedImageListServiceInterfaceClientInsMap_.find(portName) == sensorCompressedImageListServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorCompressedImageListServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorCompressedImageListServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<SensorRadarPacketServiceInterfaceClient> DriverDevNullSwC::GetSensorRadarPacketServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorRadarPacketServiceInterfaceClientInsMapMtx_);
    if (sensorRadarPacketServiceInterfaceClientInsMap_.find(portName) == sensorRadarPacketServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorRadarPacketServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorRadarPacketServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<SensorLidarPacketServiceInterfaceClient> DriverDevNullSwC::GetSensorLidarPacketServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorLidarPacketServiceInterfaceClientInsMapMtx_);
    if (sensorLidarPacketServiceInterfaceClientInsMap_.find(portName) == sensorLidarPacketServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorLidarPacketServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorLidarPacketServiceInterfaceClientInsMap_[portName];
}

bool DriverDevNullSwC::LoadConfig()
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

bool DriverDevNullSwC::DoesFileExist(const ara::core::String& filePath)
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

bool DriverDevNullSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool DriverDevNullSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("driver_dev_null")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find driver_dev_null in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["driver_dev_null"];
    /* server agent */
    if (!swcConfig.HasKeyValue("sendInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find sendInstanceID in config file!";
        return false;
    }

    /* client agent */
    if (!swcConfig.HasKeyValue("recvInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find recvInstanceID in config file!";
        return false;
    }
    swcConfig["recvInstanceID"]["SensorCompressedImageListServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorCompressedImageListServiceInterfaceClientIdVec_);
    StableUnique(sensorCompressedImageListServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["SensorCompressedImageListServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorCompressedImageListServiceInterfaceClientPortVec_);
    StableUnique(sensorCompressedImageListServiceInterfaceClientPortVec_);
    if (sensorCompressedImageListServiceInterfaceClientIdVec_.size() != sensorCompressedImageListServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorCompressedImageListServiceInterfaceClientIdVec_.size(); ++i) {
        sensorCompressedImageListServiceInterfaceClientIdMap_[sensorCompressedImageListServiceInterfaceClientPortVec_[i]] = sensorCompressedImageListServiceInterfaceClientIdVec_[i];
    }
    swcConfig["recvInstanceID"]["SensorRadarPacketServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorRadarPacketServiceInterfaceClientIdVec_);
    StableUnique(sensorRadarPacketServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["SensorRadarPacketServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorRadarPacketServiceInterfaceClientPortVec_);
    StableUnique(sensorRadarPacketServiceInterfaceClientPortVec_);
    if (sensorRadarPacketServiceInterfaceClientIdVec_.size() != sensorRadarPacketServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorRadarPacketServiceInterfaceClientIdVec_.size(); ++i) {
        sensorRadarPacketServiceInterfaceClientIdMap_[sensorRadarPacketServiceInterfaceClientPortVec_[i]] = sensorRadarPacketServiceInterfaceClientIdVec_[i];
    }
    swcConfig["recvInstanceID"]["SensorLidarPacketServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorLidarPacketServiceInterfaceClientIdVec_);
    StableUnique(sensorLidarPacketServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["SensorLidarPacketServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorLidarPacketServiceInterfaceClientPortVec_);
    StableUnique(sensorLidarPacketServiceInterfaceClientPortVec_);
    if (sensorLidarPacketServiceInterfaceClientIdVec_.size() != sensorLidarPacketServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorLidarPacketServiceInterfaceClientIdVec_.size(); ++i) {
        sensorLidarPacketServiceInterfaceClientIdMap_[sensorLidarPacketServiceInterfaceClientPortVec_[i]] = sensorLidarPacketServiceInterfaceClientIdVec_[i];
    }
    return true;
}
}  /* namespace driver_dev_null */
}  /* namespace mdc */