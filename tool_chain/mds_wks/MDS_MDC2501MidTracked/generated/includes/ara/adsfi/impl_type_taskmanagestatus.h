/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_TASKMANAGESTATUS_H
#define ARA_ADSFI_IMPL_TYPE_TASKMANAGESTATUS_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct TaskManageStatus {
    ::ara::adsfi::CommonHeader header;
    ::uint8_t action_id{ 0U };
    ::uint8_t task_status{ 0U };
    ::uint8_t task_type{ 0U };
    ::uint8_t task_progress{ 0U };
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
        fun(header);
        fun(action_id);
        fun(task_status);
        fun(task_type);
        fun(task_progress);
        fun(distance_to_nearest);
        fun(total_mileage);
        fun(current_mileage);
        fun(progress);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(action_id);
        fun(task_status);
        fun(task_type);
        fun(task_progress);
        fun(distance_to_nearest);
        fun(total_mileage);
        fun(current_mileage);
        fun(progress);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("action_id", action_id);
        fun("task_status", task_status);
        fun("task_type", task_type);
        fun("task_progress", task_progress);
        fun("distance_to_nearest", distance_to_nearest);
        fun("total_mileage", total_mileage);
        fun("current_mileage", current_mileage);
        fun("progress", progress);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("action_id", action_id);
        fun("task_status", task_status);
        fun("task_type", task_type);
        fun("task_progress", task_progress);
        fun("distance_to_nearest", distance_to_nearest);
        fun("total_mileage", total_mileage);
        fun("current_mileage", current_mileage);
        fun("progress", progress);
    }

    friend bool operator==(const ::ara::adsfi::TaskManageStatus& lhs, const ::ara::adsfi::TaskManageStatus& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.action_id == rhs.action_id) && (lhs.task_status == rhs.task_status) && (lhs.task_type == rhs.task_type) && (lhs.task_progress == rhs.task_progress) && (fabs(static_cast<double>(lhs.distance_to_nearest - rhs.distance_to_nearest)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.total_mileage - rhs.total_mileage)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.current_mileage - rhs.current_mileage)) < DBL_EPSILON) && (lhs.progress == rhs.progress);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_TASKMANAGESTATUS_H
