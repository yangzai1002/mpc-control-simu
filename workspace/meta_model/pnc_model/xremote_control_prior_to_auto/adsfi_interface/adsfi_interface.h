
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <atomic>
#include <memory>
#include <string>

#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() {
        is_remote_control.store(true);
        // is_act_control.store(true);
    }

    ~AdsfiInterface() {
    }

    void Init() override {
        SetScheduleInfo("messge");
        std::cout << "xremote_control_prior_to_auto init" << std::endl;
    }

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppRemoteControl> &msg) {
        if (msg->remotectrl_flag != 0) {
            is_remote_control.store(true);
        } else {
            is_remote_control.store(false);
        }
    }

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &msg) {
        if (msg->business_mode != 0) {
            is_act_control.store(true);
        } else {
            is_act_control.store(false);
        }
    }

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleActControl> &msg) {
        std::shared_ptr<ara::adsfi::VehicleActControl> remote_control_ptr = nullptr;
        std::shared_ptr<ara::adsfi::VehicleActControl> track_control_ptr = nullptr;
        if (name == "tpvehicle_to_can_rc") { // todo
            remote_control_ptr = msg;
        }

        if (name == "tpvehicle_to_can_auto") { // todo
            track_control_ptr = msg;
        }

        if (is_remote_control.load() == true) {
            if (remote_control_ptr != nullptr) {
                act_deque.push(remote_control_ptr);
                ApInfo("rc_prior_auto") << "use remote control";
            } else {
                ApError("rc_prior_auto") << "remote control is not exist";
            }
            return;
        }

        if (is_act_control.load() == true) {
            if (track_control_ptr != nullptr) {
                act_deque.push(track_control_ptr);
                ApInfo("rc_prior_auto") << "use track act control";
            } else {
                ApError("rc_prior_auto") << "track act control is not exist";
            }
            return;
        }

        if (remote_control_ptr != nullptr) {
            if (remote_control_ptr != nullptr) {
                act_deque.push(remote_control_ptr);
                ApInfo("rc_prior_auto") << "use remote control";
            } else {
                ApError("rc_prior_auto") << "remote control is not exist";
            }
            return;
        }
    }

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleActControl> &msg) {
        std::shared_ptr<ara::adsfi::VehicleActControl> temp_ptr;
        act_deque.waitAndPop(temp_ptr);
        *msg = *temp_ptr;
        return 0;
    }

private:
    ThreadSafeQueue<std::shared_ptr<ara::adsfi::VehicleActControl>> act_deque;

    std::atomic<bool> is_remote_control;

    std::atomic<bool> is_act_control;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
