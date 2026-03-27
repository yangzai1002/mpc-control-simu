/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VEHICLEACTCONTROL_H
#define ARA_ADSFI_IMPL_TYPE_VEHICLEACTCONTROL_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "ara/adsfi/impl_type_vehiclelatcontrol.h"
#include "ara/adsfi/impl_type_vehicleloncontrol.h"
#include "ara/adsfi/impl_type_vehiclesensorcontrol.h"

namespace ara {
namespace adsfi {
struct VehicleActControl {
    ::ara::adsfi::CommonHeader header;
    ::ara::adsfi::VehicleLatControl lat_control;
    ::ara::adsfi::VehicleLonControl lon_control;
    ::ara::adsfi::VehicleSensorControl sensor_control;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(lat_control);
        fun(lon_control);
        fun(sensor_control);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(lat_control);
        fun(lon_control);
        fun(sensor_control);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("lat_control", lat_control);
        fun("lon_control", lon_control);
        fun("sensor_control", sensor_control);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("lat_control", lat_control);
        fun("lon_control", lon_control);
        fun("sensor_control", sensor_control);
    }

    friend bool operator==(const ::ara::adsfi::VehicleActControl& lhs, const ::ara::adsfi::VehicleActControl& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.lat_control == rhs.lat_control) && (lhs.lon_control == rhs.lon_control) && (lhs.sensor_control == rhs.sensor_control);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VEHICLEACTCONTROL_H
