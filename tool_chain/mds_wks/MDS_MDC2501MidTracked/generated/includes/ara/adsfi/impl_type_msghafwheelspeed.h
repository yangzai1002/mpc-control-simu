/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFWHEELSPEED_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFWHEELSPEED_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint8_t.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct MsgHafWheelSpeed {
    ::uint8_t wheelDirection{ 0U };
    ::int32_t wheelCount{ 0 };
    float wheelSpeedMps{ 0.0F };
    ::uint8_t wheelCountValid{ 0U };
    ::uint8_t wheelSpeedMpsValid{ 0U };
    ::uint8_t wheelSpeedPos{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(wheelDirection);
        fun(wheelCount);
        fun(wheelSpeedMps);
        fun(wheelCountValid);
        fun(wheelSpeedMpsValid);
        fun(wheelSpeedPos);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(wheelDirection);
        fun(wheelCount);
        fun(wheelSpeedMps);
        fun(wheelCountValid);
        fun(wheelSpeedMpsValid);
        fun(wheelSpeedPos);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("wheelDirection", wheelDirection);
        fun("wheelCount", wheelCount);
        fun("wheelSpeedMps", wheelSpeedMps);
        fun("wheelCountValid", wheelCountValid);
        fun("wheelSpeedMpsValid", wheelSpeedMpsValid);
        fun("wheelSpeedPos", wheelSpeedPos);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("wheelDirection", wheelDirection);
        fun("wheelCount", wheelCount);
        fun("wheelSpeedMps", wheelSpeedMps);
        fun("wheelCountValid", wheelCountValid);
        fun("wheelSpeedMpsValid", wheelSpeedMpsValid);
        fun("wheelSpeedPos", wheelSpeedPos);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafWheelSpeed& lhs, const ::ara::adsfi::MsgHafWheelSpeed& rhs) noexcept
    {
        return (lhs.wheelDirection == rhs.wheelDirection) && (lhs.wheelCount == rhs.wheelCount) && (fabs(static_cast<double>(lhs.wheelSpeedMps - rhs.wheelSpeedMps)) < DBL_EPSILON) && (lhs.wheelCountValid == rhs.wheelCountValid) && (lhs.wheelSpeedMpsValid == rhs.wheelSpeedMpsValid) && (lhs.wheelSpeedPos == rhs.wheelSpeedPos);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFWHEELSPEED_H
