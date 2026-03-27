/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_POLYGON_H
#define IMPL_TYPE_POLYGON_H

#include "impl_type_pointarray.h"

struct Polygon {
    ::PointArray points;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(points);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(points);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("points", points);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("points", points);
    }

    friend bool operator==(const ::Polygon& lhs, const ::Polygon& rhs) noexcept
    {
        return (lhs.points == rhs.points);
    }
};


#endif // IMPL_TYPE_POLYGON_H
