/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_BUSINESSPATH_H
#define ARA_ADSFI_IMPL_TYPE_BUSINESSPATH_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_businesspointvec.h"

namespace ara {
namespace adsfi {
struct BusinessPath {
    ::ara::adsfi::BusinessPointVec points;
    double length{ 0.0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(points);
        fun(length);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(points);
        fun(length);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("points", points);
        fun("length", length);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("points", points);
        fun("length", length);
    }

    friend bool operator==(const ::ara::adsfi::BusinessPath& lhs, const ::ara::adsfi::BusinessPath& rhs) noexcept
    {
        return (lhs.points == rhs.points) && (fabs(static_cast<double>(lhs.length - rhs.length)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_BUSINESSPATH_H
