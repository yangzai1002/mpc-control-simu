/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_VIRTUALWALL_H
#define IMPL_TYPE_VIRTUALWALL_H

#include <cfloat>
#include <cmath>
#include "impl_type_float.h"
#include "impl_type_int32.h"

struct VirtualWall {
    ::Float target_speed{ 0.0F };
    ::Float target_distance{ 0.0F };
    ::Float desire_follow_seconds{ 0.0F };
    ::Int32 id{ 0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(target_speed);
        fun(target_distance);
        fun(desire_follow_seconds);
        fun(id);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(target_speed);
        fun(target_distance);
        fun(desire_follow_seconds);
        fun(id);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("target_speed", target_speed);
        fun("target_distance", target_distance);
        fun("desire_follow_seconds", desire_follow_seconds);
        fun("id", id);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("target_speed", target_speed);
        fun("target_distance", target_distance);
        fun("desire_follow_seconds", desire_follow_seconds);
        fun("id", id);
    }

    friend bool operator==(const ::VirtualWall& lhs, const ::VirtualWall& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.target_speed - rhs.target_speed)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.target_distance - rhs.target_distance)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.desire_follow_seconds - rhs.desire_follow_seconds)) < DBL_EPSILON) && (lhs.id == rhs.id);
    }
};


#endif // IMPL_TYPE_VIRTUALWALL_H
