/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_TRACKPATH_H
#define ARA_ADSFI_IMPL_TYPE_TRACKPATH_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_int32_t.h"
#include "ara/adsfi/impl_type_trackpointvec.h"

namespace ara {
namespace adsfi {
struct TrackPath {
    ::ara::adsfi::CommonHeader header;
    ::int32_t status{ 0 };
    ::int32_t avoid{ 0 };
    double speed{ 0.0 };
    ::ara::adsfi::TrackPointVec points;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(status);
        fun(avoid);
        fun(speed);
        fun(points);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(status);
        fun(avoid);
        fun(speed);
        fun(points);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("status", status);
        fun("avoid", avoid);
        fun("speed", speed);
        fun("points", points);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("status", status);
        fun("avoid", avoid);
        fun("speed", speed);
        fun("points", points);
    }

    friend bool operator==(const ::ara::adsfi::TrackPath& lhs, const ::ara::adsfi::TrackPath& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.status == rhs.status) && (lhs.avoid == rhs.avoid) && (fabs(static_cast<double>(lhs.speed - rhs.speed)) < DBL_EPSILON) && (lhs.points == rhs.points);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_TRACKPATH_H
