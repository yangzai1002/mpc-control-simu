#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"

AdsfiInterface::AdsfiInterface() : running_(false) {
}

AdsfiInterface::~AdsfiInterface() {
}

void AdsfiInterface::Init() {
    try {
        ApError("zmq_notification") << "[Init] 初始化开始";
        SetScheduleInfo("timmer", 10);
        // ApLogInterface::Instance()->Init("XZmqNotification");
        running_ = true;
        std::thread(&AdsfiInterface::WorkerLoop, this).detach();
        ApError("zmq_notification") << "[Init] 初始化完成";
    } catch (const std::exception& e) {
        ApError("zmq_notification") << "[Init] 初始化异常:" << e.what();
        running_ = false;
        throw;
    } catch (...) {
        ApError("zmq_notification") << "[Init] 初始化未知异常";
        running_ = false;
        throw std::runtime_error("AdsfiInterface initialization failed with unknown exception");
    }
}

void AdsfiInterface::WorkerLoop() {
    ApError("zmq_notification") << "[WorkerLoop] 线程启动";

    auto next_time = std::chrono::steady_clock::now();

    while (running_) {
        next_time += std::chrono::milliseconds(10);

        try {
            ara::adsfi::MsgHafLocation       imu;
            ara::adsfi::MsgHafGnssInfo       location;
            ara::adsfi::FaultData            fault_data;
            ara::adsfi::VehicleInformation   vehicle_info;
            ara::adsfi::VehicleActControl    control;
            ara::adsfi::PerceptionStaticEnv  semantic_map;
            ara::adsfi::MsgHafFusionOutArray detected_objects;
            ara::adsfi::MsgHafFusionOutArray target_obj;
            ara::adsfi::BusinessCommand      business_cmd;
            ara::adsfi::TaskManageStatus     task_status;
            ara::adsfi::AppVideoProfile      video_profile;
            ara::adsfi::AppRemoteControl     remote_control;
            ara::adsfi::VideoEncodingPerf    encoding_perf;

            {
                std::lock_guard<std::mutex> lock(data_mutex_);
                GetCurrentDataSnapshotUnsafe(imu, location, fault_data, vehicle_info, control, semantic_map,
                                             detected_objects, target_obj, business_cmd, task_status, video_profile,
                                             remote_control, encoding_perf);
            }

            Xzmq_notification3(imu, location, fault_data, vehicle_info, control, semantic_map, detected_objects,
                               target_obj, business_cmd, task_status, video_profile, remote_control, encoding_perf);

        } catch (const std::exception& e) {
            ApError("zmq_notification") << "[WorkerLoop] 异常:" << e.what();
        } catch (...) {
            ApError("zmq_notification") << "[WorkerLoop] 未知异常";
        }

        std::this_thread::sleep_until(next_time);
    }
}

void AdsfiInterface::GetCurrentDataSnapshotUnsafe(
    ara::adsfi::MsgHafLocation& imu, ara::adsfi::MsgHafGnssInfo& location, ara::adsfi::FaultData& fault_data,
    ara::adsfi::VehicleInformation& vehicle_info, ara::adsfi::VehicleActControl& control,
    ara::adsfi::PerceptionStaticEnv& semantic_map, ara::adsfi::MsgHafFusionOutArray& detected_objects,
    ara::adsfi::MsgHafFusionOutArray& target_obj, ara::adsfi::BusinessCommand& business_cmd,
    ara::adsfi::TaskManageStatus& task_status, ara::adsfi::AppVideoProfile& video_profile,
    ara::adsfi::AppRemoteControl& remote_control, ara::adsfi::VideoEncodingPerf& encoding_perf) {
    imu = latest_imu_;
    location = latest_location_;
    fault_data = latest_fault_data_;
    vehicle_info = latest_vehicle_information_;
    control = latest_control_;
    semantic_map = latest_semantic_map_;
    detected_objects = latest_detected_objects_;
    target_obj = latest_target_obj_out_;
    business_cmd = latest_business_command_;
    task_status = latest_task_manage_status_;
    video_profile = latest_video_profile_;
    remote_control = latest_remote_control_;
    encoding_perf = latest_encoding_perf_;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::AppVideoProfile>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][AppVideoProfile] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_video_profile_ = *msg;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::TaskManageStatus>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][TaskManageStatus] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_task_manage_status_ = *msg;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::BusinessCommand>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][BusinessCommand] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_business_command_ = *msg;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][MsgHafFusionOutArray] msg is nullptr";
        return;
    }

    if ("tpperceptionobjects" == name) {
        std::lock_guard<std::mutex> lock(data_mutex_);
        latest_detected_objects_ = *msg;
    } else if ("tptarget_obj" == name) {
        std::lock_guard<std::mutex> lock(data_mutex_);
        latest_target_obj_out_ = *msg;
    } else {
        ApError("zmq_notification") << "[Callback][MsgHafFusionOutArray] 未知的name: " << name;
    }
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgHafLocation>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][MsgHafLocation] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_imu_ = *msg;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgHafGnssInfo>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][MsgHafGnssInfo] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_location_ = *msg;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][PerceptionStaticEnv] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_semantic_map_ = *msg;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::FaultData>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][FaultData] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_fault_data_ = *msg;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::VehicleInformation>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][VehicleInformation] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_vehicle_information_ = *msg;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::VideoEncodingPerf>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][VideoEncodingPerf] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_encoding_perf_ = *msg;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::VehicleActControl>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][VehicleActControl] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_control_ = *msg;
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::AppRemoteControl>& msg) {
    if (__glibc_unlikely(!msg)) {
        ApError("zmq_notification") << "[Callback][AppRemoteControl] msg is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_remote_control_ = *msg;
}