/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFRADARMODEDATAINFO_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFRADARMODEDATAINFO_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msgpoint3d.h"

namespace ara {
namespace adsfi {
struct MsgHafRadarModeDatainfo {
    double x{ 0.0 };
    double y{ 0.0 };
    double z{ 0.0 };
    ::ara::adsfi::MsgPoint3D rms;
    ::ara::adsfi::MsgPoint3D quality;

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(x);
        fun(y);
        fun(z);
        fun(rms);
        fun(quality);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(x);
        fun(y);
        fun(z);
        fun(rms);
        fun(quality);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("x", x);
        fun("y", y);
        fun("z", z);
        fun("rms", rms);
        fun("quality", quality);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("x", x);
        fun("y", y);
        fun("z", z);
        fun("rms", rms);
        fun("quality", quality);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafRadarModeDatainfo& lhs, const ::ara::adsfi::MsgHafRadarModeDatainfo& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.z - rhs.z)) < DBL_EPSILON) && (lhs.rms == rhs.rms) && (lhs.quality == rhs.quality);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFRADARMODEDATAINFO_H
