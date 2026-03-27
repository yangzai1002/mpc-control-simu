/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_COMMON_IMPL_TYPE_IDPAPPREMOTECONTROLINAP_H
#define ARA_COMMON_IMPL_TYPE_IDPAPPREMOTECONTROLINAP_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"

namespace ara {
namespace common {
struct IdpAppRemoteControlInAP {
    ::Double steering_wheel_ap{ 0.0 };
    ::Double gear_ap{ 0.0 };
    ::Double brake_ap{ 0.0 };
    ::Double accelerator_ap{ 0.0 };
    ::Double remotectrl_flag_ap{ 0.0 };
    ::Double estop_flag_ap{ 0.0 };
    ::Double steer_torque_ap{ 0.0 };
    ::Double eng_status_ap{ 0.0 };
    ::Double ctrl_level_ap{ 0.0 };
    ::Double in_place_ap{ 0.0 };
    ::Double tick_ap{ 0.0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(steering_wheel_ap);
        fun(gear_ap);
        fun(brake_ap);
        fun(accelerator_ap);
        fun(remotectrl_flag_ap);
        fun(estop_flag_ap);
        fun(steer_torque_ap);
        fun(eng_status_ap);
        fun(ctrl_level_ap);
        fun(in_place_ap);
        fun(tick_ap);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(steering_wheel_ap);
        fun(gear_ap);
        fun(brake_ap);
        fun(accelerator_ap);
        fun(remotectrl_flag_ap);
        fun(estop_flag_ap);
        fun(steer_torque_ap);
        fun(eng_status_ap);
        fun(ctrl_level_ap);
        fun(in_place_ap);
        fun(tick_ap);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("steering_wheel_ap", steering_wheel_ap);
        fun("gear_ap", gear_ap);
        fun("brake_ap", brake_ap);
        fun("accelerator_ap", accelerator_ap);
        fun("remotectrl_flag_ap", remotectrl_flag_ap);
        fun("estop_flag_ap", estop_flag_ap);
        fun("steer_torque_ap", steer_torque_ap);
        fun("eng_status_ap", eng_status_ap);
        fun("ctrl_level_ap", ctrl_level_ap);
        fun("in_place_ap", in_place_ap);
        fun("tick_ap", tick_ap);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("steering_wheel_ap", steering_wheel_ap);
        fun("gear_ap", gear_ap);
        fun("brake_ap", brake_ap);
        fun("accelerator_ap", accelerator_ap);
        fun("remotectrl_flag_ap", remotectrl_flag_ap);
        fun("estop_flag_ap", estop_flag_ap);
        fun("steer_torque_ap", steer_torque_ap);
        fun("eng_status_ap", eng_status_ap);
        fun("ctrl_level_ap", ctrl_level_ap);
        fun("in_place_ap", in_place_ap);
        fun("tick_ap", tick_ap);
    }

    friend bool operator==(const ::ara::common::IdpAppRemoteControlInAP& lhs, const ::ara::common::IdpAppRemoteControlInAP& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.steering_wheel_ap - rhs.steering_wheel_ap)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.gear_ap - rhs.gear_ap)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.brake_ap - rhs.brake_ap)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.accelerator_ap - rhs.accelerator_ap)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.remotectrl_flag_ap - rhs.remotectrl_flag_ap)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.estop_flag_ap - rhs.estop_flag_ap)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.steer_torque_ap - rhs.steer_torque_ap)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.eng_status_ap - rhs.eng_status_ap)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.ctrl_level_ap - rhs.ctrl_level_ap)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.in_place_ap - rhs.in_place_ap)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.tick_ap - rhs.tick_ap)) < DBL_EPSILON);
    }
};
} // namespace common
} // namespace ara


#endif // ARA_COMMON_IMPL_TYPE_IDPAPPREMOTECONTROLINAP_H
