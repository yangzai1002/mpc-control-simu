/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VEHICLEINFORMATION_H
#define ARA_ADSFI_IMPL_TYPE_VEHICLEINFORMATION_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "ara/adsfi/impl_type_vehiclesensorstate.h"
#include "ara/adsfi/impl_type_vehicleactstate.h"
#include "ara/adsfi/impl_type_msghafwheelspeedlist.h"

namespace ara {
namespace adsfi {
struct VehicleInformation {
    ::ara::adsfi::CommonHeader header;
    ::ara::adsfi::VehicleSensorState vehicle_sensor_state;
    ::ara::adsfi::VehicleActState vehicle_act_state;
    ::ara::adsfi::MsgHafWheelSpeedList wheel_speed;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(vehicle_sensor_state);
        fun(vehicle_act_state);
        fun(wheel_speed);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(vehicle_sensor_state);
        fun(vehicle_act_state);
        fun(wheel_speed);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("vehicle_sensor_state", vehicle_sensor_state);
        fun("vehicle_act_state", vehicle_act_state);
        fun("wheel_speed", wheel_speed);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("vehicle_sensor_state", vehicle_sensor_state);
        fun("vehicle_act_state", vehicle_act_state);
        fun("wheel_speed", wheel_speed);
    }

    friend bool operator==(const ::ara::adsfi::VehicleInformation& lhs, const ::ara::adsfi::VehicleInformation& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.vehicle_sensor_state == rhs.vehicle_sensor_state) && (lhs.vehicle_act_state == rhs.vehicle_act_state) && (lhs.wheel_speed == rhs.wheel_speed);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VEHICLEINFORMATION_H
