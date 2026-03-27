/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPPATROLCONTROLPARAMETER_H
#define ARA_ADSFI_IMPL_TYPE_APPPATROLCONTROLPARAMETER_H

#include "impl_type_string.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct AppPatrolControlParameter {
    ::String command;
    ::String file_track;
    ::String file_map;
    ::String file_task;
    ::String dest;
    ::int32_t type{ 0 };
    bool is_loop{ false };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(command);
        fun(file_track);
        fun(file_map);
        fun(file_task);
        fun(dest);
        fun(type);
        fun(is_loop);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(command);
        fun(file_track);
        fun(file_map);
        fun(file_task);
        fun(dest);
        fun(type);
        fun(is_loop);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("command", command);
        fun("file_track", file_track);
        fun("file_map", file_map);
        fun("file_task", file_task);
        fun("dest", dest);
        fun("type", type);
        fun("is_loop", is_loop);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("command", command);
        fun("file_track", file_track);
        fun("file_map", file_map);
        fun("file_task", file_task);
        fun("dest", dest);
        fun("type", type);
        fun("is_loop", is_loop);
    }

    friend bool operator==(const ::ara::adsfi::AppPatrolControlParameter& lhs, const ::ara::adsfi::AppPatrolControlParameter& rhs) noexcept
    {
        return (lhs.command == rhs.command) && (lhs.file_track == rhs.file_track) && (lhs.file_map == rhs.file_map) && (lhs.file_task == rhs.file_task) && (lhs.dest == rhs.dest) && (lhs.type == rhs.type) && (lhs.is_loop == rhs.is_loop);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPPATROLCONTROLPARAMETER_H
