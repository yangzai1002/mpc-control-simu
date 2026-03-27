/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFEGOTRAJECTORY_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFEGOTRAJECTORY_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msghafheader.h"
#include "ara/adsfi/impl_type_msghaftrajectorypointvec.h"
#include "ara/adsfi/impl_type_msghafwaypointvec.h"
#include "impl_type_uint8_t.h"
#include "ara/adsfi/impl_type_msghafestop.h"
#include "impl_type_string.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct MsgHafEgoTrajectory {
    ::ara::adsfi::MsgHafHeader egoTrajectoryHeader;
    double trajectoryLength{ 0.0 };
    double trajectoryPeriod{ 0.0 };
    ::ara::adsfi::MsgHafTrajectoryPointVec trajectoryPoints;
    ::ara::adsfi::MsgHafWayPointVec wayPoints;
    ::uint8_t isReplanning{ 0U };
    ::uint8_t gear{ 0U };
    ::ara::adsfi::MsgHafEstop estop;
    ::ara::adsfi::MsgHafHeader routingHeader;
    ::String selfLaneid;
    ::int32_t trajectoryType{ 0 };
    ::String targetLaneId;
    ::uint8_t turnLight{ 0U };
    ::uint8_t isHeld{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(egoTrajectoryHeader);
        fun(trajectoryLength);
        fun(trajectoryPeriod);
        fun(trajectoryPoints);
        fun(wayPoints);
        fun(isReplanning);
        fun(gear);
        fun(estop);
        fun(routingHeader);
        fun(selfLaneid);
        fun(trajectoryType);
        fun(targetLaneId);
        fun(turnLight);
        fun(isHeld);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(egoTrajectoryHeader);
        fun(trajectoryLength);
        fun(trajectoryPeriod);
        fun(trajectoryPoints);
        fun(wayPoints);
        fun(isReplanning);
        fun(gear);
        fun(estop);
        fun(routingHeader);
        fun(selfLaneid);
        fun(trajectoryType);
        fun(targetLaneId);
        fun(turnLight);
        fun(isHeld);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("egoTrajectoryHeader", egoTrajectoryHeader);
        fun("trajectoryLength", trajectoryLength);
        fun("trajectoryPeriod", trajectoryPeriod);
        fun("trajectoryPoints", trajectoryPoints);
        fun("wayPoints", wayPoints);
        fun("isReplanning", isReplanning);
        fun("gear", gear);
        fun("estop", estop);
        fun("routingHeader", routingHeader);
        fun("selfLaneid", selfLaneid);
        fun("trajectoryType", trajectoryType);
        fun("targetLaneId", targetLaneId);
        fun("turnLight", turnLight);
        fun("isHeld", isHeld);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("egoTrajectoryHeader", egoTrajectoryHeader);
        fun("trajectoryLength", trajectoryLength);
        fun("trajectoryPeriod", trajectoryPeriod);
        fun("trajectoryPoints", trajectoryPoints);
        fun("wayPoints", wayPoints);
        fun("isReplanning", isReplanning);
        fun("gear", gear);
        fun("estop", estop);
        fun("routingHeader", routingHeader);
        fun("selfLaneid", selfLaneid);
        fun("trajectoryType", trajectoryType);
        fun("targetLaneId", targetLaneId);
        fun("turnLight", turnLight);
        fun("isHeld", isHeld);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafEgoTrajectory& lhs, const ::ara::adsfi::MsgHafEgoTrajectory& rhs) noexcept
    {
        return (lhs.egoTrajectoryHeader == rhs.egoTrajectoryHeader) && (fabs(static_cast<double>(lhs.trajectoryLength - rhs.trajectoryLength)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.trajectoryPeriod - rhs.trajectoryPeriod)) < DBL_EPSILON) && (lhs.trajectoryPoints == rhs.trajectoryPoints) && (lhs.wayPoints == rhs.wayPoints) && (lhs.isReplanning == rhs.isReplanning) && (lhs.gear == rhs.gear) && (lhs.estop == rhs.estop) && (lhs.routingHeader == rhs.routingHeader) && (lhs.selfLaneid == rhs.selfLaneid) && (lhs.trajectoryType == rhs.trajectoryType) && (lhs.targetLaneId == rhs.targetLaneId) && (lhs.turnLight == rhs.turnLight) && (lhs.isHeld == rhs.isHeld);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFEGOTRAJECTORY_H
