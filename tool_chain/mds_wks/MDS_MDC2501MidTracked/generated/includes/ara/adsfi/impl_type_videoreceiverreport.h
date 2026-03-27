/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VIDEORECEIVERREPORT_H
#define ARA_ADSFI_IMPL_TYPE_VIDEORECEIVERREPORT_H

#include <cfloat>
#include <cmath>
#include "impl_type_int64_t.h"

namespace ara {
namespace adsfi {
struct VideoReceiverReport {
    ::int64_t ts{ 0 };
    double fps{ 0.0 };
    double bps{ 0.0 };
    double total_latency{ 0.0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(ts);
        fun(fps);
        fun(bps);
        fun(total_latency);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(ts);
        fun(fps);
        fun(bps);
        fun(total_latency);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("ts", ts);
        fun("fps", fps);
        fun("bps", bps);
        fun("total_latency", total_latency);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("ts", ts);
        fun("fps", fps);
        fun("bps", bps);
        fun("total_latency", total_latency);
    }

    friend bool operator==(const ::ara::adsfi::VideoReceiverReport& lhs, const ::ara::adsfi::VideoReceiverReport& rhs) noexcept
    {
        return (lhs.ts == rhs.ts) && (fabs(static_cast<double>(lhs.fps - rhs.fps)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.bps - rhs.bps)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.total_latency - rhs.total_latency)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VIDEORECEIVERREPORT_H
