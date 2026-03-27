/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_TRAJPOINT_H
#define ADSFI_IMPL_TYPE_TRAJPOINT_H

#include <cfloat>
#include <cmath>
#include "adsfi/impl_type_pathpoint.h"
#include "impl_type_double.h"

namespace adsfi {
struct TrajPoint {
    ::adsfi::PathPoint pathPoint;
    ::Double speed{ 0.0 };
    ::Double velocity{ 0.0 };
    ::Double acceleration{ 0.0 };
    ::Double relativeTime{ 0.0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(pathPoint);
        fun(speed);
        fun(velocity);
        fun(acceleration);
        fun(relativeTime);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(pathPoint);
        fun(speed);
        fun(velocity);
        fun(acceleration);
        fun(relativeTime);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("pathPoint", pathPoint);
        fun("speed", speed);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("relativeTime", relativeTime);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("pathPoint", pathPoint);
        fun("speed", speed);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("relativeTime", relativeTime);
    }

    friend bool operator==(const ::adsfi::TrajPoint& lhs, const ::adsfi::TrajPoint& rhs) noexcept
    {
        return (lhs.pathPoint == rhs.pathPoint) && (fabs(static_cast<double>(lhs.speed - rhs.speed)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.velocity - rhs.velocity)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.acceleration - rhs.acceleration)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.relativeTime - rhs.relativeTime)) < DBL_EPSILON);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_TRAJPOINT_H
