/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_BUSNIESSREMOTECONTROL_H
#define ARA_ADSFI_IMPL_TYPE_BUSNIESSREMOTECONTROL_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct BusniessRemoteControl {
    double steering_wheel{ 0.0 };
    ::int32_t gear{ 0 };
    double brake{ 0.0 };
    double accelerator{ 0.0 };
    ::int32_t remotectrl_flag{ 0 };
    ::int32_t estop_flag{ 0 };
    double steer_torque{ 0.0 };
    bool eng_status{ false };
    ::int32_t ctrl_level{ 0 };
    ::int32_t tick{ 0 };
    bool in_place{ false };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(steering_wheel);
        fun(gear);
        fun(brake);
        fun(accelerator);
        fun(remotectrl_flag);
        fun(estop_flag);
        fun(steer_torque);
        fun(eng_status);
        fun(ctrl_level);
        fun(tick);
        fun(in_place);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(steering_wheel);
        fun(gear);
        fun(brake);
        fun(accelerator);
        fun(remotectrl_flag);
        fun(estop_flag);
        fun(steer_torque);
        fun(eng_status);
        fun(ctrl_level);
        fun(tick);
        fun(in_place);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("steering_wheel", steering_wheel);
        fun("gear", gear);
        fun("brake", brake);
        fun("accelerator", accelerator);
        fun("remotectrl_flag", remotectrl_flag);
        fun("estop_flag", estop_flag);
        fun("steer_torque", steer_torque);
        fun("eng_status", eng_status);
        fun("ctrl_level", ctrl_level);
        fun("tick", tick);
        fun("in_place", in_place);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("steering_wheel", steering_wheel);
        fun("gear", gear);
        fun("brake", brake);
        fun("accelerator", accelerator);
        fun("remotectrl_flag", remotectrl_flag);
        fun("estop_flag", estop_flag);
        fun("steer_torque", steer_torque);
        fun("eng_status", eng_status);
        fun("ctrl_level", ctrl_level);
        fun("tick", tick);
        fun("in_place", in_place);
    }

    friend bool operator==(const ::ara::adsfi::BusniessRemoteControl& lhs, const ::ara::adsfi::BusniessRemoteControl& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.steering_wheel - rhs.steering_wheel)) < DBL_EPSILON) && (lhs.gear == rhs.gear) && (fabs(static_cast<double>(lhs.brake - rhs.brake)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.accelerator - rhs.accelerator)) < DBL_EPSILON) && (lhs.remotectrl_flag == rhs.remotectrl_flag) && (lhs.estop_flag == rhs.estop_flag) && (fabs(static_cast<double>(lhs.steer_torque - rhs.steer_torque)) < DBL_EPSILON) && (lhs.eng_status == rhs.eng_status) && (lhs.ctrl_level == rhs.ctrl_level) && (lhs.tick == rhs.tick) && (lhs.in_place == rhs.in_place);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_BUSNIESSREMOTECONTROL_H
