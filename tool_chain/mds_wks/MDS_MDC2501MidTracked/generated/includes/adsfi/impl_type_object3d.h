/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_OBJECT3D_H
#define ADSFI_IMPL_TYPE_OBJECT3D_H

#include <cfloat>
#include <cmath>
#include "ara/common/impl_type_commonheader.h"
#include "impl_type_double.h"
#include "impl_type_uint32.h"
#include "ara/common/impl_type_commontime.h"
#include "impl_type_uint8.h"
#include "impl_type_string.h"
#include "impl_type_point.h"
#include "impl_type_pointarray.h"
#include "impl_type_rect3d.h"

namespace adsfi {
struct Object3d {
    ::ara::common::CommonHeader header;
    ::Double existProb{ 0.0 };
    ::UInt32 id{ 0U };
    ::ara::common::CommonTime timeCreation;
    ::UInt8 cls{ 0U };
    ::String clsDescription;
    ::Double clsConfidence{ 0.0 };
    ::Point velocity;
    ::Point absVelocity;
    ::PointArray pointsInObject;
    ::UInt8 cipvFlag{ 0U };
    ::Point acceleration;
    ::UInt8 cameraStatus{ 0U };
    ::String coordinate;
    ::Rect3d rect3d;
    ::UInt8 blinkerStatus{ 0U };

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
        fun(pointsInObject);
        fun(cipvFlag);
        fun(acceleration);
        fun(cameraStatus);
        fun(coordinate);
        fun(rect3d);
        fun(blinkerStatus);
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
        fun(pointsInObject);
        fun(cipvFlag);
        fun(acceleration);
        fun(cameraStatus);
        fun(coordinate);
        fun(rect3d);
        fun(blinkerStatus);
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
        fun("pointsInObject", pointsInObject);
        fun("cipvFlag", cipvFlag);
        fun("acceleration", acceleration);
        fun("cameraStatus", cameraStatus);
        fun("coordinate", coordinate);
        fun("rect3d", rect3d);
        fun("blinkerStatus", blinkerStatus);
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
        fun("pointsInObject", pointsInObject);
        fun("cipvFlag", cipvFlag);
        fun("acceleration", acceleration);
        fun("cameraStatus", cameraStatus);
        fun("coordinate", coordinate);
        fun("rect3d", rect3d);
        fun("blinkerStatus", blinkerStatus);
    }

    friend bool operator==(const ::adsfi::Object3d& lhs, const ::adsfi::Object3d& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.existProb - rhs.existProb)) < DBL_EPSILON) && (lhs.id == rhs.id) && (lhs.timeCreation == rhs.timeCreation) && (lhs.cls == rhs.cls) && (lhs.clsDescription == rhs.clsDescription) && (fabs(static_cast<double>(lhs.clsConfidence - rhs.clsConfidence)) < DBL_EPSILON) && (lhs.velocity == rhs.velocity) && (lhs.absVelocity == rhs.absVelocity) && (lhs.pointsInObject == rhs.pointsInObject) && (lhs.cipvFlag == rhs.cipvFlag) && (lhs.acceleration == rhs.acceleration) && (lhs.cameraStatus == rhs.cameraStatus) && (lhs.coordinate == rhs.coordinate) && (lhs.rect3d == rhs.rect3d) && (lhs.blinkerStatus == rhs.blinkerStatus);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_OBJECT3D_H
