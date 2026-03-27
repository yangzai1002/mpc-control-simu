#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"

AdsfiInterface::AdsfiInterface() {
}

AdsfiInterface::~AdsfiInterface() {
    // 录包器会在 unique_ptr 销毁时自动停止（RAII）
}

void AdsfiInterface::Init() {
    // 设置调度信息（10Hz = 100ms）
    SetScheduleInfo("timmer", 100);

    // 初始化日志
    // ApLogInterface::Instance()->Init("XDataTriggerRecorder");
}

void AdsfiInterface::init_recorder() {
    try {
        // 创建录包器实例（在构造时自动启动）
        recorder_ = std::make_unique<event_recorder::EventTriggeredRecorder>();

        ApInfo("trigger_record") << "Event triggered recorder initialized and started";
    } catch (const std::exception& e) {
        ApError("trigger_record") << "Failed to initialize recorder: " << e.what();
        throw;
    }
}

void AdsfiInterface::Callback(const std::string& name, std::shared_ptr<ara::adsfi::BusinessCommand>& msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("trigger_record") << "msg is null";
        return;
    }

    std::lock_guard<std::mutex> lock(recorder_mutex_);
    if (__glibc_unlikely(nullptr == recorder_)) {
        ApError("trigger_record") << "Recorder is not initialized";
        return;
    }
    recorder_->update_manual_trigger(msg->record_trigger_flag);
}

void AdsfiInterface::Callback(const std::string& name, std::shared_ptr<ara::adsfi::FaultData>& msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("trigger_record") << "msg is null";
        return;
    }

    std::lock_guard<std::mutex> lock(recorder_mutex_);
    if (__glibc_unlikely(nullptr == recorder_)) {
        ApError("trigger_record") << "Recorder is not initialized";
        return;
    }

    // 更新录包器的故障数据
    recorder_->update_fault_data(*msg);
}

int AdsfiInterface::Process(const std::string& name, std::shared_ptr<ara::adsfi::FaultData>& msg) {
    std::lock_guard<std::mutex> lock(recorder_mutex_);
    if (__glibc_unlikely(nullptr == recorder_)) {
        try {
            init_recorder();
        } catch (const std::exception& e) {
            ApError("trigger_record") << "Failed to re-initialize recorder: " << e.what();
        }
    }

    return -1;
}
