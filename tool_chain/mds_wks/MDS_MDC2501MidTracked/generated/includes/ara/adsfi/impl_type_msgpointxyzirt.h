/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGPOINTXYZIRT_H
#define ARA_ADSFI_IMPL_TYPE_MSGPOINTXYZIRT_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint16_t.h"
#include "impl_type_uint32_t.h"

namespace ara {
namespace adsfi {
struct MsgPointXYZIRT {
    float x{ 0.0F };
    float y{ 0.0F };
    float z{ 0.0F };
    float angle{ 0.0F };
    ::uint16_t intensity{ 0U };
    ::uint16_t ring{ 0U };
    ::uint32_t latency{ 0U };

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
        fun(angle);
        fun(intensity);
        fun(ring);
        fun(latency);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(x);
        fun(y);
        fun(z);
        fun(angle);
        fun(intensity);
        fun(ring);
        fun(latency);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("x", x);
        fun("y", y);
        fun("z", z);
        fun("angle", angle);
        fun("intensity", intensity);
        fun("ring", ring);
        fun("latency", latency);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("x", x);
        fun("y", y);
        fun("z", z);
        fun("angle", angle);
        fun("intensity", intensity);
        fun("ring", ring);
        fun("latency", latency);
    }

    friend bool operator==(const ::ara::adsfi::MsgPointXYZIRT& lhs, const ::ara::adsfi::MsgPointXYZIRT& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.z - rhs.z)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.angle - rhs.angle)) < DBL_EPSILON) && (lhs.intensity == rhs.intensity) && (lhs.ring == rhs.ring) && (lhs.latency == rhs.latency);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGPOINTXYZIRT_H
