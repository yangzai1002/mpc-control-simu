#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"

void AdsfiInterface::Init() {
    try {
        SetScheduleInfo("timmer", 100);
        // ApLogInterface::Instance()->Init("XZmqTask");

        _handler = std::make_shared<ZmqApiHandler3>();
        if (!_handler) {
            ApError("zmq_task") << "AdsfiInterface::Init() - Failed to create ZmqApiHandler3";
            throw std::runtime_error("Failed to create ZmqApiHandler3");
        }

        ApInfo("zmq_task") << "AdsfiInterface::Init() - Initialization completed successfully";
    } catch (const std::exception &e) {
        ApError("zmq_task") << "AdsfiInterface::Init() exception: " << e.what();
        _handler.reset();
        throw;
    } catch (...) {
        ApError("zmq_task") << "AdsfiInterface::Init() unknown exception";
        _handler.reset();
        throw std::runtime_error("AdsfiInterface initialization failed with unknown exception");
    }
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &msg) {
    if (__glibc_unlikely(msg == nullptr)) {
        ApError("zmq_task") << "msg is null";
        return;
    }

    std::lock_guard<std::mutex> lg(_data_mtx);
    _vehicle_information_cache = *msg;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &msg) {
    if (__glibc_unlikely(msg == nullptr)) {
        ApError("zmq_task") << "msg is null";
        return;
    }
    std::lock_guard<std::mutex> lg(_data_mtx);
    _gps_cache = *msg;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &msg) {
    if (__glibc_unlikely(msg == nullptr)) {
        ApError("zmq_task") << "msg is null";
        return;
    }

    if (name == "tpperceptionobjects") {
        std::lock_guard<std::mutex> lg(_data_mtx);
        _detected_objects_cache = *msg;
    } else if (name == "tptarget_obj") {
        std::lock_guard<std::mutex> lg(_data_mtx);
        _target_obj_cache = *msg;
    } else {
        ApError("zmq_task") << "unknown name: " << name;
    }
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::TaskExecuteStatus> &msg) {
    if (__glibc_unlikely(msg == nullptr)) {
        ApError("zmq_task") << "msg is null";
        return;
    }
    std::lock_guard<std::mutex> lg(_data_mtx);
    _task_exe_status_cache = *msg;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::BusinessCommand> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("zmq_task") << "Process(BusinessCommand): msg is null";
        return -1;
    }
    if (__glibc_unlikely(nullptr == _handler)) {
        ApError("zmq_task") << "Process(BusinessCommand): _handler is nullptr";
        return -1;
    }

    try {
        std::lock_guard<std::mutex> lg(_data_mtx);
        _handler->on_data_change(
            _gps_cache, _task_exe_status_cache, _vehicle_information_cache, _detected_objects_cache, _target_obj_cache
            /*,bag_in */);

        _handler->values(*msg);
        msg->header.time = app_common::DatetimeUtil::current_sec_nsec<decltype(msg->header.time)>();
    } catch (const std::exception &e) {
        ApError("zmq_task") << "Process(BusinessCommand) exception: " << e.what();
        return -1;
    } catch (...) {
        ApError("zmq_task") << "Process(BusinessCommand) unknown exception";
        return -1;
    }

    return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleSensorControl> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("zmq_task") << "Process(VehicleSensorControl) msg is null";
        return -1;
    }
    if (__glibc_unlikely(nullptr == _handler)) {
        ApError("zmq_task") << "Process(VehicleSensorControl) _handler is nullptr";
        return -1;
    }

    _handler->values(*msg);
    msg->header.time = app_common::DatetimeUtil::current_sec_nsec<decltype(msg->header.time)>();

    return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::AppVideoProfile> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("zmq_task") << "Process(AppVideoProfile) msg is null";
        return -1;
    }
    if (__glibc_unlikely(nullptr == _handler)) {
        ApError("zmq_task") << "Process(AppVideoProfile) _handler is nullptr";
        return -1;
    }

    _handler->values(*msg);
    msg->header.time = app_common::DatetimeUtil::current_sec_nsec<decltype(msg->header.time)>();

    return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::VideoRTPParameter> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("zmq_task") << "Process(VideoRTPParameter) msg is null";
        return -1;
    }
    if (__glibc_unlikely(nullptr == _handler)) {
        ApError("zmq_task") << "Process(VideoRTPParameter) _handler is nullptr";
        return -1;
    }

    _handler->values(*msg);
    msg->header.time = app_common::DatetimeUtil::current_sec_nsec<decltype(msg->header.time)>();

    return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::TaskManageStatus> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("zmq_task") << "Process(TaskManageStatus) msg is null";
        return -1;
    }
    if (__glibc_unlikely(nullptr == _handler)) {
        ApError("zmq_task") << "Process(TaskManageStatus) _handler is nullptr";
        return -1;
    }

    _handler->values(*msg);
    msg->header.time = app_common::DatetimeUtil::current_sec_nsec<decltype(msg->header.time)>();

    return 0;
}