#include "DatetimeUtil.hpp"
#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"
#include "ap_log/ap_log_interface.h"

void AdsfiInterface::Init() {
    SetScheduleInfo("timmer", 5);
    // ApLogInterface::Instance()->Init("XZmqRc");
    handler_ptr = std::make_shared<ZmqRC3>();
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::AppRemoteControl> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("zmq_rc") << "msg is null";
        return -1;
    }

    handler_ptr->on_data_change();
    handler_ptr->values(*msg);
    msg->header.time = app_common::DatetimeUtil::current_sec_nsec<decltype(msg->header.time)>();

    return 0;
}
