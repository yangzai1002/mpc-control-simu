/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_TWISTWITHCOVARIANCE_H
#define IMPL_TYPE_TWISTWITHCOVARIANCE_H

#include "impl_type_twist.h"
#include "impl_type_doublearray36.h"

struct TwistWithCovariance {
    ::Twist twist;
    ::DoubleArray36 covariance{};

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(twist);
        fun(covariance);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(twist);
        fun(covariance);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("twist", twist);
        fun("covariance", covariance);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("twist", twist);
        fun("covariance", covariance);
    }

    friend bool operator==(const ::TwistWithCovariance& lhs, const ::TwistWithCovariance& rhs) noexcept
    {
        return (lhs.twist == rhs.twist) && (lhs.covariance == rhs.covariance);
    }
};


#endif // IMPL_TYPE_TWISTWITHCOVARIANCE_H
