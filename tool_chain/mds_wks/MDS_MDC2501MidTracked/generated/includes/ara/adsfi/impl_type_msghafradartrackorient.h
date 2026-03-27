/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKORIENT_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKORIENT_H

#include <cfloat>
#include <cmath>

namespace ara {
namespace adsfi {
struct MsgHafRadarTrackOrient {
    double orientation{ 0.0 };
    double rms{ 0.0 };
    double quality{ 0.0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(orientation);
        fun(rms);
        fun(quality);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(orientation);
        fun(rms);
        fun(quality);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("orientation", orientation);
        fun("rms", rms);
        fun("quality", quality);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("orientation", orientation);
        fun("rms", rms);
        fun("quality", quality);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafRadarTrackOrient& lhs, const ::ara::adsfi::MsgHafRadarTrackOrient& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.orientation - rhs.orientation)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.rms - rhs.rms)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.quality - rhs.quality)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKORIENT_H
