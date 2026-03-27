/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VEHICLEACTSTATE_H
#define ARA_ADSFI_IMPL_TYPE_VEHICLEACTSTATE_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct VehicleActState {
    float throttle_info{ 0.0F };
    float steer_angle{ 0.0F };
    float steer_curvature{ 0.0F };
    float speed{ 0.0F };
    float acceleration{ 0.0F };
    ::uint8_t shift_position{ 0U };
    ::uint8_t aeb_active{ 0U };
    ::uint8_t parking_mode_status{ 0U };
    ::uint8_t epb_status{ 0U };
    ::uint8_t drive_mode{ 0U };
    ::uint8_t steering_mode_status{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(throttle_info);
        fun(steer_angle);
        fun(steer_curvature);
        fun(speed);
        fun(acceleration);
        fun(shift_position);
        fun(aeb_active);
        fun(parking_mode_status);
        fun(epb_status);
        fun(drive_mode);
        fun(steering_mode_status);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(throttle_info);
        fun(steer_angle);
        fun(steer_curvature);
        fun(speed);
        fun(acceleration);
        fun(shift_position);
        fun(aeb_active);
        fun(parking_mode_status);
        fun(epb_status);
        fun(drive_mode);
        fun(steering_mode_status);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("throttle_info", throttle_info);
        fun("steer_angle", steer_angle);
        fun("steer_curvature", steer_curvature);
        fun("speed", speed);
        fun("acceleration", acceleration);
        fun("shift_position", shift_position);
        fun("aeb_active", aeb_active);
        fun("parking_mode_status", parking_mode_status);
        fun("epb_status", epb_status);
        fun("drive_mode", drive_mode);
        fun("steering_mode_status", steering_mode_status);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("throttle_info", throttle_info);
        fun("steer_angle", steer_angle);
        fun("steer_curvature", steer_curvature);
        fun("speed", speed);
        fun("acceleration", acceleration);
        fun("shift_position", shift_position);
        fun("aeb_active", aeb_active);
        fun("parking_mode_status", parking_mode_status);
        fun("epb_status", epb_status);
        fun("drive_mode", drive_mode);
        fun("steering_mode_status", steering_mode_status);
    }

    friend bool operator==(const ::ara::adsfi::VehicleActState& lhs, const ::ara::adsfi::VehicleActState& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.throttle_info - rhs.throttle_info)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.steer_angle - rhs.steer_angle)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.steer_curvature - rhs.steer_curvature)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.speed - rhs.speed)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.acceleration - rhs.acceleration)) < DBL_EPSILON) && (lhs.shift_position == rhs.shift_position) && (lhs.aeb_active == rhs.aeb_active) && (lhs.parking_mode_status == rhs.parking_mode_status) && (lhs.epb_status == rhs.epb_status) && (lhs.drive_mode == rhs.drive_mode) && (lhs.steering_mode_status == rhs.steering_mode_status);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VEHICLEACTSTATE_H
