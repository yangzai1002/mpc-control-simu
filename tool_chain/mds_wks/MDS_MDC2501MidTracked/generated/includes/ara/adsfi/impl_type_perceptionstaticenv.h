/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PERCEPTIONSTATICENV_H
#define ARA_ADSFI_IMPL_TYPE_PERCEPTIONSTATICENV_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_commonheader.h"
#include "ara/adsfi/impl_type_perceptionsemanticobjvec.h"
#include "ara/adsfi/impl_type_perceptionterrainpatchvec.h"
#include "impl_type_int16.h"
#include "ara/adsfi/impl_type_idpuint8vec.h"

namespace ara {
namespace adsfi {
struct PerceptionStaticEnv {
    ::ara::adsfi::CommonHeader header;
    double send_stamp{ 0.0 };
    double start_stamp{ 0.0 };
    float cell_size{ 0.0F };
    ::ara::adsfi::PerceptionSemanticObjVec semantic_objs;
    float terrain_patch_size{ 0.0F };
    ::ara::adsfi::PerceptionTerrainPatchVec terrain_patches;
    ::Int16 map_width{ 0 };
    ::Int16 map_height{ 0 };
    ::Int16 vehicle_posx{ 0 };
    ::Int16 vehicle_posy{ 0 };
    ::ara::adsfi::IdpUint8Vec reserved;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(send_stamp);
        fun(start_stamp);
        fun(cell_size);
        fun(semantic_objs);
        fun(terrain_patch_size);
        fun(terrain_patches);
        fun(map_width);
        fun(map_height);
        fun(vehicle_posx);
        fun(vehicle_posy);
        fun(reserved);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(send_stamp);
        fun(start_stamp);
        fun(cell_size);
        fun(semantic_objs);
        fun(terrain_patch_size);
        fun(terrain_patches);
        fun(map_width);
        fun(map_height);
        fun(vehicle_posx);
        fun(vehicle_posy);
        fun(reserved);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("send_stamp", send_stamp);
        fun("start_stamp", start_stamp);
        fun("cell_size", cell_size);
        fun("semantic_objs", semantic_objs);
        fun("terrain_patch_size", terrain_patch_size);
        fun("terrain_patches", terrain_patches);
        fun("map_width", map_width);
        fun("map_height", map_height);
        fun("vehicle_posx", vehicle_posx);
        fun("vehicle_posy", vehicle_posy);
        fun("reserved", reserved);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("send_stamp", send_stamp);
        fun("start_stamp", start_stamp);
        fun("cell_size", cell_size);
        fun("semantic_objs", semantic_objs);
        fun("terrain_patch_size", terrain_patch_size);
        fun("terrain_patches", terrain_patches);
        fun("map_width", map_width);
        fun("map_height", map_height);
        fun("vehicle_posx", vehicle_posx);
        fun("vehicle_posy", vehicle_posy);
        fun("reserved", reserved);
    }

    friend bool operator==(const ::ara::adsfi::PerceptionStaticEnv& lhs, const ::ara::adsfi::PerceptionStaticEnv& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.send_stamp - rhs.send_stamp)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.start_stamp - rhs.start_stamp)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.cell_size - rhs.cell_size)) < DBL_EPSILON) && (lhs.semantic_objs == rhs.semantic_objs) && (fabs(static_cast<double>(lhs.terrain_patch_size - rhs.terrain_patch_size)) < DBL_EPSILON) && (lhs.terrain_patches == rhs.terrain_patches) && (lhs.map_width == rhs.map_width) && (lhs.map_height == rhs.map_height) && (lhs.vehicle_posx == rhs.vehicle_posx) && (lhs.vehicle_posy == rhs.vehicle_posy) && (lhs.reserved == rhs.reserved);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PERCEPTIONSTATICENV_H
