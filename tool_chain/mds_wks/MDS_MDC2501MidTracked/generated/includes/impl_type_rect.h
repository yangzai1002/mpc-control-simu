/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_RECT_H
#define IMPL_TYPE_RECT_H

#include <cfloat>
#include <cmath>
#include "impl_type_float.h"

struct Rect {
    ::Float x{ 0.0F };
    ::Float y{ 0.0F };
    ::Float width{ 0.0F };
    ::Float height{ 0.0F };

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
        fun(width);
        fun(height);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(x);
        fun(y);
        fun(width);
        fun(height);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("x", x);
        fun("y", y);
        fun("width", width);
        fun("height", height);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("x", x);
        fun("y", y);
        fun("width", width);
        fun("height", height);
    }

    friend bool operator==(const ::Rect& lhs, const ::Rect& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.width - rhs.width)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.height - rhs.height)) < DBL_EPSILON);
    }
};


#endif // IMPL_TYPE_RECT_H
