/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_COMMONTIMECHAIN_H
#define ARA_ADSFI_IMPL_TYPE_COMMONTIMECHAIN_H

#include <cfloat>
#include <cmath>
#include "impl_type_string.h"

namespace ara {
namespace adsfi {
struct CommonTimeChain {
    double t1{ 0.0 };
    double t2{ 0.0 };
    ::String from_module;
    ::String to_module;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(t1);
        fun(t2);
        fun(from_module);
        fun(to_module);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(t1);
        fun(t2);
        fun(from_module);
        fun(to_module);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("t1", t1);
        fun("t2", t2);
        fun("from_module", from_module);
        fun("to_module", to_module);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("t1", t1);
        fun("t2", t2);
        fun("from_module", from_module);
        fun("to_module", to_module);
    }

    friend bool operator==(const ::ara::adsfi::CommonTimeChain& lhs, const ::ara::adsfi::CommonTimeChain& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.t1 - rhs.t1)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.t2 - rhs.t2)) < DBL_EPSILON) && (lhs.from_module == rhs.from_module) && (lhs.to_module == rhs.to_module);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_COMMONTIMECHAIN_H
