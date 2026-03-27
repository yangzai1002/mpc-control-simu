/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PERCEPTIONUTRADATA_H
#define ARA_ADSFI_IMPL_TYPE_PERCEPTIONUTRADATA_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32_t.h"
#include "ara/adsfi/impl_type_msgpoint2dvec.h"

namespace ara {
namespace adsfi {
struct PerceptionUtraData {
    double trigger_stamp{ 0.0 };
    ::int32_t id{ 0 };
    float dis{ 0.0F };
    float angle{ 0.0F };
    ::ara::adsfi::MsgPoint2DVec cells;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(trigger_stamp);
        fun(id);
        fun(dis);
        fun(angle);
        fun(cells);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(trigger_stamp);
        fun(id);
        fun(dis);
        fun(angle);
        fun(cells);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("trigger_stamp", trigger_stamp);
        fun("id", id);
        fun("dis", dis);
        fun("angle", angle);
        fun("cells", cells);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("trigger_stamp", trigger_stamp);
        fun("id", id);
        fun("dis", dis);
        fun("angle", angle);
        fun("cells", cells);
    }

    friend bool operator==(const ::ara::adsfi::PerceptionUtraData& lhs, const ::ara::adsfi::PerceptionUtraData& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.trigger_stamp - rhs.trigger_stamp)) < DBL_EPSILON) && (lhs.id == rhs.id) && (fabs(static_cast<double>(lhs.dis - rhs.dis)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.angle - rhs.angle)) < DBL_EPSILON) && (lhs.cells == rhs.cells);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PERCEPTIONUTRADATA_H
