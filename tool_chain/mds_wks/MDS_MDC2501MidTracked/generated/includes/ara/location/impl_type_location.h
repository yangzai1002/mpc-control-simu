/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_LOCATION_IMPL_TYPE_LOCATION_H
#define ARA_LOCATION_IMPL_TYPE_LOCATION_H

#include "ara/location/impl_type_header.h"
#include "impl_type_uint16.h"
#include "impl_type_posewithcovariance.h"
#include "impl_type_twistwithcovariance.h"
#include "impl_type_accelwithcovariance.h"
#include "impl_type_uint8_t.h"

namespace ara {
namespace location {
struct Location {
    ::ara::location::Header header;
    ::UInt16 locationState{ 0U };
    ::PoseWithCovariance pose;
    ::TwistWithCovariance velocity;
    ::AccelWithCovariance acceleration;
    ::uint8_t odomType{ 0U };

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
        fun(odomType);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(locationState);
        fun(pose);
        fun(velocity);
        fun(acceleration);
        fun(odomType);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("locationState", locationState);
        fun("pose", pose);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("odomType", odomType);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("locationState", locationState);
        fun("pose", pose);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("odomType", odomType);
    }

    friend bool operator==(const ::ara::location::Location& lhs, const ::ara::location::Location& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.locationState == rhs.locationState) && (lhs.pose == rhs.pose) && (lhs.velocity == rhs.velocity) && (lhs.acceleration == rhs.acceleration) && (lhs.odomType == rhs.odomType);
    }
};
} // namespace location
} // namespace ara


#endif // ARA_LOCATION_IMPL_TYPE_LOCATION_H
