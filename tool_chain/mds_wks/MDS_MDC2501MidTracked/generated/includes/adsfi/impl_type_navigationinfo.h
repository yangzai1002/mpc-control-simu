/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_NAVIGATIONINFO_H
#define ADSFI_IMPL_TYPE_NAVIGATIONINFO_H

#include "ara/common/impl_type_commonheader.h"
#include "impl_type_roadpiecevector.h"
#include "impl_type_naviroadpointvector.h"
#include "impl_type_string.h"
#include "impl_type_boolean.h"
#include "impl_type_stringvector.h"
#include "impl_type_lanepiecevector.h"

namespace adsfi {
struct NavigationInfo {
    ::ara::common::CommonHeader header;
    ::RoadPieceVector roadPieces;
    ::NaviRoadPointVector selectPoints;
    ::String mapVersion;
    ::Boolean isReRouted{ false };
    ::StringVector blacklistRoads;
    ::LanePieceVector blacklistLanePieces;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(roadPieces);
        fun(selectPoints);
        fun(mapVersion);
        fun(isReRouted);
        fun(blacklistRoads);
        fun(blacklistLanePieces);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(roadPieces);
        fun(selectPoints);
        fun(mapVersion);
        fun(isReRouted);
        fun(blacklistRoads);
        fun(blacklistLanePieces);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("roadPieces", roadPieces);
        fun("selectPoints", selectPoints);
        fun("mapVersion", mapVersion);
        fun("isReRouted", isReRouted);
        fun("blacklistRoads", blacklistRoads);
        fun("blacklistLanePieces", blacklistLanePieces);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("roadPieces", roadPieces);
        fun("selectPoints", selectPoints);
        fun("mapVersion", mapVersion);
        fun("isReRouted", isReRouted);
        fun("blacklistRoads", blacklistRoads);
        fun("blacklistLanePieces", blacklistLanePieces);
    }

    friend bool operator==(const ::adsfi::NavigationInfo& lhs, const ::adsfi::NavigationInfo& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.roadPieces == rhs.roadPieces) && (lhs.selectPoints == rhs.selectPoints) && (lhs.mapVersion == rhs.mapVersion) && (lhs.isReRouted == rhs.isReRouted) && (lhs.blacklistRoads == rhs.blacklistRoads) && (lhs.blacklistLanePieces == rhs.blacklistLanePieces);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_NAVIGATIONINFO_H
