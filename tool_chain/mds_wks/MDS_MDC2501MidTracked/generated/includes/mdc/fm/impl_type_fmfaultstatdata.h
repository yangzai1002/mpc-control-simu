/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_IMPL_TYPE_FMFAULTSTATDATA_H
#define MDC_FM_IMPL_TYPE_FMFAULTSTATDATA_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint32.h"
#include "impl_type_uint64.h"
#include "impl_type_double.h"

namespace mdc {
namespace fm {
struct FmFaultStatData {
    ::UInt32 faultCnt{ 0U };
    ::UInt32 faultClearCnt{ 0U };
    ::UInt64 minTimeInvl{ 0U };
    ::UInt64 maxTimeInvl{ 0U };
    ::Double avgTimeInvl{ 0.0 };
    ::UInt32 alarmCnt{ 0U };
    ::UInt32 alarmClearCnt{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(faultCnt);
        fun(faultClearCnt);
        fun(minTimeInvl);
        fun(maxTimeInvl);
        fun(avgTimeInvl);
        fun(alarmCnt);
        fun(alarmClearCnt);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(faultCnt);
        fun(faultClearCnt);
        fun(minTimeInvl);
        fun(maxTimeInvl);
        fun(avgTimeInvl);
        fun(alarmCnt);
        fun(alarmClearCnt);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("faultCnt", faultCnt);
        fun("faultClearCnt", faultClearCnt);
        fun("minTimeInvl", minTimeInvl);
        fun("maxTimeInvl", maxTimeInvl);
        fun("avgTimeInvl", avgTimeInvl);
        fun("alarmCnt", alarmCnt);
        fun("alarmClearCnt", alarmClearCnt);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("faultCnt", faultCnt);
        fun("faultClearCnt", faultClearCnt);
        fun("minTimeInvl", minTimeInvl);
        fun("maxTimeInvl", maxTimeInvl);
        fun("avgTimeInvl", avgTimeInvl);
        fun("alarmCnt", alarmCnt);
        fun("alarmClearCnt", alarmClearCnt);
    }

    friend bool operator==(const ::mdc::fm::FmFaultStatData& lhs, const ::mdc::fm::FmFaultStatData& rhs) noexcept
    {
        return (lhs.faultCnt == rhs.faultCnt) && (lhs.faultClearCnt == rhs.faultClearCnt) && (lhs.minTimeInvl == rhs.minTimeInvl) && (lhs.maxTimeInvl == rhs.maxTimeInvl) && (fabs(static_cast<double>(lhs.avgTimeInvl - rhs.avgTimeInvl)) < DBL_EPSILON) && (lhs.alarmCnt == rhs.alarmCnt) && (lhs.alarmClearCnt == rhs.alarmClearCnt);
    }
};
} // namespace fm
} // namespace mdc


#endif // MDC_FM_IMPL_TYPE_FMFAULTSTATDATA_H
