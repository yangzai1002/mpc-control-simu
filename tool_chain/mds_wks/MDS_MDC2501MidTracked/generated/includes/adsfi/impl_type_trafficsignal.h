/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_TRAFFICSIGNAL_H
#define ADSFI_IMPL_TYPE_TRAFFICSIGNAL_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"
#include "impl_type_uint32.h"
#include "impl_type_uint8.h"
#include "impl_type_rect3d.h"
#include "impl_type_int32array.h"
#include "impl_type_point.h"
#include "impl_type_boolean.h"
#include "ara/common/impl_type_commontime.h"

namespace adsfi {
struct TrafficSignal {
    ::Double confidence{ 0.0 };
    ::UInt32 id{ 0U };
    ::UInt8 color{ 0U };
    ::Rect3d rect3d;
    ::Int32Array laneId;
    ::Point position;
    ::UInt8 type{ 0U };
    ::Double distance{ 0.0 };
    ::Boolean flash{ false };
    ::Double timeTrack{ 0.0 };
    ::Double timeFlash{ 0.0 };
    ::Double timeV2X{ 0.0 };
    ::ara::common::CommonTime timeCreation;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(confidence);
        fun(id);
        fun(color);
        fun(rect3d);
        fun(laneId);
        fun(position);
        fun(type);
        fun(distance);
        fun(flash);
        fun(timeTrack);
        fun(timeFlash);
        fun(timeV2X);
        fun(timeCreation);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(confidence);
        fun(id);
        fun(color);
        fun(rect3d);
        fun(laneId);
        fun(position);
        fun(type);
        fun(distance);
        fun(flash);
        fun(timeTrack);
        fun(timeFlash);
        fun(timeV2X);
        fun(timeCreation);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("confidence", confidence);
        fun("id", id);
        fun("color", color);
        fun("rect3d", rect3d);
        fun("laneId", laneId);
        fun("position", position);
        fun("type", type);
        fun("distance", distance);
        fun("flash", flash);
        fun("timeTrack", timeTrack);
        fun("timeFlash", timeFlash);
        fun("timeV2X", timeV2X);
        fun("timeCreation", timeCreation);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("confidence", confidence);
        fun("id", id);
        fun("color", color);
        fun("rect3d", rect3d);
        fun("laneId", laneId);
        fun("position", position);
        fun("type", type);
        fun("distance", distance);
        fun("flash", flash);
        fun("timeTrack", timeTrack);
        fun("timeFlash", timeFlash);
        fun("timeV2X", timeV2X);
        fun("timeCreation", timeCreation);
    }

    friend bool operator==(const ::adsfi::TrafficSignal& lhs, const ::adsfi::TrafficSignal& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.confidence - rhs.confidence)) < DBL_EPSILON) && (lhs.id == rhs.id) && (lhs.color == rhs.color) && (lhs.rect3d == rhs.rect3d) && (lhs.laneId == rhs.laneId) && (lhs.position == rhs.position) && (lhs.type == rhs.type) && (fabs(static_cast<double>(lhs.distance - rhs.distance)) < DBL_EPSILON) && (lhs.flash == rhs.flash) && (fabs(static_cast<double>(lhs.timeTrack - rhs.timeTrack)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.timeFlash - rhs.timeFlash)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.timeV2X - rhs.timeV2X)) < DBL_EPSILON) && (lhs.timeCreation == rhs.timeCreation);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_TRAFFICSIGNAL_H
