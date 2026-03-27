/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_TIMEWINDOW_H
#define IMPL_TYPE_TIMEWINDOW_H

#include "ara/common/impl_type_commontime.h"

struct TimeWindow {
    ::ara::common::CommonTime stamp;
    ::ara::common::CommonTime age;

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(stamp);
        fun(age);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(stamp);
        fun(age);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("stamp", stamp);
        fun("age", age);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("stamp", stamp);
        fun("age", age);
    }

    friend bool operator==(const ::TimeWindow& lhs, const ::TimeWindow& rhs) noexcept
    {
        return (lhs.stamp == rhs.stamp) && (lhs.age == rhs.age);
    }
};


#endif // IMPL_TYPE_TIMEWINDOW_H
