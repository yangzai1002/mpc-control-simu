/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_ACCELWITHCOVARIANCE_H
#define IMPL_TYPE_ACCELWITHCOVARIANCE_H

#include "impl_type_accel.h"
#include "impl_type_doublearray36.h"

struct AccelWithCovariance {
    ::Accel accel;
    ::DoubleArray36 covariance{};

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(accel);
        fun(covariance);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(accel);
        fun(covariance);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("accel", accel);
        fun("covariance", covariance);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("accel", accel);
        fun("covariance", covariance);
    }

    friend bool operator==(const ::AccelWithCovariance& lhs, const ::AccelWithCovariance& rhs) noexcept
    {
        return (lhs.accel == rhs.accel) && (lhs.covariance == rhs.covariance);
    }
};


#endif // IMPL_TYPE_ACCELWITHCOVARIANCE_H
