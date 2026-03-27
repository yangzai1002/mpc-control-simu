/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_BUSINESSPOINT_H
#define ARA_ADSFI_IMPL_TYPE_BUSINESSPOINT_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct BusinessPoint {
    ::int32_t index{ 0 };
    double x{ 0.0 };
    double y{ 0.0 };
    double xg{ 0.0 };
    double yg{ 0.0 };
    double angle{ 0.0 };
    double heading{ 0.0 };
    ::int32_t direction{ 0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(index);
        fun(x);
        fun(y);
        fun(xg);
        fun(yg);
        fun(angle);
        fun(heading);
        fun(direction);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(index);
        fun(x);
        fun(y);
        fun(xg);
        fun(yg);
        fun(angle);
        fun(heading);
        fun(direction);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("index", index);
        fun("x", x);
        fun("y", y);
        fun("xg", xg);
        fun("yg", yg);
        fun("angle", angle);
        fun("heading", heading);
        fun("direction", direction);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("index", index);
        fun("x", x);
        fun("y", y);
        fun("xg", xg);
        fun("yg", yg);
        fun("angle", angle);
        fun("heading", heading);
        fun("direction", direction);
    }

    friend bool operator==(const ::ara::adsfi::BusinessPoint& lhs, const ::ara::adsfi::BusinessPoint& rhs) noexcept
    {
        return (lhs.index == rhs.index) && (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.xg - rhs.xg)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.yg - rhs.yg)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.angle - rhs.angle)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.heading - rhs.heading)) < DBL_EPSILON) && (lhs.direction == rhs.direction);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_BUSINESSPOINT_H
