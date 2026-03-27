/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_COMMONFAULTINFO_H
#define ARA_ADSFI_IMPL_TYPE_COMMONFAULTINFO_H

#include <cfloat>
#include <cmath>
#include "impl_type_string.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct CommonFaultInfo {
    double t{ 0.0 };
    ::String info;
    ::int32_t error_code{ 0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(t);
        fun(info);
        fun(error_code);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(t);
        fun(info);
        fun(error_code);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("t", t);
        fun("info", info);
        fun("error_code", error_code);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("t", t);
        fun("info", info);
        fun("error_code", error_code);
    }

    friend bool operator==(const ::ara::adsfi::CommonFaultInfo& lhs, const ::ara::adsfi::CommonFaultInfo& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.t - rhs.t)) < DBL_EPSILON) && (lhs.info == rhs.info) && (lhs.error_code == rhs.error_code);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_COMMONFAULTINFO_H
