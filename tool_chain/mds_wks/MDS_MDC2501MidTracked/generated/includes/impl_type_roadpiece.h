/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_ROADPIECE_H
#define IMPL_TYPE_ROADPIECE_H

#include "impl_type_laneseriesvector.h"
#include "impl_type_string.h"
#include "impl_type_uint32.h"

struct RoadPiece {
    ::LaneSeriesVector roadPiece;
    ::String id;
    ::UInt32 roadType{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(roadPiece);
        fun(id);
        fun(roadType);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(roadPiece);
        fun(id);
        fun(roadType);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("roadPiece", roadPiece);
        fun("id", id);
        fun("roadType", roadType);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("roadPiece", roadPiece);
        fun("id", id);
        fun("roadType", roadType);
    }

    friend bool operator==(const ::RoadPiece& lhs, const ::RoadPiece& rhs) noexcept
    {
        return (lhs.roadPiece == rhs.roadPiece) && (lhs.id == rhs.id) && (lhs.roadType == rhs.roadType);
    }
};


#endif // IMPL_TYPE_ROADPIECE_H
