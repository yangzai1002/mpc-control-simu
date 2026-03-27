/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/xvehicle_status_recorder_base/xvehicle_status_recorder_base_swc.h"

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
#include "mdc/xvehicle_status_recorder_base_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/xvehicle_status_recorder_base/remotecontrolserviceinterface_client.h"
#include "mdc/xvehicle_status_recorder_base/bodycommandserviceinterface_client.h"
#include "mdc/xvehicle_status_recorder_base/locationinfointf_client.h"
#include "mdc/xvehicle_status_recorder_base/sensorgpsserviceinterface_client.h"
#include "mdc/xvehicle_status_recorder_base/vehicleinfomationserviceinterface_client.h"
#include "mdc/xvehicle_status_recorder_base/faultdataserviceinterface_client.h"
#include "mdc/xvehicle_status_recorder_base/msgimagedatalistserviceinterface_client.h"

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
namespace xvehicle_status_recorder_base {
using ara::core::String;
using LOG_SPACE = mdc::xvehicle_status_recorder_base::XvehicleStatusRecorderBaseLogger;

XvehicleStatusRecorderBaseSwC::XvehicleStatusRecorderBaseSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

XvehicleStatusRecorderBaseSwC::~XvehicleStatusRecorderBaseSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool XvehicleStatusRecorderBaseSwC::Init()
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
    for (auto id : remoteControlServiceInterfaceClientIdMap_) {
        remoteControlServiceInterfaceClientInsMap_[id.first] = std::make_shared<RemoteControlServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (remoteControlServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "RemoteControlServiceInterfaceProxy",
            id.second);
    }
    /* client agent */
    for (auto id : bodyCommandServiceInterfaceClientIdMap_) {
        bodyCommandServiceInterfaceClientInsMap_[id.first] = std::make_shared<BodyCommandServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (bodyCommandServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "BodyCommandServiceInterfaceProxy",
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
    for (auto id : sensorGpsServiceInterfaceClientIdMap_) {
        sensorGpsServiceInterfaceClientInsMap_[id.first] = std::make_shared<SensorGpsServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (sensorGpsServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "SensorGpsServiceInterfaceProxy",
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
    /* client agent */
    for (auto id : faultDataServiceInterfaceClientIdMap_) {
        faultDataServiceInterfaceClientInsMap_[id.first] = std::make_shared<FaultDataServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (faultDataServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "FaultDataServiceInterfaceProxy",
            id.second);
    }
    /* client agent */
    for (auto id : msgImageDataListServiceInterfaceClientIdMap_) {
        msgImageDataListServiceInterfaceClientInsMap_[id.first] = std::make_shared<MsgImageDataListServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (msgImageDataListServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "MsgImageDataListServiceInterfaceProxy",
            id.second);
    }

    if ((initHandler_) && (!initHandler_())) {
        return false;
    }

    initFlag_ = true;
    return true;
}

void XvehicleStatusRecorderBaseSwC::Stop()
{
    workFlag_ = false;
    /* server agent */

    /* client agent */
    for (auto id : remoteControlServiceInterfaceClientIdMap_) {
        if (remoteControlServiceInterfaceClientInsMap_[id.first]) {
            remoteControlServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : bodyCommandServiceInterfaceClientIdMap_) {
        if (bodyCommandServiceInterfaceClientInsMap_[id.first]) {
            bodyCommandServiceInterfaceClientInsMap_[id.first]->Stop();
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
    for (auto id : sensorGpsServiceInterfaceClientIdMap_) {
        if (sensorGpsServiceInterfaceClientInsMap_[id.first]) {
            sensorGpsServiceInterfaceClientInsMap_[id.first]->Stop();
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
    for (auto id : faultDataServiceInterfaceClientIdMap_) {
        if (faultDataServiceInterfaceClientInsMap_[id.first]) {
            faultDataServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }
    for (auto id : msgImageDataListServiceInterfaceClientIdMap_) {
        if (msgImageDataListServiceInterfaceClientInsMap_[id.first]) {
            msgImageDataListServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    if (stopHandler_) {
        stopHandler_();
    }
    return;
}


std::shared_ptr<RemoteControlServiceInterfaceClient> XvehicleStatusRecorderBaseSwC::GetRemoteControlServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(remoteControlServiceInterfaceClientInsMapMtx_);
    if (remoteControlServiceInterfaceClientInsMap_.find(portName) == remoteControlServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(remoteControlServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return remoteControlServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<BodyCommandServiceInterfaceClient> XvehicleStatusRecorderBaseSwC::GetBodyCommandServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(bodyCommandServiceInterfaceClientInsMapMtx_);
    if (bodyCommandServiceInterfaceClientInsMap_.find(portName) == bodyCommandServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(bodyCommandServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return bodyCommandServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<LocationInfoIntfClient> XvehicleStatusRecorderBaseSwC::GetLocationInfoIntfClient(const ara::core::String& portName)
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
std::shared_ptr<SensorGpsServiceInterfaceClient> XvehicleStatusRecorderBaseSwC::GetSensorGpsServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(sensorGpsServiceInterfaceClientInsMapMtx_);
    if (sensorGpsServiceInterfaceClientInsMap_.find(portName) == sensorGpsServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(sensorGpsServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return sensorGpsServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<VehicleInfomationServiceInterfaceClient> XvehicleStatusRecorderBaseSwC::GetVehicleInfomationServiceInterfaceClient(const ara::core::String& portName)
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
std::shared_ptr<FaultDataServiceInterfaceClient> XvehicleStatusRecorderBaseSwC::GetFaultDataServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(faultDataServiceInterfaceClientInsMapMtx_);
    if (faultDataServiceInterfaceClientInsMap_.find(portName) == faultDataServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(faultDataServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return faultDataServiceInterfaceClientInsMap_[portName];
}
std::shared_ptr<MsgImageDataListServiceInterfaceClient> XvehicleStatusRecorderBaseSwC::GetMsgImageDataListServiceInterfaceClient(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(msgImageDataListServiceInterfaceClientInsMapMtx_);
    if (msgImageDataListServiceInterfaceClientInsMap_.find(portName) == msgImageDataListServiceInterfaceClientInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(msgImageDataListServiceInterfaceClientInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return msgImageDataListServiceInterfaceClientInsMap_[portName];
}

bool XvehicleStatusRecorderBaseSwC::LoadConfig()
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

bool XvehicleStatusRecorderBaseSwC::DoesFileExist(const ara::core::String& filePath)
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

bool XvehicleStatusRecorderBaseSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool XvehicleStatusRecorderBaseSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("xvehicle_status_recorder_base")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find xvehicle_status_recorder_base in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["xvehicle_status_recorder_base"];
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
    swcConfig["recvInstanceID"]["RemoteControlServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", remoteControlServiceInterfaceClientIdVec_);
    StableUnique(remoteControlServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["RemoteControlServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", remoteControlServiceInterfaceClientPortVec_);
    StableUnique(remoteControlServiceInterfaceClientPortVec_);
    if (remoteControlServiceInterfaceClientIdVec_.size() != remoteControlServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < remoteControlServiceInterfaceClientIdVec_.size(); ++i) {
        remoteControlServiceInterfaceClientIdMap_[remoteControlServiceInterfaceClientPortVec_[i]] = remoteControlServiceInterfaceClientIdVec_[i];
    }
    swcConfig["recvInstanceID"]["BodyCommandServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", bodyCommandServiceInterfaceClientIdVec_);
    StableUnique(bodyCommandServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["BodyCommandServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", bodyCommandServiceInterfaceClientPortVec_);
    StableUnique(bodyCommandServiceInterfaceClientPortVec_);
    if (bodyCommandServiceInterfaceClientIdVec_.size() != bodyCommandServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < bodyCommandServiceInterfaceClientIdVec_.size(); ++i) {
        bodyCommandServiceInterfaceClientIdMap_[bodyCommandServiceInterfaceClientPortVec_[i]] = bodyCommandServiceInterfaceClientIdVec_[i];
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
    swcConfig["recvInstanceID"]["SensorGpsServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", sensorGpsServiceInterfaceClientIdVec_);
    StableUnique(sensorGpsServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["SensorGpsServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", sensorGpsServiceInterfaceClientPortVec_);
    StableUnique(sensorGpsServiceInterfaceClientPortVec_);
    if (sensorGpsServiceInterfaceClientIdVec_.size() != sensorGpsServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < sensorGpsServiceInterfaceClientIdVec_.size(); ++i) {
        sensorGpsServiceInterfaceClientIdMap_[sensorGpsServiceInterfaceClientPortVec_[i]] = sensorGpsServiceInterfaceClientIdVec_[i];
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
    swcConfig["recvInstanceID"]["FaultDataServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", faultDataServiceInterfaceClientIdVec_);
    StableUnique(faultDataServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["FaultDataServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", faultDataServiceInterfaceClientPortVec_);
    StableUnique(faultDataServiceInterfaceClientPortVec_);
    if (faultDataServiceInterfaceClientIdVec_.size() != faultDataServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < faultDataServiceInterfaceClientIdVec_.size(); ++i) {
        faultDataServiceInterfaceClientIdMap_[faultDataServiceInterfaceClientPortVec_[i]] = faultDataServiceInterfaceClientIdVec_[i];
    }
    swcConfig["recvInstanceID"]["MsgImageDataListServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", msgImageDataListServiceInterfaceClientIdVec_);
    StableUnique(msgImageDataListServiceInterfaceClientIdVec_);
    swcConfig["recvInstanceID"]["MsgImageDataListServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", msgImageDataListServiceInterfaceClientPortVec_);
    StableUnique(msgImageDataListServiceInterfaceClientPortVec_);
    if (msgImageDataListServiceInterfaceClientIdVec_.size() != msgImageDataListServiceInterfaceClientPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < msgImageDataListServiceInterfaceClientIdVec_.size(); ++i) {
        msgImageDataListServiceInterfaceClientIdMap_[msgImageDataListServiceInterfaceClientPortVec_[i]] = msgImageDataListServiceInterfaceClientIdVec_[i];
    }
    return true;
}
}  /* namespace xvehicle_status_recorder_base */
}  /* namespace mdc */