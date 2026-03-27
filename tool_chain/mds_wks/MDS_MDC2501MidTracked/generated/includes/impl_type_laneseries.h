/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_LANESERIES_H
#define IMPL_TYPE_LANESERIES_H

#include "impl_type_lanepiecevector.h"
#include "impl_type_boolean.h"
#include "impl_type_uint32.h"

struct LaneSeries {
    ::LanePieceVector laneSeries;
    ::Boolean isDrivable{ false };
    ::UInt32 directionType{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(laneSeries);
        fun(isDrivable);
        fun(directionType);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(laneSeries);
        fun(isDrivable);
        fun(directionType);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("laneSeries", laneSeries);
        fun("isDrivable", isDrivable);
        fun("directionType", directionType);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("laneSeries", laneSeries);
        fun("isDrivable", isDrivable);
        fun("directionType", directionType);
    }

    friend bool operator==(const ::LaneSeries& lhs, const ::LaneSeries& rhs) noexcept
    {
        return (lhs.laneSeries == rhs.laneSeries) && (lhs.isDrivable == rhs.isDrivable) && (lhs.directionType == rhs.directionType);
    }
};


#endif // IMPL_TYPE_LANESERIES_H
