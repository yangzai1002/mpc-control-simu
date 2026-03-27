/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_LANE_H
#define IMPL_TYPE_LANE_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32.h"
#include "impl_type_float.h"
#include "impl_type_uint8.h"
#include "impl_type_pose2darray.h"
#include "impl_type_sublanearray.h"

struct Lane {
    ::Int32 id{ 0 };
    ::Float speed_limit{ 0.0F };
    ::UInt8 left_changable{ 0U };
    ::Float left_changable_distance{ 0.0F };
    ::Float right_changable_distance{ 0.0F };
    ::UInt8 right_changable{ 0U };
    ::Pose2DArray road_points;
    ::Pose2DArray left_edge;
    ::Pose2DArray right_edge;
    ::Int32 next_lane_id{ 0 };
    ::Int32 previous_lane_id{ 0 };
    ::Int32 left_lane_id{ 0 };
    ::Int32 right_lane_id{ 0 };
    ::UInt8 traffic_light_state{ 0U };
    ::Float traffic_light_distance{ 0.0F };
    ::Float stop_sign_distance{ 0.0F };
    ::Float parking_pole_distance{ 0.0F };
    ::Float cross_walk_distance{ 0.0F };
    ::UInt8 type{ 0U };
    ::SubLaneArray sub_lanes;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(speed_limit);
        fun(left_changable);
        fun(left_changable_distance);
        fun(right_changable_distance);
        fun(right_changable);
        fun(road_points);
        fun(left_edge);
        fun(right_edge);
        fun(next_lane_id);
        fun(previous_lane_id);
        fun(left_lane_id);
        fun(right_lane_id);
        fun(traffic_light_state);
        fun(traffic_light_distance);
        fun(stop_sign_distance);
        fun(parking_pole_distance);
        fun(cross_walk_distance);
        fun(type);
        fun(sub_lanes);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(speed_limit);
        fun(left_changable);
        fun(left_changable_distance);
        fun(right_changable_distance);
        fun(right_changable);
        fun(road_points);
        fun(left_edge);
        fun(right_edge);
        fun(next_lane_id);
        fun(previous_lane_id);
        fun(left_lane_id);
        fun(right_lane_id);
        fun(traffic_light_state);
        fun(traffic_light_distance);
        fun(stop_sign_distance);
        fun(parking_pole_distance);
        fun(cross_walk_distance);
        fun(type);
        fun(sub_lanes);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("speed_limit", speed_limit);
        fun("left_changable", left_changable);
        fun("left_changable_distance", left_changable_distance);
        fun("right_changable_distance", right_changable_distance);
        fun("right_changable", right_changable);
        fun("road_points", road_points);
        fun("left_edge", left_edge);
        fun("right_edge", right_edge);
        fun("next_lane_id", next_lane_id);
        fun("previous_lane_id", previous_lane_id);
        fun("left_lane_id", left_lane_id);
        fun("right_lane_id", right_lane_id);
        fun("traffic_light_state", traffic_light_state);
        fun("traffic_light_distance", traffic_light_distance);
        fun("stop_sign_distance", stop_sign_distance);
        fun("parking_pole_distance", parking_pole_distance);
        fun("cross_walk_distance", cross_walk_distance);
        fun("type", type);
        fun("sub_lanes", sub_lanes);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("speed_limit", speed_limit);
        fun("left_changable", left_changable);
        fun("left_changable_distance", left_changable_distance);
        fun("right_changable_distance", right_changable_distance);
        fun("right_changable", right_changable);
        fun("road_points", road_points);
        fun("left_edge", left_edge);
        fun("right_edge", right_edge);
        fun("next_lane_id", next_lane_id);
        fun("previous_lane_id", previous_lane_id);
        fun("left_lane_id", left_lane_id);
        fun("right_lane_id", right_lane_id);
        fun("traffic_light_state", traffic_light_state);
        fun("traffic_light_distance", traffic_light_distance);
        fun("stop_sign_distance", stop_sign_distance);
        fun("parking_pole_distance", parking_pole_distance);
        fun("cross_walk_distance", cross_walk_distance);
        fun("type", type);
        fun("sub_lanes", sub_lanes);
    }

    friend bool operator==(const ::Lane& lhs, const ::Lane& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (fabs(static_cast<double>(lhs.speed_limit - rhs.speed_limit)) < DBL_EPSILON) && (lhs.left_changable == rhs.left_changable) && (fabs(static_cast<double>(lhs.left_changable_distance - rhs.left_changable_distance)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.right_changable_distance - rhs.right_changable_distance)) < DBL_EPSILON) && (lhs.right_changable == rhs.right_changable) && (lhs.road_points == rhs.road_points) && (lhs.left_edge == rhs.left_edge) && (lhs.right_edge == rhs.right_edge) && (lhs.next_lane_id == rhs.next_lane_id) && (lhs.previous_lane_id == rhs.previous_lane_id) && (lhs.left_lane_id == rhs.left_lane_id) && (lhs.right_lane_id == rhs.right_lane_id) && (lhs.traffic_light_state == rhs.traffic_light_state) && (fabs(static_cast<double>(lhs.traffic_light_distance - rhs.traffic_light_distance)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.stop_sign_distance - rhs.stop_sign_distance)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.parking_pole_distance - rhs.parking_pole_distance)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.cross_walk_distance - rhs.cross_walk_distance)) < DBL_EPSILON) && (lhs.type == rhs.type) && (lhs.sub_lanes == rhs.sub_lanes);
    }
};


#endif // IMPL_TYPE_LANE_H
