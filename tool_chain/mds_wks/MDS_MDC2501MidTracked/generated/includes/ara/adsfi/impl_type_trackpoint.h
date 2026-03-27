/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_TRACKPOINT_H
#define ARA_ADSFI_IMPL_TYPE_TRACKPOINT_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32_t.h"
#include "impl_type_int8_t.h"

namespace ara {
namespace adsfi {
struct TrackPoint {
    double lon{ 0.0 };
    double lat{ 0.0 };
    double heading{ 0.0 };
    ::int32_t index{ 0 };
    double length{ 0.0 };
    ::int8_t direction{ 0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(lon);
        fun(lat);
        fun(heading);
        fun(index);
        fun(length);
        fun(direction);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(lon);
        fun(lat);
        fun(heading);
        fun(index);
        fun(length);
        fun(direction);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("lon", lon);
        fun("lat", lat);
        fun("heading", heading);
        fun("index", index);
        fun("length", length);
        fun("direction", direction);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("lon", lon);
        fun("lat", lat);
        fun("heading", heading);
        fun("index", index);
        fun("length", length);
        fun("direction", direction);
    }

    friend bool operator==(const ::ara::adsfi::TrackPoint& lhs, const ::ara::adsfi::TrackPoint& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.lon - rhs.lon)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.lat - rhs.lat)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.heading - rhs.heading)) < DBL_EPSILON) && (lhs.index == rhs.index) && (fabs(static_cast<double>(lhs.length - rhs.length)) < DBL_EPSILON) && (lhs.direction == rhs.direction);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_TRACKPOINT_H
