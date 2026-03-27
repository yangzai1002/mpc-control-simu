/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_POSEWITHCOVARIANCESTAMPED_H
#define IMPL_TYPE_POSEWITHCOVARIANCESTAMPED_H

#include "ara/common/impl_type_commonheader.h"
#include "impl_type_posewithcovariance.h"

struct PoseWithCovarianceStamped {
    ::ara::common::CommonHeader header;
    ::PoseWithCovariance pose;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(pose);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(pose);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("pose", pose);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("pose", pose);
    }

    friend bool operator==(const ::PoseWithCovarianceStamped& lhs, const ::PoseWithCovarianceStamped& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.pose == rhs.pose);
    }
};


#endif // IMPL_TYPE_POSEWITHCOVARIANCESTAMPED_H
