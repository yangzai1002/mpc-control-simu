/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKYAW_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKYAW_H

#include <cfloat>
#include <cmath>

namespace ara {
namespace adsfi {
struct MsgHafRadarTrackYaw {
    double yawRate{ 0.0 };
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
        fun(yawRate);
        fun(rms);
        fun(quality);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(yawRate);
        fun(rms);
        fun(quality);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("yawRate", yawRate);
        fun("rms", rms);
        fun("quality", quality);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("yawRate", yawRate);
        fun("rms", rms);
        fun("quality", quality);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafRadarTrackYaw& lhs, const ::ara::adsfi::MsgHafRadarTrackYaw& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.yawRate - rhs.yawRate)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.rms - rhs.rms)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.quality - rhs.quality)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKYAW_H
