/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_QUATERNION_H
#define IMPL_TYPE_QUATERNION_H

#include <cfloat>
#include <cmath>
#include "impl_type_float.h"

struct Quaternion {
    ::Float x{ 0.0F };
    ::Float y{ 0.0F };
    ::Float z{ 0.0F };
    ::Float w{ 0.0F };

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
        fun(w);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(x);
        fun(y);
        fun(z);
        fun(w);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("x", x);
        fun("y", y);
        fun("z", z);
        fun("w", w);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("x", x);
        fun("y", y);
        fun("z", z);
        fun("w", w);
    }

    friend bool operator==(const ::Quaternion& lhs, const ::Quaternion& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.z - rhs.z)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.w - rhs.w)) < DBL_EPSILON);
    }
};


#endif // IMPL_TYPE_QUATERNION_H
