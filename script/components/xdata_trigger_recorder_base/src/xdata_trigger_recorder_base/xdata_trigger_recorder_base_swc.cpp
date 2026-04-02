/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: class swc
 */

#include "mdc/xdata_trigger_recorder_base/xdata_trigger_recorder_base_swc.h"

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
#include "mdc/xdata_trigger_recorder_base_logger.h"
#include "mdc/utility/mdc_yaml_node.h"
#include "mdc/xdata_trigger_recorder_base/bagstatusserviceinterface_server.h"
#include "ara/adsfi/bagstatusserviceinterface_skeleton.h"
#include "mdc/xdata_trigger_recorder_base/faultdataserviceinterface_client.h"

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
namespace xdata_trigger_recorder_base {
using ara::core::String;
using LOG_SPACE = mdc::xdata_trigger_recorder_base::XdataTriggerRecorderBaseLogger;

XdataTriggerRecorderBaseSwC::XdataTriggerRecorderBaseSwC()
    : initFlag_(false),
      workFlag_(true),
      initHandler_(nullptr),
      stopHandler_(nullptr),
      identifier_("SWC"),
      loggerId_("SWC")
{}

XdataTriggerRecorderBaseSwC::~XdataTriggerRecorderBaseSwC()
{
    if (workFlag_) {
        Stop();
    }
}

bool XdataTriggerRecorderBaseSwC::Init()
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
    for (auto id : bagStatusServiceInterfaceServerIdMap_) {
        auto resultToken = ara::adsfi::skeleton::BagStatusServiceInterfaceSkeleton::Preconstruct(
            ara::com::InstanceIdentifier(ara::core::StringView(std::to_string(id.second).c_str())),
            ara::com::MethodCallProcessingMode::kPoll);
        if (resultToken.HasValue()) {
            bagStatusServiceInterfaceServerInsMap_[id.first] = std::make_shared<BagStatusServiceInterfaceServer>(id.second);
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Skeleton preconstruct Failed!";
            return false;
        }
        RETURN_IF_INIT_SERVICE_FAILED(
            (bagStatusServiceInterfaceServerInsMap_[id.first]!= nullptr),
            "BagStatusServiceInterfaceSkeleton",
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

    if ((initHandler_) && (!initHandler_())) {
        return false;
    }

    initFlag_ = true;
    return true;
}

void XdataTriggerRecorderBaseSwC::Stop()
{
    workFlag_ = false;
    /* server agent */
    for (auto id : bagStatusServiceInterfaceServerIdMap_) {
        if (bagStatusServiceInterfaceServerInsMap_[id.first]) {
            bagStatusServiceInterfaceServerInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    /* client agent */
    for (auto id : faultDataServiceInterfaceClientIdMap_) {
        if (faultDataServiceInterfaceClientInsMap_[id.first]) {
            faultDataServiceInterfaceClientInsMap_[id.first]->Stop();
        } else {
            LOG_SPACE::GetLoggerIns("SWC")->LogError()<< "Cannot stop because " << id.first << " construction failed";
        }
    }

    if (stopHandler_) {
        stopHandler_();
    }
    return;
}


std::shared_ptr<BagStatusServiceInterfaceServer> XdataTriggerRecorderBaseSwC::GetBagStatusServiceInterfaceServer(const ara::core::String& portName)
{
    std::lock_guard<std::mutex> lck(bagStatusServiceInterfaceServerInsMapMtx_);
    if (bagStatusServiceInterfaceServerInsMap_.find(portName) == bagStatusServiceInterfaceServerInsMap_.end()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << "does not exist.";
        return nullptr;
    }
    if (!(bagStatusServiceInterfaceServerInsMap_[portName]->Init())) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << portName << " init failed.";
        return nullptr;
    }
    return bagStatusServiceInterfaceServerInsMap_[portName];
}
std::shared_ptr<FaultDataServiceInterfaceClient> XdataTriggerRecorderBaseSwC::GetFaultDataServiceInterfaceClient(const ara::core::String& portName)
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

bool XdataTriggerRecorderBaseSwC::LoadConfig()
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

bool XdataTriggerRecorderBaseSwC::DoesFileExist(const ara::core::String& filePath)
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

bool XdataTriggerRecorderBaseSwC::IsDirType(const mode_t& fileMode)
{
    if (S_ISDIR(fileMode)) {
        return true;
    }
    return false;
}

bool XdataTriggerRecorderBaseSwC::ParsingInstanceId(const std::unique_ptr<MdcYamlNode>& config)
{
    if (!config->HasKeyValue("xdata_trigger_recorder_base")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find xdata_trigger_recorder_base in config file!";
        return false;
    }
    auto swcConfig = (*(config.get()))["xdata_trigger_recorder_base"];
    /* server agent */
    if (!swcConfig.HasKeyValue("sendInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find sendInstanceID in config file!";
        return false;
    }
    swcConfig["sendInstanceID"]["BagStatusServiceInterface"].GetValue<std::vector<uint32_t>>(
        "instanceId", bagStatusServiceInterfaceServerIdVec_);
    StableUnique(bagStatusServiceInterfaceServerIdVec_);
    swcConfig["sendInstanceID"]["BagStatusServiceInterface"].GetValue<std::vector<std::string>>(
        "portName", bagStatusServiceInterfaceServerPortVec_);
    StableUnique(bagStatusServiceInterfaceServerPortVec_);
    if (bagStatusServiceInterfaceServerIdVec_.size() != bagStatusServiceInterfaceServerPortVec_.size()) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "The port and instance mapping information is incorrect.";
        return false;
    }
    for (size_t i = 0; i < bagStatusServiceInterfaceServerIdVec_.size(); ++i) {
        bagStatusServiceInterfaceServerIdMap_[bagStatusServiceInterfaceServerPortVec_[i]] = bagStatusServiceInterfaceServerIdVec_[i];
    }

    /* client agent */
    if (!swcConfig.HasKeyValue("recvInstanceID")) {
        LOG_SPACE::GetLoggerIns("SWC")->LogError() << "Cannot find recvInstanceID in config file!";
        return false;
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
    return true;
}
}  /* namespace xdata_trigger_recorder_base */
}  /* namespace mdc */