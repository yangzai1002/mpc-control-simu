
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <memory>
#include <string>

#include "AionCanbusSend.h"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehiclesensorcontrol.h"
#include "log/log.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    void Init();

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleActControl> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::FaultData> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleSensorControl> &msg);

private:
    std::int32_t                        _expiry_threshold_control_ms;
    std::int32_t                        _expiry_threshold_fault_ms;
    std::shared_ptr<AionCanbusSendData> _driver_ptr;

    ara::adsfi::MsgHafTime _last_fault_data_time;
    ara::adsfi::MsgHafTime _last_control_data_time;
    std::atomic_bool       _estop;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
