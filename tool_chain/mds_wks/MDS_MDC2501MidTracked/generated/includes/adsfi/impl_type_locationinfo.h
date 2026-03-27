/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_LOCATIONINFO_H
#define ADSFI_IMPL_TYPE_LOCATIONINFO_H

#include "ara/common/impl_type_commonheader.h"
#include "impl_type_uint16.h"
#include "impl_type_posewithcovariance.h"
#include "impl_type_twistwithcovariance.h"
#include "impl_type_accelwithcovariance.h"

namespace adsfi {
struct LocationInfo {
    ::ara::common::CommonHeader header;
    ::UInt16 locationState{ 0U };
    ::PoseWithCovariance pose;
    ::TwistWithCovariance velocity;
    ::AccelWithCovariance acceleration;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(locationState);
        fun(pose);
        fun(velocity);
        fun(acceleration);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(locationState);
        fun(pose);
        fun(velocity);
        fun(acceleration);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("locationState", locationState);
        fun("pose", pose);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("locationState", locationState);
        fun("pose", pose);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
    }

    friend bool operator==(const ::adsfi::LocationInfo& lhs, const ::adsfi::LocationInfo& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.locationState == rhs.locationState) && (lhs.pose == rhs.pose) && (lhs.velocity == rhs.velocity) && (lhs.acceleration == rhs.acceleration);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_LOCATIONINFO_H
