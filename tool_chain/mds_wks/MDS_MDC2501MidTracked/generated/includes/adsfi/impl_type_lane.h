/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_LANE_H
#define ADSFI_IMPL_TYPE_LANE_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint32.h"
#include "impl_type_uint8.h"
#include "impl_type_string.h"
#include "impl_type_float.h"
#include "adsfi/impl_type_lanefitparam.h"
#include "impl_type_pointxy.h"
#include "impl_type_pointxyarray.h"
#include "impl_type_matrix3d.h"
#include "ara/common/impl_type_commontime.h"

namespace adsfi {
struct Lane {
    ::UInt32 id{ 0U };
    ::UInt8 cls{ 0U };
    ::String clsDescription;
    ::Float clsConfidence{ 0.0F };
    ::adsfi::LaneFitParam laneFit;
    ::Pointxy startPoint;
    ::Pointxy endPoint;
    ::PointxyArray keyPoints;
    ::Matrix3d homographyMat{};
    ::Matrix3d homographyMatInv{};
    ::ara::common::CommonTime timeCreation;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(cls);
        fun(clsDescription);
        fun(clsConfidence);
        fun(laneFit);
        fun(startPoint);
        fun(endPoint);
        fun(keyPoints);
        fun(homographyMat);
        fun(homographyMatInv);
        fun(timeCreation);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(cls);
        fun(clsDescription);
        fun(clsConfidence);
        fun(laneFit);
        fun(startPoint);
        fun(endPoint);
        fun(keyPoints);
        fun(homographyMat);
        fun(homographyMatInv);
        fun(timeCreation);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("cls", cls);
        fun("clsDescription", clsDescription);
        fun("clsConfidence", clsConfidence);
        fun("laneFit", laneFit);
        fun("startPoint", startPoint);
        fun("endPoint", endPoint);
        fun("keyPoints", keyPoints);
        fun("homographyMat", homographyMat);
        fun("homographyMatInv", homographyMatInv);
        fun("timeCreation", timeCreation);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("cls", cls);
        fun("clsDescription", clsDescription);
        fun("clsConfidence", clsConfidence);
        fun("laneFit", laneFit);
        fun("startPoint", startPoint);
        fun("endPoint", endPoint);
        fun("keyPoints", keyPoints);
        fun("homographyMat", homographyMat);
        fun("homographyMatInv", homographyMatInv);
        fun("timeCreation", timeCreation);
    }

    friend bool operator==(const ::adsfi::Lane& lhs, const ::adsfi::Lane& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.cls == rhs.cls) && (lhs.clsDescription == rhs.clsDescription) && (fabs(static_cast<double>(lhs.clsConfidence - rhs.clsConfidence)) < DBL_EPSILON) && (lhs.laneFit == rhs.laneFit) && (lhs.startPoint == rhs.startPoint) && (lhs.endPoint == rhs.endPoint) && (lhs.keyPoints == rhs.keyPoints) && (lhs.homographyMat == rhs.homographyMat) && (lhs.homographyMatInv == rhs.homographyMatInv) && (lhs.timeCreation == rhs.timeCreation);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_LANE_H
