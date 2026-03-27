/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PERCEPTIONSEMANTICMAP_H
#define ARA_ADSFI_IMPL_TYPE_PERCEPTIONSEMANTICMAP_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_commonheader.h"
#include "ara/adsfi/impl_type_perceptionutradatavec.h"
#include "ara/adsfi/impl_type_perceptionsemanticobjvec.h"
#include "ara/adsfi/impl_type_perceptionterrainpatchvec.h"
#include "ara/adsfi/impl_type_idpuint8vec.h"
#include "impl_type_int16.h"

namespace ara {
namespace adsfi {
struct PerceptionSemanticMap {
    ::ara::adsfi::CommonHeader header;
    double trigger_stamp{ 0.0 };
    float cell_size{ 0.0F };
    ::ara::adsfi::PerceptionUtraDataVec ultra_results;
    ::ara::adsfi::PerceptionSemanticObjVec semantic_objs;
    ::ara::adsfi::PerceptionTerrainPatchVec terrain_patches;
    ::ara::adsfi::IdpUint8Vec reserved;
    ::Int16 map_width{ 0 };
    ::Int16 map_height{ 0 };
    ::Int16 vehicle_posx{ 0 };
    ::Int16 vehicle_posy{ 0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(trigger_stamp);
        fun(cell_size);
        fun(ultra_results);
        fun(semantic_objs);
        fun(terrain_patches);
        fun(reserved);
        fun(map_width);
        fun(map_height);
        fun(vehicle_posx);
        fun(vehicle_posy);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(trigger_stamp);
        fun(cell_size);
        fun(ultra_results);
        fun(semantic_objs);
        fun(terrain_patches);
        fun(reserved);
        fun(map_width);
        fun(map_height);
        fun(vehicle_posx);
        fun(vehicle_posy);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("trigger_stamp", trigger_stamp);
        fun("cell_size", cell_size);
        fun("ultra_results", ultra_results);
        fun("semantic_objs", semantic_objs);
        fun("terrain_patches", terrain_patches);
        fun("reserved", reserved);
        fun("map_width", map_width);
        fun("map_height", map_height);
        fun("vehicle_posx", vehicle_posx);
        fun("vehicle_posy", vehicle_posy);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("trigger_stamp", trigger_stamp);
        fun("cell_size", cell_size);
        fun("ultra_results", ultra_results);
        fun("semantic_objs", semantic_objs);
        fun("terrain_patches", terrain_patches);
        fun("reserved", reserved);
        fun("map_width", map_width);
        fun("map_height", map_height);
        fun("vehicle_posx", vehicle_posx);
        fun("vehicle_posy", vehicle_posy);
    }

    friend bool operator==(const ::ara::adsfi::PerceptionSemanticMap& lhs, const ::ara::adsfi::PerceptionSemanticMap& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.trigger_stamp - rhs.trigger_stamp)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.cell_size - rhs.cell_size)) < DBL_EPSILON) && (lhs.ultra_results == rhs.ultra_results) && (lhs.semantic_objs == rhs.semantic_objs) && (lhs.terrain_patches == rhs.terrain_patches) && (lhs.reserved == rhs.reserved) && (lhs.map_width == rhs.map_width) && (lhs.map_height == rhs.map_height) && (lhs.vehicle_posx == rhs.vehicle_posx) && (lhs.vehicle_posy == rhs.vehicle_posy);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PERCEPTIONSEMANTICMAP_H
