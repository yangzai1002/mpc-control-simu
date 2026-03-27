/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_SELECTPOINT_IMPL_TYPE_POINT_H
#define ARA_SELECTPOINT_IMPL_TYPE_POINT_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"

namespace ara {
namespace selectpoint {
struct Point {
    ::Double x{ 0.0 };
    ::Double y{ 0.0 };
    ::Double z{ 0.0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(x);
        fun(y);
        fun(z);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(x);
        fun(y);
        fun(z);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("x", x);
        fun("y", y);
        fun("z", z);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("x", x);
        fun("y", y);
        fun("z", z);
    }

    friend bool operator==(const ::ara::selectpoint::Point& lhs, const ::ara::selectpoint::Point& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.z - rhs.z)) < DBL_EPSILON);
    }
};
} // namespace selectpoint
} // namespace ara


#endif // ARA_SELECTPOINT_IMPL_TYPE_POINT_H
