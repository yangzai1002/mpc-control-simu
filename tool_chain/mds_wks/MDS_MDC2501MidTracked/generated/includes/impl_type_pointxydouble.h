/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_POINTXYDOUBLE_H
#define IMPL_TYPE_POINTXYDOUBLE_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"

struct PointxyDouble {
    ::Double x{ 0.0 };
    ::Double y{ 0.0 };

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
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(x);
        fun(y);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("x", x);
        fun("y", y);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("x", x);
        fun("y", y);
    }

    friend bool operator==(const ::PointxyDouble& lhs, const ::PointxyDouble& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON);
    }
};


#endif // IMPL_TYPE_POINTXYDOUBLE_H
