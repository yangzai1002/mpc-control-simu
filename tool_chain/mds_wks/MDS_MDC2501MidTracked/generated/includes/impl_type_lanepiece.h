/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_LANEPIECE_H
#define IMPL_TYPE_LANEPIECE_H

#include <cfloat>
#include <cmath>
#include "impl_type_string.h"
#include "impl_type_double.h"

struct LanePiece {
    ::String id;
    ::Double startS{ 0.0 };
    ::Double endS{ 0.0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(startS);
        fun(endS);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(startS);
        fun(endS);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("startS", startS);
        fun("endS", endS);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("startS", startS);
        fun("endS", endS);
    }

    friend bool operator==(const ::LanePiece& lhs, const ::LanePiece& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (fabs(static_cast<double>(lhs.startS - rhs.startS)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.endS - rhs.endS)) < DBL_EPSILON);
    }
};


#endif // IMPL_TYPE_LANEPIECE_H
