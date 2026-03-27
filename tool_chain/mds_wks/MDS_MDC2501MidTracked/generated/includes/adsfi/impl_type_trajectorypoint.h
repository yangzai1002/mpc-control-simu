/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_TRAJECTORYPOINT_H
#define ADSFI_IMPL_TYPE_TRAJECTORYPOINT_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"
#include "adsfi/impl_type_waypoint.h"
#include "ara/common/impl_type_commonheader.h"

namespace adsfi {
struct TrajectoryPoint {
    ::Double speed{ 0.0 };
    ::Double acc{ 0.0 };
    ::Double timeRelative{ 0.0 };
    ::Double deltaAcc{ 0.0 };
    ::Double steerAngle{ 0.0 };
    ::adsfi::WayPoint wayPoint;
    ::ara::common::CommonHeader header;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(speed);
        fun(acc);
        fun(timeRelative);
        fun(deltaAcc);
        fun(steerAngle);
        fun(wayPoint);
        fun(header);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(speed);
        fun(acc);
        fun(timeRelative);
        fun(deltaAcc);
        fun(steerAngle);
        fun(wayPoint);
        fun(header);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("speed", speed);
        fun("acc", acc);
        fun("timeRelative", timeRelative);
        fun("deltaAcc", deltaAcc);
        fun("steerAngle", steerAngle);
        fun("wayPoint", wayPoint);
        fun("header", header);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("speed", speed);
        fun("acc", acc);
        fun("timeRelative", timeRelative);
        fun("deltaAcc", deltaAcc);
        fun("steerAngle", steerAngle);
        fun("wayPoint", wayPoint);
        fun("header", header);
    }

    friend bool operator==(const ::adsfi::TrajectoryPoint& lhs, const ::adsfi::TrajectoryPoint& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.speed - rhs.speed)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.acc - rhs.acc)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.timeRelative - rhs.timeRelative)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.deltaAcc - rhs.deltaAcc)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.steerAngle - rhs.steerAngle)) < DBL_EPSILON) && (lhs.wayPoint == rhs.wayPoint) && (lhs.header == rhs.header);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_TRAJECTORYPOINT_H
