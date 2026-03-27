/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_TRAFFICLIGHT_IMPL_TYPE_TRAFFICLIGHT_H
#define ARA_TRAFFICLIGHT_IMPL_TYPE_TRAFFICLIGHT_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"
#include "impl_type_int32.h"
#include "impl_type_uint8.h"
#include "impl_type_rect.h"
#include "impl_type_int32array.h"
#include "impl_type_point.h"

namespace ara {
namespace trafficlight {
struct TrafficLight {
    ::Double confidence{ 0.0 };
    ::Int32 objectID{ 0 };
    ::UInt8 color{ 0U };
    ::Rect box_image;
    ::Int32Array lanes_id;
    ::Point position;
    ::UInt8 type{ 0U };
    ::Double distance{ 0.0 };
    bool flash{ false };
    ::Double time_track{ 0.0 };
    ::Double time_flash{ 0.0 };
    ::Double time_v2x{ 0.0 };
    ::Double time_creation{ 0.0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(confidence);
        fun(objectID);
        fun(color);
        fun(box_image);
        fun(lanes_id);
        fun(position);
        fun(type);
        fun(distance);
        fun(flash);
        fun(time_track);
        fun(time_flash);
        fun(time_v2x);
        fun(time_creation);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(confidence);
        fun(objectID);
        fun(color);
        fun(box_image);
        fun(lanes_id);
        fun(position);
        fun(type);
        fun(distance);
        fun(flash);
        fun(time_track);
        fun(time_flash);
        fun(time_v2x);
        fun(time_creation);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("confidence", confidence);
        fun("objectID", objectID);
        fun("color", color);
        fun("box_image", box_image);
        fun("lanes_id", lanes_id);
        fun("position", position);
        fun("type", type);
        fun("distance", distance);
        fun("flash", flash);
        fun("time_track", time_track);
        fun("time_flash", time_flash);
        fun("time_v2x", time_v2x);
        fun("time_creation", time_creation);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("confidence", confidence);
        fun("objectID", objectID);
        fun("color", color);
        fun("box_image", box_image);
        fun("lanes_id", lanes_id);
        fun("position", position);
        fun("type", type);
        fun("distance", distance);
        fun("flash", flash);
        fun("time_track", time_track);
        fun("time_flash", time_flash);
        fun("time_v2x", time_v2x);
        fun("time_creation", time_creation);
    }

    friend bool operator==(const ::ara::trafficlight::TrafficLight& lhs, const ::ara::trafficlight::TrafficLight& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.confidence - rhs.confidence)) < DBL_EPSILON) && (lhs.objectID == rhs.objectID) && (lhs.color == rhs.color) && (lhs.box_image == rhs.box_image) && (lhs.lanes_id == rhs.lanes_id) && (lhs.position == rhs.position) && (lhs.type == rhs.type) && (fabs(static_cast<double>(lhs.distance - rhs.distance)) < DBL_EPSILON) && (lhs.flash == rhs.flash) && (fabs(static_cast<double>(lhs.time_track - rhs.time_track)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.time_flash - rhs.time_flash)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.time_v2x - rhs.time_v2x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.time_creation - rhs.time_creation)) < DBL_EPSILON);
    }
};
} // namespace trafficlight
} // namespace ara


#endif // ARA_TRAFFICLIGHT_IMPL_TYPE_TRAFFICLIGHT_H
