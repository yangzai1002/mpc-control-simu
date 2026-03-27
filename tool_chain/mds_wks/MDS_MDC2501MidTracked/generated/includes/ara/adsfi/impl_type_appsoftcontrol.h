/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPSOFTCONTROL_H
#define ARA_ADSFI_IMPL_TYPE_APPSOFTCONTROL_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct AppSoftControl {
    ::int32_t low_beam_ctrl{ 0 };
    float max_speed{ 0.0F };
    ::int32_t other_ctrl{ 0 };
    ::int32_t record_return_path{ 0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(low_beam_ctrl);
        fun(max_speed);
        fun(other_ctrl);
        fun(record_return_path);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(low_beam_ctrl);
        fun(max_speed);
        fun(other_ctrl);
        fun(record_return_path);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("low_beam_ctrl", low_beam_ctrl);
        fun("max_speed", max_speed);
        fun("other_ctrl", other_ctrl);
        fun("record_return_path", record_return_path);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("low_beam_ctrl", low_beam_ctrl);
        fun("max_speed", max_speed);
        fun("other_ctrl", other_ctrl);
        fun("record_return_path", record_return_path);
    }

    friend bool operator==(const ::ara::adsfi::AppSoftControl& lhs, const ::ara::adsfi::AppSoftControl& rhs) noexcept
    {
        return (lhs.low_beam_ctrl == rhs.low_beam_ctrl) && (fabs(static_cast<double>(lhs.max_speed - rhs.max_speed)) < DBL_EPSILON) && (lhs.other_ctrl == rhs.other_ctrl) && (lhs.record_return_path == rhs.record_return_path);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPSOFTCONTROL_H
