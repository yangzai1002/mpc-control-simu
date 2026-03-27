#include "XCollectSystemFaults.hpp"
#include "XDetermineBehaviorPolicyFromFault.hpp"
#include "XSaveFaultInfoToFile.hpp"
#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"

void AdsfiInterface::Init() {
    SetScheduleInfo("timmer", 100);
    // ApLogInterface::Instance()->Init("XFaultHandler");
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &msg) {
    if (msg) {
        std::lock_guard lg(_mtx);
        _in_business_command = *msg;
    }
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppRemoteControl> &msg) {
    if (msg) {
        std::lock_guard lg(_mtx);
        _in_remote_control = *msg;
    }
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::FaultData> &msg) {
    _tmp_fault_data = {};
    XCollectSystemFaults(_tmp_fault_data);

    _out_fault_data = {};

    {
        std::lock_guard lg(_mtx);
        XDetermineBehaviorPolicyFromFault(_in_business_command, _in_remote_control, _tmp_fault_data, _out_fault_data);
    }

    XSaveFaultInfoToFile(_out_fault_data);
    if (msg) {
        *msg = _out_fault_data;
    }

    return 0;
}
