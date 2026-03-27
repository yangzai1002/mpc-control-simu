/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_NAVIROADPOINT_H
#define IMPL_TYPE_NAVIROADPOINT_H

#include <cfloat>
#include <cmath>
#include "impl_type_string.h"
#include "impl_type_double.h"
#include "impl_type_point.h"

struct NaviRoadPoint {
    ::String id;
    ::Double s{ 0.0 };
    ::Point pose;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(s);
        fun(pose);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(s);
        fun(pose);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("s", s);
        fun("pose", pose);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("s", s);
        fun("pose", pose);
    }

    friend bool operator==(const ::NaviRoadPoint& lhs, const ::NaviRoadPoint& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (fabs(static_cast<double>(lhs.s - rhs.s)) < DBL_EPSILON) && (lhs.pose == rhs.pose);
    }
};


#endif // IMPL_TYPE_NAVIROADPOINT_H
