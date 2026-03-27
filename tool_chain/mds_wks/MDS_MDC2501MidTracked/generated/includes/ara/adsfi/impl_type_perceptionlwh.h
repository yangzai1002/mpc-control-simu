/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PERCEPTIONLWH_H
#define ARA_ADSFI_IMPL_TYPE_PERCEPTIONLWH_H

#include <cfloat>
#include <cmath>

namespace ara {
namespace adsfi {
struct PerceptionLWH {
    float width{ 0.0F };
    float length{ 0.0F };
    float height{ 0.0F };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(width);
        fun(length);
        fun(height);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(width);
        fun(length);
        fun(height);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("width", width);
        fun("length", length);
        fun("height", height);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("width", width);
        fun("length", length);
        fun("height", height);
    }

    friend bool operator==(const ::ara::adsfi::PerceptionLWH& lhs, const ::ara::adsfi::PerceptionLWH& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.width - rhs.width)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.length - rhs.length)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.height - rhs.height)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PERCEPTIONLWH_H
