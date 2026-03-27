/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_FAULTINFO_H
#define ARA_ADSFI_IMPL_TYPE_FAULTINFO_H

#include <cfloat>
#include <cmath>
#include "impl_type_string.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct FaultInfo {
    ::String code;
    ::int32_t handle{ 0 };
    ::String desc;
    ::String extra_desc;
    double timestamp{ 0.0 };
    ::String from;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(code);
        fun(handle);
        fun(desc);
        fun(extra_desc);
        fun(timestamp);
        fun(from);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(code);
        fun(handle);
        fun(desc);
        fun(extra_desc);
        fun(timestamp);
        fun(from);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("code", code);
        fun("handle", handle);
        fun("desc", desc);
        fun("extra_desc", extra_desc);
        fun("timestamp", timestamp);
        fun("from", from);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("code", code);
        fun("handle", handle);
        fun("desc", desc);
        fun("extra_desc", extra_desc);
        fun("timestamp", timestamp);
        fun("from", from);
    }

    friend bool operator==(const ::ara::adsfi::FaultInfo& lhs, const ::ara::adsfi::FaultInfo& rhs) noexcept
    {
        return (lhs.code == rhs.code) && (lhs.handle == rhs.handle) && (lhs.desc == rhs.desc) && (lhs.extra_desc == rhs.extra_desc) && (fabs(static_cast<double>(lhs.timestamp - rhs.timestamp)) < DBL_EPSILON) && (lhs.from == rhs.from);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_FAULTINFO_H
