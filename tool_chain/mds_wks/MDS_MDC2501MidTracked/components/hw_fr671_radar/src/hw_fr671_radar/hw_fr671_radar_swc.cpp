/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/hw_fr671_radar/hw_fr671_radar_swc.h"

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
#include "mdc/hw_fr671_radar_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/hw_fr671_radar/radarpointcloudserviceinterface_server.h"
#include "ara/adsfi/radarpointcloudserviceinterface_skeleton.h"
#include "mdc/hw_fr671_radar/sensorradarpacketserviceinterface_server.h"
#include "ara/adsfi/sensorradarpacketserviceinterface_skeleton.h"
#include "mdc/hw_fr671_radar/locationserviceinterface_client.h"
#include "mdc/hw_fr671_radar/vehicleinfomationserviceinterface_client.h"

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
namespace hw_fr671_radar {
using ara::core::String;
using LOG_SPACE = mdc::hw_fr671_radar::HwFr671RadarLogger;

HwFr671RadarSwC::HwFr671RadarSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

HwFr671RadarSwC::~HwFr671RadarSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool HwFr671RadarSwC::Init()
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
    for (auto id : radarPointCloudServiceInterfaceServerIdMap_) {
        auto resultToken = ara::adsfi::skeleton::RadarPointCloudServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            radarPointCloudServiceInterfaceServerInsMap_[id.first] = std::make_shared<RadarPointCloudServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (radarPointCloudServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "RadarPointCloudServiceInterfaceSkeleton",
            id.second);
    }
    /* server agent */
    for (auto id : sensorRadarPacketServiceInterfaceServerIdMap_) {
        auto resultToken = ara::adsfi::skeleton::SensorRadarPacketServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            sensorRadarPacketServiceInterfaceServerInsMap_[id.first] = std::make_shared<SensorRadarPacketServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorRadarPacketServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "SensorRadarPacketServiceInterfaceSkeleton",
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

void HwFr671RadarSwC::Stop()
{
    workFlag_ = false;
    /* server agent */
    for (auto id : radarPointCloudServiceInterfaceServerIdMap_) {
        if (radarPointCloudServiceInterfaceServerInsMap_[id.first]) {
            radarPointCloudServiceInterfaceServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : sensorRadarPacketServiceInterfaceServerIdMap_) {
        if (sensorRadarPacketServiceInterfaceServerInsMap_[id.first]) {
            sensorRadarPacketServiceInterfaceServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    /* client agent */
    for (auto id : locationServiceInterfaceClientIdMap_) {
        if (locationServiceInterfaceClientInsMap_[id.first]) {
            locationServiceInterfaceClientInsMap_[id.first]->Stop();
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


std::shared_ptr<RadarPointCloudServiceInterfaceServer> HwFr671RadarSwC::GetRadarPointCloudServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(radarPointCloudServiceInterfaceServerInsMapMtx_);
    if (radarPointCloudServiceInterfaceServerInsMap_.find(portName) == radarPointCloudServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(radarPointCloudServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return radarPointCloudServiceInterfaceServerInsMap_[portName];
}
std::shared_ptr<SensorRadarPacketServiceInterfaceServer> HwFr671RadarSwC::GetSensorRadarPacketServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorRadarPacketServiceInterfaceServerInsMapMtx_);
    if (sensorRadarPacketServiceInterfaceServerInsMap_.find(portName) == sensorRadarPacketServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorRadarPacketServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorRadarPacketServiceInterfaceServerInsMap_[portName];
}
std::shared_ptr<LocationServiceInterfaceClient> HwFr671RadarSwC::GetLocationServiceInterfaceClient(const ara::core::String& portName)
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
std::shared_ptr<VehicleInfomationServiceInterfaceClient> HwFr671RadarSwC::GetVehicleInfomationServiceInterfaceClient(const ara::core::String& portName)
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

bool HwFr671RadarSwC::LoadConfig()
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

bool HwFr671RadarSwC::DoesFileExist(const ara::core::String& filePath)
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

bool HwFr671RadarSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool HwFr671RadarSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("hw_fr671_radar")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find hw_fr671_radar in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["hw_fr671_radar"];
    /* server agent */
    if (!swcConfig.HasKeyValue("sendInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find sendInstanceID in config file!";
        return false;
    }
    swcConfig["sendInstanceID"]["RadarPointCloudServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", radarPointCloudServiceInterfaceServerIdVec_);
    StableUnique(radarPointCloudServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["RadarPointCloudServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", radarPointCloudServiceInterfaceServerPortVec_);
    StableUnique(radarPointCloudServiceInterfaceServerPortVec_);
    if (radarPointCloudServiceInterfaceServerIdVec_.size() != radarPointCloudServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < radarPointCloudServiceInterfaceServerIdVec_.size(); ++i) {
        radarPointCloudServiceInterfaceServerIdMap_[radarPointCloudServiceInterfaceServerPortVec_[i]] = radarPointCloudServiceInterfaceServerIdVec_[i];
    }
    swcConfig["sendInstanceID"]["SensorRadarPacketServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorRadarPacketServiceInterfaceServerIdVec_);
    StableUnique(sensorRadarPacketServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["SensorRadarPacketServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorRadarPacketServiceInterfaceServerPortVec_);
    StableUnique(sensorRadarPacketServiceInterfaceServerPortVec_);
    if (sensorRadarPacketServiceInterfaceServerIdVec_.size() != sensorRadarPacketServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorRadarPacketServiceInterfaceServerIdVec_.size(); ++i) {
        sensorRadarPacketServiceInterfaceServerIdMap_[sensorRadarPacketServiceInterfaceServerPortVec_[i]] = sensorRadarPacketServiceInterfaceServerIdVec_[i];
    }

    /* client agent */
    if (!swcConfig.HasKeyValue("recvInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find recvInstanceID in config file!";
        return false;
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
}  /* namespace hw_fr671_radar */
}  /* namespace mdc */