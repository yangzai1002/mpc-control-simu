/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_GAP_H
#define IMPL_TYPE_GAP_H

#include <cfloat>
#include <cmath>
#include "impl_type_float.h"
#include "impl_type_int32.h"

struct Gap {
    ::Float speed_front{ 0.0F };
    ::Float speed_back{ 0.0F };
    ::Float length{ 0.0F };
    ::Float distance{ 0.0F };
    ::Int32 lane_id{ 0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(speed_front);
        fun(speed_back);
        fun(length);
        fun(distance);
        fun(lane_id);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(speed_front);
        fun(speed_back);
        fun(length);
        fun(distance);
        fun(lane_id);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("speed_front", speed_front);
        fun("speed_back", speed_back);
        fun("length", length);
        fun("distance", distance);
        fun("lane_id", lane_id);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("speed_front", speed_front);
        fun("speed_back", speed_back);
        fun("length", length);
        fun("distance", distance);
        fun("lane_id", lane_id);
    }

    friend bool operator==(const ::Gap& lhs, const ::Gap& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.speed_front - rhs.speed_front)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.speed_back - rhs.speed_back)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.length - rhs.length)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.distance - rhs.distance)) < DBL_EPSILON) && (lhs.lane_id == rhs.lane_id);
    }
};


#endif // IMPL_TYPE_GAP_H
