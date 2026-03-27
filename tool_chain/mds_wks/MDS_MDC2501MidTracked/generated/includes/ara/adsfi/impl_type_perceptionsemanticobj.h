/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PERCEPTIONSEMANTICOBJ_H
#define ARA_ADSFI_IMPL_TYPE_PERCEPTIONSEMANTICOBJ_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint8_t.h"
#include "ara/adsfi/impl_type_perceptionlwh.h"
#include "ara/adsfi/impl_type_perceptionsemanticcellvec.h"
#include "impl_type_int32_t.h"
#include "ara/adsfi/impl_type_msghafrect2d.h"

namespace ara {
namespace adsfi {
struct PerceptionSemanticObj {
    float x{ 0.0F };
    float y{ 0.0F };
    double xg{ 0.0 };
    double yg{ 0.0 };
    float z{ 0.0F };
    ::uint8_t type{ 0U };
    ::ara::adsfi::PerceptionLWH obj_size;
    float heading{ 0.0F };
    float speed{ 0.0F };
    ::ara::adsfi::PerceptionSemanticCellVec cells;
    ::int32_t plan_risk_level{ 0 };
    ::ara::adsfi::MsgHafRect2D rect_2d;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(x);
        fun(y);
        fun(xg);
        fun(yg);
        fun(z);
        fun(type);
        fun(obj_size);
        fun(heading);
        fun(speed);
        fun(cells);
        fun(plan_risk_level);
        fun(rect_2d);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(x);
        fun(y);
        fun(xg);
        fun(yg);
        fun(z);
        fun(type);
        fun(obj_size);
        fun(heading);
        fun(speed);
        fun(cells);
        fun(plan_risk_level);
        fun(rect_2d);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("x", x);
        fun("y", y);
        fun("xg", xg);
        fun("yg", yg);
        fun("z", z);
        fun("type", type);
        fun("obj_size", obj_size);
        fun("heading", heading);
        fun("speed", speed);
        fun("cells", cells);
        fun("plan_risk_level", plan_risk_level);
        fun("rect_2d", rect_2d);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("x", x);
        fun("y", y);
        fun("xg", xg);
        fun("yg", yg);
        fun("z", z);
        fun("type", type);
        fun("obj_size", obj_size);
        fun("heading", heading);
        fun("speed", speed);
        fun("cells", cells);
        fun("plan_risk_level", plan_risk_level);
        fun("rect_2d", rect_2d);
    }

    friend bool operator==(const ::ara::adsfi::PerceptionSemanticObj& lhs, const ::ara::adsfi::PerceptionSemanticObj& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.xg - rhs.xg)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.yg - rhs.yg)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.z - rhs.z)) < DBL_EPSILON) && (lhs.type == rhs.type) && (lhs.obj_size == rhs.obj_size) && (fabs(static_cast<double>(lhs.heading - rhs.heading)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.speed - rhs.speed)) < DBL_EPSILON) && (lhs.cells == rhs.cells) && (lhs.plan_risk_level == rhs.plan_risk_level) && (lhs.rect_2d == rhs.rect_2d);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PERCEPTIONSEMANTICOBJ_H
