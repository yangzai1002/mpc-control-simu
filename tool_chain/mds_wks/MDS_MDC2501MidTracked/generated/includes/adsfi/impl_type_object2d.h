/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_OBJECT2D_H
#define ADSFI_IMPL_TYPE_OBJECT2D_H

#include <cfloat>
#include <cmath>
#include "ara/common/impl_type_commonheader.h"
#include "impl_type_double.h"
#include "impl_type_uint32.h"
#include "ara/common/impl_type_commontime.h"
#include "impl_type_uint8.h"
#include "impl_type_string.h"
#include "impl_type_pointxydouble.h"
#include "impl_type_rect2d.h"

namespace adsfi {
struct Object2d {
    ::ara::common::CommonHeader header;
    ::Double existProb{ 0.0 };
    ::UInt32 id{ 0U };
    ::ara::common::CommonTime timeCreation;
    ::UInt8 cls{ 0U };
    ::String clsDescription;
    ::Double clsConfidence{ 0.0 };
    ::PointxyDouble velocity;
    ::PointxyDouble absVelocity;
    ::UInt8 cipvFlag{ 0U };
    ::PointxyDouble acceleration;
    ::UInt8 cameraStatus{ 0U };
    ::String coordinate;
    ::UInt8 blinkerStatus{ 0U };
    ::Rect2d rect2d;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(existProb);
        fun(id);
        fun(timeCreation);
        fun(cls);
        fun(clsDescription);
        fun(clsConfidence);
        fun(velocity);
        fun(absVelocity);
        fun(cipvFlag);
        fun(acceleration);
        fun(cameraStatus);
        fun(coordinate);
        fun(blinkerStatus);
        fun(rect2d);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(existProb);
        fun(id);
        fun(timeCreation);
        fun(cls);
        fun(clsDescription);
        fun(clsConfidence);
        fun(velocity);
        fun(absVelocity);
        fun(cipvFlag);
        fun(acceleration);
        fun(cameraStatus);
        fun(coordinate);
        fun(blinkerStatus);
        fun(rect2d);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("existProb", existProb);
        fun("id", id);
        fun("timeCreation", timeCreation);
        fun("cls", cls);
        fun("clsDescription", clsDescription);
        fun("clsConfidence", clsConfidence);
        fun("velocity", velocity);
        fun("absVelocity", absVelocity);
        fun("cipvFlag", cipvFlag);
        fun("acceleration", acceleration);
        fun("cameraStatus", cameraStatus);
        fun("coordinate", coordinate);
        fun("blinkerStatus", blinkerStatus);
        fun("rect2d", rect2d);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("existProb", existProb);
        fun("id", id);
        fun("timeCreation", timeCreation);
        fun("cls", cls);
        fun("clsDescription", clsDescription);
        fun("clsConfidence", clsConfidence);
        fun("velocity", velocity);
        fun("absVelocity", absVelocity);
        fun("cipvFlag", cipvFlag);
        fun("acceleration", acceleration);
        fun("cameraStatus", cameraStatus);
        fun("coordinate", coordinate);
        fun("blinkerStatus", blinkerStatus);
        fun("rect2d", rect2d);
    }

    friend bool operator==(const ::adsfi::Object2d& lhs, const ::adsfi::Object2d& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.existProb - rhs.existProb)) < DBL_EPSILON) && (lhs.id == rhs.id) && (lhs.timeCreation == rhs.timeCreation) && (lhs.cls == rhs.cls) && (lhs.clsDescription == rhs.clsDescription) && (fabs(static_cast<double>(lhs.clsConfidence - rhs.clsConfidence)) < DBL_EPSILON) && (lhs.velocity == rhs.velocity) && (lhs.absVelocity == rhs.absVelocity) && (lhs.cipvFlag == rhs.cipvFlag) && (lhs.acceleration == rhs.acceleration) && (lhs.cameraStatus == rhs.cameraStatus) && (lhs.coordinate == rhs.coordinate) && (lhs.blinkerStatus == rhs.blinkerStatus) && (lhs.rect2d == rhs.rect2d);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_OBJECT2D_H
