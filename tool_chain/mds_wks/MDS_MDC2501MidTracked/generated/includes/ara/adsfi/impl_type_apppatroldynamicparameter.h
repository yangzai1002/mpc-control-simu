/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPPATROLDYNAMICPARAMETER_H
#define ARA_ADSFI_IMPL_TYPE_APPPATROLDYNAMICPARAMETER_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct AppPatrolDynamicParameter {
    ::int32_t avoid{ 0 };
    double speed{ 0.0 };
    bool paused{ false };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(avoid);
        fun(speed);
        fun(paused);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(avoid);
        fun(speed);
        fun(paused);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("avoid", avoid);
        fun("speed", speed);
        fun("paused", paused);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("avoid", avoid);
        fun("speed", speed);
        fun("paused", paused);
    }

    friend bool operator==(const ::ara::adsfi::AppPatrolDynamicParameter& lhs, const ::ara::adsfi::AppPatrolDynamicParameter& rhs) noexcept
    {
        return (lhs.avoid == rhs.avoid) && (fabs(static_cast<double>(lhs.speed - rhs.speed)) < DBL_EPSILON) && (lhs.paused == rhs.paused);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPPATROLDYNAMICPARAMETER_H
