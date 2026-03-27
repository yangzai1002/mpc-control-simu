/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_TWIST_H
#define IMPL_TYPE_TWIST_H

#include "impl_type_vector3.h"

struct Twist {
    ::Vector3 linear;
    ::Vector3 angular;

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(linear);
        fun(angular);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(linear);
        fun(angular);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("linear", linear);
        fun("angular", angular);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("linear", linear);
        fun("angular", angular);
    }

    friend bool operator==(const ::Twist& lhs, const ::Twist& rhs) noexcept
    {
        return (lhs.linear == rhs.linear) && (lhs.angular == rhs.angular);
    }
};


#endif // IMPL_TYPE_TWIST_H
