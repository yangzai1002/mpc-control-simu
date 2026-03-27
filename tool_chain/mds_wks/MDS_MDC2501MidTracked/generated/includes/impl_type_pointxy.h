/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_POINTXY_H
#define IMPL_TYPE_POINTXY_H

#include "impl_type_int32.h"

struct Pointxy {
    ::Int32 x{ 0 };
    ::Int32 y{ 0 };

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

    friend bool operator==(const ::Pointxy& lhs, const ::Pointxy& rhs) noexcept
    {
        return (lhs.x == rhs.x) && (lhs.y == rhs.y);
    }
};


#endif // IMPL_TYPE_POINTXY_H
