/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/xfault_handler_base/xfault_handler_base_swc.h"

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
#include "mdc/xfault_handler_base_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/xfault_handler_base/faultdataserviceinterface_server.h"
#include "ara/adsfi/faultdataserviceinterface_skeleton.h"
#include "mdc/xfault_handler_base/remotecontrolserviceinterface_client.h"

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
namespace xfault_handler_base {
using ara::core::String;
using LOG_SPACE = mdc::xfault_handler_base::XfaultHandlerBaseLogger;

XfaultHandlerBaseSwC::XfaultHandlerBaseSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

XfaultHandlerBaseSwC::~XfaultHandlerBaseSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool XfaultHandlerBaseSwC::Init()
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
    for (auto id : faultDataServiceInterfaceServerIdMap_) {
        auto resultToken = ara::adsfi::skeleton::FaultDataServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            faultDataServiceInterfaceServerInsMap_[id.first] = std::make_shared<FaultDataServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (faultDataServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "FaultDataServiceInterfaceSkeleton",
            id.second);
    }
    /* client agent */
    for (auto id : remoteControlServiceInterfaceClientIdMap_) {
        remoteControlServiceInterfaceClientInsMap_[id.first] = std::make_shared<RemoteControlServiceInterfaceClient>(id.second);
        RETURN_IF_INIT_SERVICE_FAILED(
            (remoteControlServiceInterfaceClientInsMap_[id.first]!= nullptr),
            "RemoteControlServiceInterfaceProxy",
            id.second);
    }

    if ((initHandler_) && (!initHandler_())) {
        return false;
    }

    initFlag_ = true;
    return true;
}

void XfaultHandlerBaseSwC::Stop()
{
    workFlag_ = false;
    /* server agent */
    for (auto id : faultDataServiceInterfaceServerIdMap_) {
        if (faultDataServiceInterfaceServerInsMap_[id.first]) {
            faultDataServiceInterfaceServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    /* client agent */
    for (auto id : remoteControlServiceInterfaceClientIdMap_) {
        if (remoteControlServiceInterfaceClientInsMap_[id.first]) {
            remoteControlServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    if (stopHandler_) {
        stopHandler_();
    }
    return;
}


std::shared_ptr<FaultDataServiceInterfaceServer> XfaultHandlerBaseSwC::GetFaultDataServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(faultDataServiceInterfaceServerInsMapMtx_);
    if (faultDataServiceInterfaceServerInsMap_.find(portName) == faultDataServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(faultDataServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return faultDataServiceInterfaceServerInsMap_[portName];
}
std::shared_ptr<RemoteControlServiceInterfaceClient> XfaultHandlerBaseSwC::GetRemoteControlServiceInterfaceClient(const ara::core::String& portName)
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

bool XfaultHandlerBaseSwC::LoadConfig()
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

bool XfaultHandlerBaseSwC::DoesFileExist(const ara::core::String& filePath)
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

bool XfaultHandlerBaseSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool XfaultHandlerBaseSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("xfault_handler_base")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find xfault_handler_base in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["xfault_handler_base"];
    /* server agent */
    if (!swcConfig.HasKeyValue("sendInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find sendInstanceID in config file!";
        return false;
    }
    swcConfig["sendInstanceID"]["FaultDataServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", faultDataServiceInterfaceServerIdVec_);
    StableUnique(faultDataServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["FaultDataServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", faultDataServiceInterfaceServerPortVec_);
    StableUnique(faultDataServiceInterfaceServerPortVec_);
    if (faultDataServiceInterfaceServerIdVec_.size() != faultDataServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < faultDataServiceInterfaceServerIdVec_.size(); ++i) {
        faultDataServiceInterfaceServerIdMap_[faultDataServiceInterfaceServerPortVec_[i]] = faultDataServiceInterfaceServerIdVec_[i];
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
    return true;
}
}  /* namespace xfault_handler_base */
}  /* namespace mdc */