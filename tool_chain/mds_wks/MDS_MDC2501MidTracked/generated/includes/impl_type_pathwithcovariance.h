/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_PATHWITHCOVARIANCE_H
#define IMPL_TYPE_PATHWITHCOVARIANCE_H

#include "ara/common/impl_type_commonheader.h"
#include "impl_type_posewithcovariancestamped.h"
#include "impl_type_timewindow.h"

struct PathWithCovariance {
    ::ara::common::CommonHeader header;
    ::PoseWithCovarianceStamped poses;
    ::TimeWindow time_window;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(poses);
        fun(time_window);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(poses);
        fun(time_window);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("poses", poses);
        fun("time_window", time_window);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("poses", poses);
        fun("time_window", time_window);
    }

    friend bool operator==(const ::PathWithCovariance& lhs, const ::PathWithCovariance& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.poses == rhs.poses) && (lhs.time_window == rhs.time_window);
    }
};


#endif // IMPL_TYPE_PATHWITHCOVARIANCE_H
