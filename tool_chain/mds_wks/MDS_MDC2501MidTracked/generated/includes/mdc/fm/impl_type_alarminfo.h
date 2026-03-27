/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_IMPL_TYPE_ALARMINFO_H
#define MDC_FM_IMPL_TYPE_ALARMINFO_H

#include "impl_type_uint16.h"
#include "impl_type_uint8.h"
#include "impl_type_uint64.h"
#include "impl_type_string.h"

namespace mdc {
namespace fm {
struct AlarmInfo {
    ::UInt16 alarmId{ 0U };
    ::UInt16 alarmObj{ 0U };
    ::UInt8 clss{ 0U };
    ::UInt8 level{ 0U };
    ::UInt8 status{ 0U };
    ::UInt64 time{ 0U };
    ::String desc;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(alarmId);
        fun(alarmObj);
        fun(clss);
        fun(level);
        fun(status);
        fun(time);
        fun(desc);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(alarmId);
        fun(alarmObj);
        fun(clss);
        fun(level);
        fun(status);
        fun(time);
        fun(desc);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("alarmId", alarmId);
        fun("alarmObj", alarmObj);
        fun("clss", clss);
        fun("level", level);
        fun("status", status);
        fun("time", time);
        fun("desc", desc);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("alarmId", alarmId);
        fun("alarmObj", alarmObj);
        fun("clss", clss);
        fun("level", level);
        fun("status", status);
        fun("time", time);
        fun("desc", desc);
    }

    friend bool operator==(const ::mdc::fm::AlarmInfo& lhs, const ::mdc::fm::AlarmInfo& rhs) noexcept
    {
        return (lhs.alarmId == rhs.alarmId) && (lhs.alarmObj == rhs.alarmObj) && (lhs.clss == rhs.clss) && (lhs.level == rhs.level) && (lhs.status == rhs.status) && (lhs.time == rhs.time) && (lhs.desc == rhs.desc);
    }
};
} // namespace fm
} // namespace mdc


#endif // MDC_FM_IMPL_TYPE_ALARMINFO_H
