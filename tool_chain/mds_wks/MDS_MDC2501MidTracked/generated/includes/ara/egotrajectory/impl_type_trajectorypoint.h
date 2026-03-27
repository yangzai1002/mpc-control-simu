/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_EGOTRAJECTORY_IMPL_TYPE_TRAJECTORYPOINT_H
#define ARA_EGOTRAJECTORY_IMPL_TYPE_TRAJECTORYPOINT_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"
#include "ara/egotrajectory/impl_type_waypoint.h"
#include "ara/egotrajectory/impl_type_header.h"

namespace ara {
namespace egotrajectory {
struct TrajectoryPoint {
    ::Double speed{ 0.0 };
    ::Double acc{ 0.0 };
    ::Double timeRelative{ 0.0 };
    ::Double deltaAcc{ 0.0 };
    ::Double steerAngle{ 0.0 };
    ::ara::egotrajectory::WayPoint wayPoint;
    ::ara::egotrajectory::Header header;

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

    friend bool operator==(const ::ara::egotrajectory::TrajectoryPoint& lhs, const ::ara::egotrajectory::TrajectoryPoint& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.speed - rhs.speed)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.acc - rhs.acc)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.timeRelative - rhs.timeRelative)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.deltaAcc - rhs.deltaAcc)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.steerAngle - rhs.steerAngle)) < DBL_EPSILON) && (lhs.wayPoint == rhs.wayPoint) && (lhs.header == rhs.header);
    }
};
} // namespace egotrajectory
} // namespace ara


#endif // ARA_EGOTRAJECTORY_IMPL_TYPE_TRAJECTORYPOINT_H
