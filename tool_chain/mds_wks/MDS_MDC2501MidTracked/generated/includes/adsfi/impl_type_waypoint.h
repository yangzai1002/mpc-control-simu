/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_WAYPOINT_H
#define ADSFI_IMPL_TYPE_WAYPOINT_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"
#include "ara/common/impl_type_commonheader.h"
#include "impl_type_string.h"

namespace adsfi {
struct WayPoint {
    ::Double x{ 0.0 };
    ::Double y{ 0.0 };
    ::Double z{ 0.0 };
    ::Double theta{ 0.0 };
    ::Double curvature{ 0.0 };
    ::Double s{ 0.0 };
    ::Double deltaCurvature{ 0.0 };
    ::Double deltaX{ 0.0 };
    ::Double deltaY{ 0.0 };
    ::ara::common::CommonHeader header;
    ::String laneId;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(x);
        fun(y);
        fun(z);
        fun(theta);
        fun(curvature);
        fun(s);
        fun(deltaCurvature);
        fun(deltaX);
        fun(deltaY);
        fun(header);
        fun(laneId);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(x);
        fun(y);
        fun(z);
        fun(theta);
        fun(curvature);
        fun(s);
        fun(deltaCurvature);
        fun(deltaX);
        fun(deltaY);
        fun(header);
        fun(laneId);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("x", x);
        fun("y", y);
        fun("z", z);
        fun("theta", theta);
        fun("curvature", curvature);
        fun("s", s);
        fun("deltaCurvature", deltaCurvature);
        fun("deltaX", deltaX);
        fun("deltaY", deltaY);
        fun("header", header);
        fun("laneId", laneId);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("x", x);
        fun("y", y);
        fun("z", z);
        fun("theta", theta);
        fun("curvature", curvature);
        fun("s", s);
        fun("deltaCurvature", deltaCurvature);
        fun("deltaX", deltaX);
        fun("deltaY", deltaY);
        fun("header", header);
        fun("laneId", laneId);
    }

    friend bool operator==(const ::adsfi::WayPoint& lhs, const ::adsfi::WayPoint& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.z - rhs.z)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.theta - rhs.theta)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.curvature - rhs.curvature)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.s - rhs.s)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.deltaCurvature - rhs.deltaCurvature)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.deltaX - rhs.deltaX)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.deltaY - rhs.deltaY)) < DBL_EPSILON) && (lhs.header == rhs.header) && (lhs.laneId == rhs.laneId);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_WAYPOINT_H
