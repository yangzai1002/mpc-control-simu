/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_TASKEXECUTESTATUS_H
#define ARA_ADSFI_IMPL_TYPE_TASKEXECUTESTATUS_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint8_t.h"
#include "ara/adsfi/impl_type_businesstaskpoints.h"

namespace ara {
namespace adsfi {
struct TaskExecuteStatus {
    ::uint8_t type{ 0U };
    ::uint8_t status{ 0U };
    double current_limit_speed{ 0.0 };
    ::ara::adsfi::BusinessTaskPoints task_points;
    double distance_to_nearest{ 0.0 };
    double total_mileage{ 0.0 };
    double current_mileage{ 0.0 };
    ::uint8_t progress{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(type);
        fun(status);
        fun(current_limit_speed);
        fun(task_points);
        fun(distance_to_nearest);
        fun(total_mileage);
        fun(current_mileage);
        fun(progress);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(type);
        fun(status);
        fun(current_limit_speed);
        fun(task_points);
        fun(distance_to_nearest);
        fun(total_mileage);
        fun(current_mileage);
        fun(progress);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("type", type);
        fun("status", status);
        fun("current_limit_speed", current_limit_speed);
        fun("task_points", task_points);
        fun("distance_to_nearest", distance_to_nearest);
        fun("total_mileage", total_mileage);
        fun("current_mileage", current_mileage);
        fun("progress", progress);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("type", type);
        fun("status", status);
        fun("current_limit_speed", current_limit_speed);
        fun("task_points", task_points);
        fun("distance_to_nearest", distance_to_nearest);
        fun("total_mileage", total_mileage);
        fun("current_mileage", current_mileage);
        fun("progress", progress);
    }

    friend bool operator==(const ::ara::adsfi::TaskExecuteStatus& lhs, const ::ara::adsfi::TaskExecuteStatus& rhs) noexcept
    {
        return (lhs.type == rhs.type) && (lhs.status == rhs.status) && (fabs(static_cast<double>(lhs.current_limit_speed - rhs.current_limit_speed)) < DBL_EPSILON) && (lhs.task_points == rhs.task_points) && (fabs(static_cast<double>(lhs.distance_to_nearest - rhs.distance_to_nearest)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.total_mileage - rhs.total_mileage)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.current_mileage - rhs.current_mileage)) < DBL_EPSILON) && (lhs.progress == rhs.progress);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_TASKEXECUTESTATUS_H
