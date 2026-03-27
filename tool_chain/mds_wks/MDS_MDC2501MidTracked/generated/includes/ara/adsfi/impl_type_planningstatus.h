/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PLANNINGSTATUS_H
#define ARA_ADSFI_IMPL_TYPE_PLANNINGSTATUS_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_uint8_t.h"
#include "impl_type_int32_t.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"

namespace ara {
namespace adsfi {
struct PlanningStatus {
    ::ara::adsfi::CommonHeader header;
    ::uint8_t command_status{ 0U };
    double current_limit_speed{ 0.0 };
    double current_set_speed{ 0.0 };
    double current_set_angle{ 0.0 };
    ::int32_t decision_info{ 0 };
    ::ara::adsfi::MsgHafEgoTrajectory driving_center_trajectory;
    ::ara::adsfi::MsgHafEgoTrajectory driving_left_boundary;
    ::ara::adsfi::MsgHafEgoTrajectory driving_right_boundary;
    ::int32_t risk_level{ 0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(command_status);
        fun(current_limit_speed);
        fun(current_set_speed);
        fun(current_set_angle);
        fun(decision_info);
        fun(driving_center_trajectory);
        fun(driving_left_boundary);
        fun(driving_right_boundary);
        fun(risk_level);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(command_status);
        fun(current_limit_speed);
        fun(current_set_speed);
        fun(current_set_angle);
        fun(decision_info);
        fun(driving_center_trajectory);
        fun(driving_left_boundary);
        fun(driving_right_boundary);
        fun(risk_level);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("command_status", command_status);
        fun("current_limit_speed", current_limit_speed);
        fun("current_set_speed", current_set_speed);
        fun("current_set_angle", current_set_angle);
        fun("decision_info", decision_info);
        fun("driving_center_trajectory", driving_center_trajectory);
        fun("driving_left_boundary", driving_left_boundary);
        fun("driving_right_boundary", driving_right_boundary);
        fun("risk_level", risk_level);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("command_status", command_status);
        fun("current_limit_speed", current_limit_speed);
        fun("current_set_speed", current_set_speed);
        fun("current_set_angle", current_set_angle);
        fun("decision_info", decision_info);
        fun("driving_center_trajectory", driving_center_trajectory);
        fun("driving_left_boundary", driving_left_boundary);
        fun("driving_right_boundary", driving_right_boundary);
        fun("risk_level", risk_level);
    }

    friend bool operator==(const ::ara::adsfi::PlanningStatus& lhs, const ::ara::adsfi::PlanningStatus& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.command_status == rhs.command_status) && (fabs(static_cast<double>(lhs.current_limit_speed - rhs.current_limit_speed)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.current_set_speed - rhs.current_set_speed)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.current_set_angle - rhs.current_set_angle)) < DBL_EPSILON) && (lhs.decision_info == rhs.decision_info) && (lhs.driving_center_trajectory == rhs.driving_center_trajectory) && (lhs.driving_left_boundary == rhs.driving_left_boundary) && (lhs.driving_right_boundary == rhs.driving_right_boundary) && (lhs.risk_level == rhs.risk_level);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PLANNINGSTATUS_H
