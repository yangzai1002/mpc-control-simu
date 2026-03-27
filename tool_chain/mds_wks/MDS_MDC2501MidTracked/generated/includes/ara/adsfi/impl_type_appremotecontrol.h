/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPREMOTECONTROL_H
#define ARA_ADSFI_IMPL_TYPE_APPREMOTECONTROL_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct AppRemoteControl {
    ::ara::adsfi::CommonHeader header;
    double steering_wheel{ 0.0 };
    ::int32_t gear{ 0 };
    double brake{ 0.0 };
    double accelerator{ 0.0 };
    ::int32_t remotectrl_flag{ 0 };
    ::int32_t estop_flag{ 0 };
    double steer_torque{ 0.0 };
    bool eng_status{ false };
    ::int32_t ctrl_level{ 0 };
    bool in_place{ false };
    ::int32_t tick{ 0 };
    ::int32_t speed_limit{ 0 };
    ::int32_t set_speed{ 0 };
    ::int32_t security_assist_enabled{ 0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(steering_wheel);
        fun(gear);
        fun(brake);
        fun(accelerator);
        fun(remotectrl_flag);
        fun(estop_flag);
        fun(steer_torque);
        fun(eng_status);
        fun(ctrl_level);
        fun(in_place);
        fun(tick);
        fun(speed_limit);
        fun(set_speed);
        fun(security_assist_enabled);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(steering_wheel);
        fun(gear);
        fun(brake);
        fun(accelerator);
        fun(remotectrl_flag);
        fun(estop_flag);
        fun(steer_torque);
        fun(eng_status);
        fun(ctrl_level);
        fun(in_place);
        fun(tick);
        fun(speed_limit);
        fun(set_speed);
        fun(security_assist_enabled);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("steering_wheel", steering_wheel);
        fun("gear", gear);
        fun("brake", brake);
        fun("accelerator", accelerator);
        fun("remotectrl_flag", remotectrl_flag);
        fun("estop_flag", estop_flag);
        fun("steer_torque", steer_torque);
        fun("eng_status", eng_status);
        fun("ctrl_level", ctrl_level);
        fun("in_place", in_place);
        fun("tick", tick);
        fun("speed_limit", speed_limit);
        fun("set_speed", set_speed);
        fun("security_assist_enabled", security_assist_enabled);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("steering_wheel", steering_wheel);
        fun("gear", gear);
        fun("brake", brake);
        fun("accelerator", accelerator);
        fun("remotectrl_flag", remotectrl_flag);
        fun("estop_flag", estop_flag);
        fun("steer_torque", steer_torque);
        fun("eng_status", eng_status);
        fun("ctrl_level", ctrl_level);
        fun("in_place", in_place);
        fun("tick", tick);
        fun("speed_limit", speed_limit);
        fun("set_speed", set_speed);
        fun("security_assist_enabled", security_assist_enabled);
    }

    friend bool operator==(const ::ara::adsfi::AppRemoteControl& lhs, const ::ara::adsfi::AppRemoteControl& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.steering_wheel - rhs.steering_wheel)) < DBL_EPSILON) && (lhs.gear == rhs.gear) && (fabs(static_cast<double>(lhs.brake - rhs.brake)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.accelerator - rhs.accelerator)) < DBL_EPSILON) && (lhs.remotectrl_flag == rhs.remotectrl_flag) && (lhs.estop_flag == rhs.estop_flag) && (fabs(static_cast<double>(lhs.steer_torque - rhs.steer_torque)) < DBL_EPSILON) && (lhs.eng_status == rhs.eng_status) && (lhs.ctrl_level == rhs.ctrl_level) && (lhs.in_place == rhs.in_place) && (lhs.tick == rhs.tick) && (lhs.speed_limit == rhs.speed_limit) && (lhs.set_speed == rhs.set_speed) && (lhs.security_assist_enabled == rhs.security_assist_enabled);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPREMOTECONTROL_H
