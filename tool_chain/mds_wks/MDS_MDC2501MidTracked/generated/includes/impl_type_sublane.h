/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_SUBLANE_H
#define IMPL_TYPE_SUBLANE_H

#include "impl_type_int32.h"
#include "impl_type_uint8.h"
#include "impl_type_pose2d.h"

struct SubLane {
    ::Int32 lane_id{ 0 };
    ::UInt8 priority{ 0U };
    ::Pose2D cross_point;
    ::Pose2D stop_point;

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(lane_id);
        fun(priority);
        fun(cross_point);
        fun(stop_point);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(lane_id);
        fun(priority);
        fun(cross_point);
        fun(stop_point);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("lane_id", lane_id);
        fun("priority", priority);
        fun("cross_point", cross_point);
        fun("stop_point", stop_point);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("lane_id", lane_id);
        fun("priority", priority);
        fun("cross_point", cross_point);
        fun("stop_point", stop_point);
    }

    friend bool operator==(const ::SubLane& lhs, const ::SubLane& rhs) noexcept
    {
        return (lhs.lane_id == rhs.lane_id) && (lhs.priority == rhs.priority) && (lhs.cross_point == rhs.cross_point) && (lhs.stop_point == rhs.stop_point);
    }
};


#endif // IMPL_TYPE_SUBLANE_H
