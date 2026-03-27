/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_EGOTRAJECTORY_IMPL_TYPE_EGOTRAJECTORY_H
#define ARA_EGOTRAJECTORY_IMPL_TYPE_EGOTRAJECTORY_H

#include <cfloat>
#include <cmath>
#include "ara/egotrajectory/impl_type_header.h"
#include "impl_type_uint8.h"
#include "impl_type_posewithcovariance.h"
#include "impl_type_twistwithcovariance.h"
#include "impl_type_accelwithcovariance.h"
#include "impl_type_double.h"
#include "impl_type_trajectorypointvector.h"
#include "impl_type_waypointvector.h"
#include "impl_type_boolean.h"
#include "impl_type_uint32.h"
#include "impl_type_string.h"
#include "ara/egotrajectory/impl_type_estop.h"
#include "impl_type_uint8_t.h"

namespace ara {
namespace egotrajectory {
struct EgoTrajectory {
    ::ara::egotrajectory::Header header;
    ::UInt8 locationState{ 0U };
    ::PoseWithCovariance pose;
    ::TwistWithCovariance velocity;
    ::AccelWithCovariance acceleration;
    ::Double trajectoryLength{ 0.0 };
    ::Double trajectoryPeriod{ 0.0 };
    ::TrajectoryPointVector trajectoryPoints;
    ::WayPointVector wayPoints;
    ::Boolean isReplanning{ false };
    ::UInt32 gear{ 0U };
    ::ara::egotrajectory::Header routingHeader;
    ::String selfLaneId;
    ::UInt32 trajectoryType{ 0U };
    ::String targetLaneId;
    ::ara::egotrajectory::Estop estop;
    ::uint8_t turnLight{ 0U };
    bool isHold{ false };

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
        fun(trajectoryLength);
        fun(trajectoryPeriod);
        fun(trajectoryPoints);
        fun(wayPoints);
        fun(isReplanning);
        fun(gear);
        fun(routingHeader);
        fun(selfLaneId);
        fun(trajectoryType);
        fun(targetLaneId);
        fun(estop);
        fun(turnLight);
        fun(isHold);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(locationState);
        fun(pose);
        fun(velocity);
        fun(acceleration);
        fun(trajectoryLength);
        fun(trajectoryPeriod);
        fun(trajectoryPoints);
        fun(wayPoints);
        fun(isReplanning);
        fun(gear);
        fun(routingHeader);
        fun(selfLaneId);
        fun(trajectoryType);
        fun(targetLaneId);
        fun(estop);
        fun(turnLight);
        fun(isHold);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("locationState", locationState);
        fun("pose", pose);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("trajectoryLength", trajectoryLength);
        fun("trajectoryPeriod", trajectoryPeriod);
        fun("trajectoryPoints", trajectoryPoints);
        fun("wayPoints", wayPoints);
        fun("isReplanning", isReplanning);
        fun("gear", gear);
        fun("routingHeader", routingHeader);
        fun("selfLaneId", selfLaneId);
        fun("trajectoryType", trajectoryType);
        fun("targetLaneId", targetLaneId);
        fun("estop", estop);
        fun("turnLight", turnLight);
        fun("isHold", isHold);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("locationState", locationState);
        fun("pose", pose);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("trajectoryLength", trajectoryLength);
        fun("trajectoryPeriod", trajectoryPeriod);
        fun("trajectoryPoints", trajectoryPoints);
        fun("wayPoints", wayPoints);
        fun("isReplanning", isReplanning);
        fun("gear", gear);
        fun("routingHeader", routingHeader);
        fun("selfLaneId", selfLaneId);
        fun("trajectoryType", trajectoryType);
        fun("targetLaneId", targetLaneId);
        fun("estop", estop);
        fun("turnLight", turnLight);
        fun("isHold", isHold);
    }

    friend bool operator==(const ::ara::egotrajectory::EgoTrajectory& lhs, const ::ara::egotrajectory::EgoTrajectory& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.locationState == rhs.locationState) && (lhs.pose == rhs.pose) && (lhs.velocity == rhs.velocity) && (lhs.acceleration == rhs.acceleration) && (fabs(static_cast<double>(lhs.trajectoryLength - rhs.trajectoryLength)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.trajectoryPeriod - rhs.trajectoryPeriod)) < DBL_EPSILON) && (lhs.trajectoryPoints == rhs.trajectoryPoints) && (lhs.wayPoints == rhs.wayPoints) && (lhs.isReplanning == rhs.isReplanning) && (lhs.gear == rhs.gear) && (lhs.routingHeader == rhs.routingHeader) && (lhs.selfLaneId == rhs.selfLaneId) && (lhs.trajectoryType == rhs.trajectoryType) && (lhs.targetLaneId == rhs.targetLaneId) && (lhs.estop == rhs.estop) && (lhs.turnLight == rhs.turnLight) && (lhs.isHold == rhs.isHold);
    }
};
} // namespace egotrajectory
} // namespace ara


#endif // ARA_EGOTRAJECTORY_IMPL_TYPE_EGOTRAJECTORY_H
