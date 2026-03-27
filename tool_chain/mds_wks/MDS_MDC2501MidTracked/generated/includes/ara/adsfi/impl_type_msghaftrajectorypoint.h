/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFTRAJECTORYPOINT_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFTRAJECTORYPOINT_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msghafheader.h"
#include "ara/adsfi/impl_type_msghafwaypoint.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct MsgHafTrajectoryPoint {
    ::ara::adsfi::MsgHafHeader header;
    ::ara::adsfi::MsgHafWayPoint wayPoint;
    double speed{ 0.0 };
    double acc{ 0.0 };
    double timeRelative{ 0.0 };
    double deltaAcc{ 0.0 };
    double steerAngle{ 0.0 };
    float length{ 0.0F };
    ::int32_t risk_level{ 0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(wayPoint);
        fun(speed);
        fun(acc);
        fun(timeRelative);
        fun(deltaAcc);
        fun(steerAngle);
        fun(length);
        fun(risk_level);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(wayPoint);
        fun(speed);
        fun(acc);
        fun(timeRelative);
        fun(deltaAcc);
        fun(steerAngle);
        fun(length);
        fun(risk_level);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("wayPoint", wayPoint);
        fun("speed", speed);
        fun("acc", acc);
        fun("timeRelative", timeRelative);
        fun("deltaAcc", deltaAcc);
        fun("steerAngle", steerAngle);
        fun("length", length);
        fun("risk_level", risk_level);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("wayPoint", wayPoint);
        fun("speed", speed);
        fun("acc", acc);
        fun("timeRelative", timeRelative);
        fun("deltaAcc", deltaAcc);
        fun("steerAngle", steerAngle);
        fun("length", length);
        fun("risk_level", risk_level);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafTrajectoryPoint& lhs, const ::ara::adsfi::MsgHafTrajectoryPoint& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.wayPoint == rhs.wayPoint) && (fabs(static_cast<double>(lhs.speed - rhs.speed)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.acc - rhs.acc)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.timeRelative - rhs.timeRelative)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.deltaAcc - rhs.deltaAcc)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.steerAngle - rhs.steerAngle)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.length - rhs.length)) < DBL_EPSILON) && (lhs.risk_level == rhs.risk_level);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFTRAJECTORYPOINT_H
