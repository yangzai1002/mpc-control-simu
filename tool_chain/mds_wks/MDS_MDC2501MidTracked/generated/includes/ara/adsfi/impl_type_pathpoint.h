/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PATHPOINT_H
#define ARA_ADSFI_IMPL_TYPE_PATHPOINT_H

#include <cfloat>
#include <cmath>
#include "impl_type_point.h"
#include "impl_type_double.h"

namespace ara {
namespace adsfi {
struct PathPoint {
    ::Point point;
    ::Double s{ 0.0 };
    ::Double theta{ 0.0 };
    ::Double kappa{ 0.0 };
    ::Double dkappa{ 0.0 };
    ::Double heading{ 0.0 };
    ::Double ddkappa{ 0.0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(point);
        fun(s);
        fun(theta);
        fun(kappa);
        fun(dkappa);
        fun(heading);
        fun(ddkappa);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(point);
        fun(s);
        fun(theta);
        fun(kappa);
        fun(dkappa);
        fun(heading);
        fun(ddkappa);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("point", point);
        fun("s", s);
        fun("theta", theta);
        fun("kappa", kappa);
        fun("dkappa", dkappa);
        fun("heading", heading);
        fun("ddkappa", ddkappa);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("point", point);
        fun("s", s);
        fun("theta", theta);
        fun("kappa", kappa);
        fun("dkappa", dkappa);
        fun("heading", heading);
        fun("ddkappa", ddkappa);
    }

    friend bool operator==(const ::ara::adsfi::PathPoint& lhs, const ::ara::adsfi::PathPoint& rhs) noexcept
    {
        return (lhs.point == rhs.point) && (fabs(static_cast<double>(lhs.s - rhs.s)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.theta - rhs.theta)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.kappa - rhs.kappa)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.dkappa - rhs.dkappa)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.heading - rhs.heading)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.ddkappa - rhs.ddkappa)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PATHPOINT_H
