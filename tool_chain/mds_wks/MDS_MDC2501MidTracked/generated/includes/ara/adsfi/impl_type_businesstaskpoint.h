/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_BUSINESSTASKPOINT_H
#define ARA_ADSFI_IMPL_TYPE_BUSINESSTASKPOINT_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_string.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct BusinessTaskPoint {
    ::ara::adsfi::CommonHeader header;
    ::String name;
    double high{ 0.0 };
    double pitch{ 0.0 };
    double yaw{ 0.0 };
    ::int32_t zoom{ 0 };
    double lat{ 0.0 };
    double lon{ 0.0 };
    double dir{ 0.0 };
    ::int32_t task_time{ 0 };
    ::int32_t type{ 0 };
    bool valid{ false };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(name);
        fun(high);
        fun(pitch);
        fun(yaw);
        fun(zoom);
        fun(lat);
        fun(lon);
        fun(dir);
        fun(task_time);
        fun(type);
        fun(valid);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(name);
        fun(high);
        fun(pitch);
        fun(yaw);
        fun(zoom);
        fun(lat);
        fun(lon);
        fun(dir);
        fun(task_time);
        fun(type);
        fun(valid);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("name", name);
        fun("high", high);
        fun("pitch", pitch);
        fun("yaw", yaw);
        fun("zoom", zoom);
        fun("lat", lat);
        fun("lon", lon);
        fun("dir", dir);
        fun("task_time", task_time);
        fun("type", type);
        fun("valid", valid);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("name", name);
        fun("high", high);
        fun("pitch", pitch);
        fun("yaw", yaw);
        fun("zoom", zoom);
        fun("lat", lat);
        fun("lon", lon);
        fun("dir", dir);
        fun("task_time", task_time);
        fun("type", type);
        fun("valid", valid);
    }

    friend bool operator==(const ::ara::adsfi::BusinessTaskPoint& lhs, const ::ara::adsfi::BusinessTaskPoint& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.name == rhs.name) && (fabs(static_cast<double>(lhs.high - rhs.high)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.pitch - rhs.pitch)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.yaw - rhs.yaw)) < DBL_EPSILON) && (lhs.zoom == rhs.zoom) && (fabs(static_cast<double>(lhs.lat - rhs.lat)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.lon - rhs.lon)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.dir - rhs.dir)) < DBL_EPSILON) && (lhs.task_time == rhs.task_time) && (lhs.type == rhs.type) && (lhs.valid == rhs.valid);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_BUSINESSTASKPOINT_H
