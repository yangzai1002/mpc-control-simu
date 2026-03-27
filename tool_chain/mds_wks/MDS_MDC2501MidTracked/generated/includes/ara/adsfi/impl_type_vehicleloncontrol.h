/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VEHICLELONCONTROL_H
#define ARA_ADSFI_IMPL_TYPE_VEHICLELONCONTROL_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct VehicleLonControl {
    float target_speed{ 0.0F };
    float target_torque{ 0.0F };
    float target_pressure{ 0.0F };
    ::uint8_t actuator_mode{ 0U };
    ::uint8_t emergency_mode{ 0U };
    ::uint8_t handbrake{ 0U };
    ::uint8_t shift_position{ 0U };
    ::uint8_t control_mode{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(target_speed);
        fun(target_torque);
        fun(target_pressure);
        fun(actuator_mode);
        fun(emergency_mode);
        fun(handbrake);
        fun(shift_position);
        fun(control_mode);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(target_speed);
        fun(target_torque);
        fun(target_pressure);
        fun(actuator_mode);
        fun(emergency_mode);
        fun(handbrake);
        fun(shift_position);
        fun(control_mode);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("target_speed", target_speed);
        fun("target_torque", target_torque);
        fun("target_pressure", target_pressure);
        fun("actuator_mode", actuator_mode);
        fun("emergency_mode", emergency_mode);
        fun("handbrake", handbrake);
        fun("shift_position", shift_position);
        fun("control_mode", control_mode);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("target_speed", target_speed);
        fun("target_torque", target_torque);
        fun("target_pressure", target_pressure);
        fun("actuator_mode", actuator_mode);
        fun("emergency_mode", emergency_mode);
        fun("handbrake", handbrake);
        fun("shift_position", shift_position);
        fun("control_mode", control_mode);
    }

    friend bool operator==(const ::ara::adsfi::VehicleLonControl& lhs, const ::ara::adsfi::VehicleLonControl& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.target_speed - rhs.target_speed)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.target_torque - rhs.target_torque)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.target_pressure - rhs.target_pressure)) < DBL_EPSILON) && (lhs.actuator_mode == rhs.actuator_mode) && (lhs.emergency_mode == rhs.emergency_mode) && (lhs.handbrake == rhs.handbrake) && (lhs.shift_position == rhs.shift_position) && (lhs.control_mode == rhs.control_mode);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VEHICLELONCONTROL_H
