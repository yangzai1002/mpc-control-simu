/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_NAVIGATION_IMPL_TYPE_NAVIGATIONRESULTSTRUCT_H
#define ARA_NAVIGATION_IMPL_TYPE_NAVIGATIONRESULTSTRUCT_H

#include "ara/navigation/impl_type_header.h"
#include "impl_type_roadpiecevector.h"
#include "impl_type_naviroadpointvector.h"
#include "impl_type_string.h"
#include "impl_type_stringvector.h"
#include "impl_type_lanepiecevector.h"

namespace ara {
namespace navigation {
struct NavigationResultStruct {
    ::ara::navigation::Header header;
    ::RoadPieceVector roadPieces;
    ::NaviRoadPointVector selectPoints;
    ::String mapVersion;
    bool isNewRouting{ false };
    ::StringVector blacklistRoads;
    ::LanePieceVector blacklistLaneSegments;

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
        fun(isNewRouting);
        fun(blacklistRoads);
        fun(blacklistLaneSegments);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(roadPieces);
        fun(selectPoints);
        fun(mapVersion);
        fun(isNewRouting);
        fun(blacklistRoads);
        fun(blacklistLaneSegments);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("roadPieces", roadPieces);
        fun("selectPoints", selectPoints);
        fun("mapVersion", mapVersion);
        fun("isNewRouting", isNewRouting);
        fun("blacklistRoads", blacklistRoads);
        fun("blacklistLaneSegments", blacklistLaneSegments);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("roadPieces", roadPieces);
        fun("selectPoints", selectPoints);
        fun("mapVersion", mapVersion);
        fun("isNewRouting", isNewRouting);
        fun("blacklistRoads", blacklistRoads);
        fun("blacklistLaneSegments", blacklistLaneSegments);
    }

    friend bool operator==(const ::ara::navigation::NavigationResultStruct& lhs, const ::ara::navigation::NavigationResultStruct& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.roadPieces == rhs.roadPieces) && (lhs.selectPoints == rhs.selectPoints) && (lhs.mapVersion == rhs.mapVersion) && (lhs.isNewRouting == rhs.isNewRouting) && (lhs.blacklistRoads == rhs.blacklistRoads) && (lhs.blacklistLaneSegments == rhs.blacklistLaneSegments);
    }
};
} // namespace navigation
} // namespace ara


#endif // ARA_NAVIGATION_IMPL_TYPE_NAVIGATIONRESULTSTRUCT_H
