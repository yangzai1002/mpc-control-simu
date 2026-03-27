/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VEHICLELATCONTROL_H
#define ARA_ADSFI_IMPL_TYPE_VEHICLELATCONTROL_H

#include <cfloat>
#include <cmath>
#include "impl_type_int8_t.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct VehicleLatControl {
    float target_angle{ 0.0F };
    float target_torque{ 0.0F };
    ::int8_t centor_control_value{ 0 };
    float target_curvature{ 0.0F };
    ::int32_t steer_control_mode{ 0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(target_angle);
        fun(target_torque);
        fun(centor_control_value);
        fun(target_curvature);
        fun(steer_control_mode);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(target_angle);
        fun(target_torque);
        fun(centor_control_value);
        fun(target_curvature);
        fun(steer_control_mode);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("target_angle", target_angle);
        fun("target_torque", target_torque);
        fun("centor_control_value", centor_control_value);
        fun("target_curvature", target_curvature);
        fun("steer_control_mode", steer_control_mode);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("target_angle", target_angle);
        fun("target_torque", target_torque);
        fun("centor_control_value", centor_control_value);
        fun("target_curvature", target_curvature);
        fun("steer_control_mode", steer_control_mode);
    }

    friend bool operator==(const ::ara::adsfi::VehicleLatControl& lhs, const ::ara::adsfi::VehicleLatControl& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.target_angle - rhs.target_angle)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.target_torque - rhs.target_torque)) < DBL_EPSILON) && (lhs.centor_control_value == rhs.centor_control_value) && (fabs(static_cast<double>(lhs.target_curvature - rhs.target_curvature)) < DBL_EPSILON) && (lhs.steer_control_mode == rhs.steer_control_mode);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VEHICLELATCONTROL_H
