#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>

#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "bag_recorder.h"
#include "nlohmann/json.hpp"
#include "custom_stack.h"

#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_msgcameravpresult.h"
#include "ara/adsfi/impl_type_msghaf3ddetectionoutarray.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_msghafgnssinfo.h"
#include "ara/adsfi/impl_type_msghafimu.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_msghafwheelspeedlist.h"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include "ara/adsfi/impl_type_msglidarframe.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_sensorlidarpacket.h"
#include "ara/adsfi/impl_type_sensorradarpacket.h"
#include "ara/adsfi/impl_type_sensorradarpointcloud.h"
#include "ara/adsfi/impl_type_taskexecutestatus.h"
#include "ara/adsfi/impl_type_taskmanagestatus.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_vehiclesensorcontrol.h"
#include "ara/adsfi/impl_type_videoencodingperf.h"
#include "ara/adsfi/impl_type_videortpparameter.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() : stop_monitor_(false), record_index_(0) {
    }
    ~AdsfiInterface() {
        stop_monitor_ = true;
    }

    void Init() {
        auto ptr= CustomStack::Instance();

        std::cout << "load config:"<<ptr->GetNodeConfigPath()+"/config/message_collection.json"<<std::endl;
        LoadConfig(ptr->GetNodeConfigPath()+"/config/message_collection.json");
        // std::thread(&AdsfiInterface::MonitorLoop, this).detach();
    }

    bool TriggerRecord(const std::string &bagPath) {
        return recorder_.Trigger(bagPath, record_pre_ms_, record_post_ms_);
    }

    rtfbag_util::RecordStatus GetRecordStatus() const   { return recorder_.GetStatus(); }
    std::string               GetRecordStatusStr() const { return recorder_.GetStatusStr(); }
    size_t                    GetLastWrittenFrames() const { return recorder_.GetLastWrittenFrames(); }
    size_t                    GetBufferUsedBytes() const   { return recorder_.GetBufferUsedBytes(); }
    size_t                    GetBufferSize() const        { return recorder_.GetBufferSize(); }

    // -----------------------------------------------------------------------
    // Callbacks
    // allow_publish 命中 → AVOS_MSGPACK_PUBLISH
    // trigger_record_topics 命中 → recorder_.Push
    // -----------------------------------------------------------------------

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafIMU> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgHafIMU", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &msg){
        // std::cout << "name:"<<std::endl;
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgHafLocation", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            // std::cout << "push"<<std::endl;
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppRemoteControl> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"AppRemoteControl", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::FaultData> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"FaultData", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"BusinessCommand", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"VehicleInformation", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VideoRTPParameter> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"VideoRTPParameter", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHaf3dDetectionOutArray> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgHaf3dDetectionOutArray", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppVideoProfile> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"AppVideoProfile", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleActControl> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"VehicleActControl", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgHafFusionOutArray", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgHafEgoTrajectory", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"PerceptionStaticEnv", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafGnssInfo> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgHafGnssInfo", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageData> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgImageData", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::TaskManageStatus> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"TaskManageStatus", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleSensorControl> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"VehicleSensorControl", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PlanningStatus> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"PlanningStatus", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VideoEncodingPerf> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"VideoEncodingPerf", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::TaskExecuteStatus> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"TaskExecuteStatus", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgCameraVPResult> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgCameraVPResult", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgHafWheelSpeedList", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageDataList> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgImageDataList", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgLidarFrame> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"MsgLidarFrame", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::SensorLidarPacket> &msg){
        // std::cout << "name:"<<name<<std::endl;
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"SensorLidarPacket", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            // std::cout << "push"<<std::endl;
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::SensorRadarPacket> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"SensorRadarPacket", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::SensorRadarPointCloud> &msg){
        if (allow_names_.count(name)) {
            CUsrAp2GsdsPubInterface::Instance()->publish( name ,"SensorRadarPointCloud", static_cast<void *>(msg.get()));
        }
        if (record_topics_.count(name)) {
            recorder_.Push(name, *msg);
        }
    }

public:
    BASE_TEMPLATE_FUNCION

private:
    std::unordered_set<std::string>                    allow_names_;
    std::unordered_set<std::string>                    record_topics_;
    rtfbag_util::BagRecorder                           recorder_;
    std::atomic<bool>                                  stop_monitor_;
    std::atomic<int>                                   record_index_;
    uint64_t                                           record_pre_ms_  = 5000;
    uint64_t                                           record_post_ms_ = 3000;

    void MonitorLoop() {
        int elapsed_sec = 0;
        while (!stop_monitor_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (stop_monitor_) break;


            std::cout << "[Monitor] status=" << GetRecordStatusStr()
                      << " frames=" << GetLastWrittenFrames()
                      << " buf=" << GetBufferUsedBytes() << "/" << GetBufferSize()
                      << std::endl;

            ApInfo("test") << "[Monitor] status=" << GetRecordStatusStr()
                      << " frames=" << GetLastWrittenFrames()
                      << " buf=" << GetBufferUsedBytes() << "/" << GetBufferSize();

            ++elapsed_sec;
            if (elapsed_sec >= 60) {
                elapsed_sec = 0;
                int idx = record_index_++;
                auto now = std::chrono::system_clock::now();
                std::time_t t = std::chrono::system_clock::to_time_t(now);
                char ts[32];
                std::strftime(ts, sizeof(ts), "%Y%m%d_%H%M%S", std::localtime(&t));
                std::string bag_name = "auto_record_" + std::to_string(idx) + "_" + ts + ".bag";
                TriggerRecord(bag_name);
                std::cout << "[Monitor] triggered record: " << bag_name << std::endl;
            }
        }
    }

    void LoadConfig(const std::string &config_path) {
        std::ifstream file(config_path);
        if (!file.is_open()) {
            std::cout << "AdsfiInterface: failed to open config: " << config_path << std::endl;
            return;
        }
        try {
            nlohmann::json j;
            file >> j;

            for (const auto &name : j.at("allow_publish")) {
                allow_names_.insert(name.get<std::string>());
            }

            for (const auto &name : j.at("trigger_record_topics")) {
                record_topics_.insert(name.get<std::string>());
            }

            std::string bag_index  = j.value("bag_index_file", "bag_topic_index.json");
            size_t      max_mem_mb = j.value("bag_max_memory_mb", 512);
            record_pre_ms_         = j.value("record_pre_ms",  5000);
            record_post_ms_        = j.value("record_post_ms", 3000);
            auto ptr= CustomStack::Instance();

            std::cout << "init recorder:"<<ptr->GetNodeConfigPath()+"/config/"+bag_index<<std::endl;
            recorder_.Init(ptr->GetNodeConfigPath()+"/config/"+bag_index, max_mem_mb * 1024 * 1024);

        } catch (const std::exception &e) {
            std::cout << "AdsfiInterface: config parse error: " << e.what() << std::endl;
        }
    }
};
#endif
