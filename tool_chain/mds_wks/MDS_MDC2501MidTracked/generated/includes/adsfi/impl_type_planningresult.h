/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_PLANNINGRESULT_H
#define ADSFI_IMPL_TYPE_PLANNINGRESULT_H

#include <cfloat>
#include <cmath>
#include "ara/common/impl_type_commonheader.h"
#include "impl_type_double.h"
#include "adsfi/impl_type_trajectorypointvector.h"
#include "adsfi/impl_type_waypointvector.h"
#include "impl_type_boolean.h"
#include "impl_type_uint32.h"
#include "impl_type_string.h"
#include "adsfi/impl_type_estop.h"
#include "impl_type_uint8.h"

namespace adsfi {
struct PlanningResult {
    ::ara::common::CommonHeader header;
    ::Double trajectoryLength{ 0.0 };
    ::Double trajectoryPeriod{ 0.0 };
    ::adsfi::TrajectoryPointVector trajectoryPoints;
    ::adsfi::WayPointVector wayPoints;
    ::Boolean isReplanning{ false };
    ::UInt32 gear{ 0U };
    ::String selfLaneId;
    ::UInt32 trajectoryType{ 0U };
    ::String targetLaneId;
    ::adsfi::Estop estop;
    ::UInt8 turnLight{ 0U };
    ::Boolean isHold{ false };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(trajectoryLength);
        fun(trajectoryPeriod);
        fun(trajectoryPoints);
        fun(wayPoints);
        fun(isReplanning);
        fun(gear);
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
        fun(trajectoryLength);
        fun(trajectoryPeriod);
        fun(trajectoryPoints);
        fun(wayPoints);
        fun(isReplanning);
        fun(gear);
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
        fun("trajectoryLength", trajectoryLength);
        fun("trajectoryPeriod", trajectoryPeriod);
        fun("trajectoryPoints", trajectoryPoints);
        fun("wayPoints", wayPoints);
        fun("isReplanning", isReplanning);
        fun("gear", gear);
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
        fun("trajectoryLength", trajectoryLength);
        fun("trajectoryPeriod", trajectoryPeriod);
        fun("trajectoryPoints", trajectoryPoints);
        fun("wayPoints", wayPoints);
        fun("isReplanning", isReplanning);
        fun("gear", gear);
        fun("selfLaneId", selfLaneId);
        fun("trajectoryType", trajectoryType);
        fun("targetLaneId", targetLaneId);
        fun("estop", estop);
        fun("turnLight", turnLight);
        fun("isHold", isHold);
    }

    friend bool operator==(const ::adsfi::PlanningResult& lhs, const ::adsfi::PlanningResult& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.trajectoryLength - rhs.trajectoryLength)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.trajectoryPeriod - rhs.trajectoryPeriod)) < DBL_EPSILON) && (lhs.trajectoryPoints == rhs.trajectoryPoints) && (lhs.wayPoints == rhs.wayPoints) && (lhs.isReplanning == rhs.isReplanning) && (lhs.gear == rhs.gear) && (lhs.selfLaneId == rhs.selfLaneId) && (lhs.trajectoryType == rhs.trajectoryType) && (lhs.targetLaneId == rhs.targetLaneId) && (lhs.estop == rhs.estop) && (lhs.turnLight == rhs.turnLight) && (lhs.isHold == rhs.isHold);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_PLANNINGRESULT_H
