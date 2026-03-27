/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PERCEPTIONTERRAINPATCH_H
#define ARA_ADSFI_IMPL_TYPE_PERCEPTIONTERRAINPATCH_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msgpoint3f.h"
#include "impl_type_uint8_t.h"
#include "impl_type_float.h"

namespace ara {
namespace adsfi {
struct PerceptionTerrainPatch {
    ::ara::adsfi::MsgPoint3f patch_center;
    float slopx{ 0.0F };
    float slopy{ 0.0F };
    ::uint8_t type{ 0U };
    ::Float terrain_height{ 0.0F };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(patch_center);
        fun(slopx);
        fun(slopy);
        fun(type);
        fun(terrain_height);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(patch_center);
        fun(slopx);
        fun(slopy);
        fun(type);
        fun(terrain_height);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("patch_center", patch_center);
        fun("slopx", slopx);
        fun("slopy", slopy);
        fun("type", type);
        fun("terrain_height", terrain_height);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("patch_center", patch_center);
        fun("slopx", slopx);
        fun("slopy", slopy);
        fun("type", type);
        fun("terrain_height", terrain_height);
    }

    friend bool operator==(const ::ara::adsfi::PerceptionTerrainPatch& lhs, const ::ara::adsfi::PerceptionTerrainPatch& rhs) noexcept
    {
        return (lhs.patch_center == rhs.patch_center) && (fabs(static_cast<double>(lhs.slopx - rhs.slopx)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.slopy - rhs.slopy)) < DBL_EPSILON) && (lhs.type == rhs.type) && (fabs(static_cast<double>(lhs.terrain_height - rhs.terrain_height)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PERCEPTIONTERRAINPATCH_H
