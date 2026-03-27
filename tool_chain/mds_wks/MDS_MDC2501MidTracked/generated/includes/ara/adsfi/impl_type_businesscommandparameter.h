/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_BUSINESSCOMMANDPARAMETER_H
#define ARA_ADSFI_IMPL_TYPE_BUSINESSCOMMANDPARAMETER_H

#include <cfloat>
#include <cmath>
#include "impl_type_string.h"
#include "impl_type_int32_t.h"
#include "ara/adsfi/impl_type_msghafpose.h"

namespace ara {
namespace adsfi {
struct BusinessCommandParameter {
    ::String patrol_name;
    ::String patrol_dest;
    ::int32_t patrol_type{ 0 };
    ::int32_t patrol_loop{ 0 };
    ::String patrol_roads;
    double max_speed{ 0.0 };
    ::int32_t task_avoid_mode{ 0 };
    ::int32_t command{ 0 };
    double follow_min_dis{ 0.0 };
    double follow_thw{ 0.0 };
    ::int32_t follow_mode{ 0 };
    ::int32_t follow_x{ 0 };
    ::int32_t follow_y{ 0 };
    ::int32_t view_width{ 0 };
    ::int32_t view_height{ 0 };
    ::int32_t id{ 0 };
    double return_lat{ 0.0 };
    double return_lon{ 0.0 };
    ::String fence;
    ::ara::adsfi::MsgHafPose pose;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(patrol_name);
        fun(patrol_dest);
        fun(patrol_type);
        fun(patrol_loop);
        fun(patrol_roads);
        fun(max_speed);
        fun(task_avoid_mode);
        fun(command);
        fun(follow_min_dis);
        fun(follow_thw);
        fun(follow_mode);
        fun(follow_x);
        fun(follow_y);
        fun(view_width);
        fun(view_height);
        fun(id);
        fun(return_lat);
        fun(return_lon);
        fun(fence);
        fun(pose);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(patrol_name);
        fun(patrol_dest);
        fun(patrol_type);
        fun(patrol_loop);
        fun(patrol_roads);
        fun(max_speed);
        fun(task_avoid_mode);
        fun(command);
        fun(follow_min_dis);
        fun(follow_thw);
        fun(follow_mode);
        fun(follow_x);
        fun(follow_y);
        fun(view_width);
        fun(view_height);
        fun(id);
        fun(return_lat);
        fun(return_lon);
        fun(fence);
        fun(pose);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("patrol_name", patrol_name);
        fun("patrol_dest", patrol_dest);
        fun("patrol_type", patrol_type);
        fun("patrol_loop", patrol_loop);
        fun("patrol_roads", patrol_roads);
        fun("max_speed", max_speed);
        fun("task_avoid_mode", task_avoid_mode);
        fun("command", command);
        fun("follow_min_dis", follow_min_dis);
        fun("follow_thw", follow_thw);
        fun("follow_mode", follow_mode);
        fun("follow_x", follow_x);
        fun("follow_y", follow_y);
        fun("view_width", view_width);
        fun("view_height", view_height);
        fun("id", id);
        fun("return_lat", return_lat);
        fun("return_lon", return_lon);
        fun("fence", fence);
        fun("pose", pose);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("patrol_name", patrol_name);
        fun("patrol_dest", patrol_dest);
        fun("patrol_type", patrol_type);
        fun("patrol_loop", patrol_loop);
        fun("patrol_roads", patrol_roads);
        fun("max_speed", max_speed);
        fun("task_avoid_mode", task_avoid_mode);
        fun("command", command);
        fun("follow_min_dis", follow_min_dis);
        fun("follow_thw", follow_thw);
        fun("follow_mode", follow_mode);
        fun("follow_x", follow_x);
        fun("follow_y", follow_y);
        fun("view_width", view_width);
        fun("view_height", view_height);
        fun("id", id);
        fun("return_lat", return_lat);
        fun("return_lon", return_lon);
        fun("fence", fence);
        fun("pose", pose);
    }

    friend bool operator==(const ::ara::adsfi::BusinessCommandParameter& lhs, const ::ara::adsfi::BusinessCommandParameter& rhs) noexcept
    {
        return (lhs.patrol_name == rhs.patrol_name) && (lhs.patrol_dest == rhs.patrol_dest) && (lhs.patrol_type == rhs.patrol_type) && (lhs.patrol_loop == rhs.patrol_loop) && (lhs.patrol_roads == rhs.patrol_roads) && (fabs(static_cast<double>(lhs.max_speed - rhs.max_speed)) < DBL_EPSILON) && (lhs.task_avoid_mode == rhs.task_avoid_mode) && (lhs.command == rhs.command) && (fabs(static_cast<double>(lhs.follow_min_dis - rhs.follow_min_dis)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.follow_thw - rhs.follow_thw)) < DBL_EPSILON) && (lhs.follow_mode == rhs.follow_mode) && (lhs.follow_x == rhs.follow_x) && (lhs.follow_y == rhs.follow_y) && (lhs.view_width == rhs.view_width) && (lhs.view_height == rhs.view_height) && (lhs.id == rhs.id) && (fabs(static_cast<double>(lhs.return_lat - rhs.return_lat)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.return_lon - rhs.return_lon)) < DBL_EPSILON) && (lhs.fence == rhs.fence) && (lhs.pose == rhs.pose);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_BUSINESSCOMMANDPARAMETER_H
