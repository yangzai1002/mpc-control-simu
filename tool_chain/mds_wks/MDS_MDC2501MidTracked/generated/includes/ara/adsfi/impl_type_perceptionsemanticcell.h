/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PERCEPTIONSEMANTICCELL_H
#define ARA_ADSFI_IMPL_TYPE_PERCEPTIONSEMANTICCELL_H

#include <cfloat>
#include <cmath>

namespace ara {
namespace adsfi {
struct PerceptionSemanticCell {
    float x{ 0.0F };
    float y{ 0.0F };
    double xg{ 0.0 };
    double yg{ 0.0 };
    float dh{ 0.0F };

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
        fun(xg);
        fun(yg);
        fun(dh);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(x);
        fun(y);
        fun(xg);
        fun(yg);
        fun(dh);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("x", x);
        fun("y", y);
        fun("xg", xg);
        fun("yg", yg);
        fun("dh", dh);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("x", x);
        fun("y", y);
        fun("xg", xg);
        fun("yg", yg);
        fun("dh", dh);
    }

    friend bool operator==(const ::ara::adsfi::PerceptionSemanticCell& lhs, const ::ara::adsfi::PerceptionSemanticCell& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.xg - rhs.xg)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.yg - rhs.yg)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.dh - rhs.dh)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PERCEPTIONSEMANTICCELL_H
