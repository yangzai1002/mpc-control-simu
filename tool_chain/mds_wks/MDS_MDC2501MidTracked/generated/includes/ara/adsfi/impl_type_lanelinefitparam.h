/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_LANELINEFITPARAM_H
#define ARA_ADSFI_IMPL_TYPE_LANELINEFITPARAM_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"

namespace ara {
namespace adsfi {
struct LaneLineFitParam {
    ::Double a{ 0.0 };
    ::Double b{ 0.0 };
    ::Double c{ 0.0 };
    ::Double d{ 0.0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(a);
        fun(b);
        fun(c);
        fun(d);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(a);
        fun(b);
        fun(c);
        fun(d);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("a", a);
        fun("b", b);
        fun("c", c);
        fun("d", d);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("a", a);
        fun("b", b);
        fun("c", c);
        fun("d", d);
    }

    friend bool operator==(const ::ara::adsfi::LaneLineFitParam& lhs, const ::ara::adsfi::LaneLineFitParam& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.a - rhs.a)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.b - rhs.b)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.c - rhs.c)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.d - rhs.d)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_LANELINEFITPARAM_H
