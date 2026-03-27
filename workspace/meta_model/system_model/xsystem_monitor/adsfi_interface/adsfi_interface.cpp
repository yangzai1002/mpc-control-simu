#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"
#include "ap_log/ap_log_interface.h"

void AdsfiInterface::Init() {
    SetScheduleInfo("timmer", 100);
    // ApLogInterface::Instance()->Init("XSystemMonitor");
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::FaultData> &msg) {
    if (!_system_monitor) {
        try {
            _system_monitor = std::make_shared<SystemMonitor>();
            ApInfo("system_monitor") << "create system monitor ok";
        } catch (const std::exception &e) {
            ApError("system_monitor") << "failed to create system monitor: " << e.what();
        }
    }

    return 0;
}
