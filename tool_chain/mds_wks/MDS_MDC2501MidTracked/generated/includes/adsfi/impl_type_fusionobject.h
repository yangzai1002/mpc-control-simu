/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_FUSIONOBJECT_H
#define ADSFI_IMPL_TYPE_FUSIONOBJECT_H

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
#include "impl_type_matrix3d.h"

namespace adsfi {
struct FusionObject {
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
    ::Rect3d absoluteRect3d;
    ::Matrix3d velocityCovariance{};
    ::Matrix3d absVelocityCovariance{};
    ::Point absAcceleration;
    ::Matrix3d accelerationCovariance{};
    ::Matrix3d absAccelerationCovariance{};
    ::PointArray pathPoints;
    ::ara::common::CommonTime lastUpdateTime;
    ::UInt8 blinkerFlag{ 0U };
    ::UInt8 fusionType{ 0U };
    ::Point anchorPoint;
    ::Point absAnchorPoint;

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
        fun(absoluteRect3d);
        fun(velocityCovariance);
        fun(absVelocityCovariance);
        fun(absAcceleration);
        fun(accelerationCovariance);
        fun(absAccelerationCovariance);
        fun(pathPoints);
        fun(lastUpdateTime);
        fun(blinkerFlag);
        fun(fusionType);
        fun(anchorPoint);
        fun(absAnchorPoint);
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
        fun(absoluteRect3d);
        fun(velocityCovariance);
        fun(absVelocityCovariance);
        fun(absAcceleration);
        fun(accelerationCovariance);
        fun(absAccelerationCovariance);
        fun(pathPoints);
        fun(lastUpdateTime);
        fun(blinkerFlag);
        fun(fusionType);
        fun(anchorPoint);
        fun(absAnchorPoint);
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
        fun("absoluteRect3d", absoluteRect3d);
        fun("velocityCovariance", velocityCovariance);
        fun("absVelocityCovariance", absVelocityCovariance);
        fun("absAcceleration", absAcceleration);
        fun("accelerationCovariance", accelerationCovariance);
        fun("absAccelerationCovariance", absAccelerationCovariance);
        fun("pathPoints", pathPoints);
        fun("lastUpdateTime", lastUpdateTime);
        fun("blinkerFlag", blinkerFlag);
        fun("fusionType", fusionType);
        fun("anchorPoint", anchorPoint);
        fun("absAnchorPoint", absAnchorPoint);
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
        fun("absoluteRect3d", absoluteRect3d);
        fun("velocityCovariance", velocityCovariance);
        fun("absVelocityCovariance", absVelocityCovariance);
        fun("absAcceleration", absAcceleration);
        fun("accelerationCovariance", accelerationCovariance);
        fun("absAccelerationCovariance", absAccelerationCovariance);
        fun("pathPoints", pathPoints);
        fun("lastUpdateTime", lastUpdateTime);
        fun("blinkerFlag", blinkerFlag);
        fun("fusionType", fusionType);
        fun("anchorPoint", anchorPoint);
        fun("absAnchorPoint", absAnchorPoint);
    }

    friend bool operator==(const ::adsfi::FusionObject& lhs, const ::adsfi::FusionObject& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.existProb - rhs.existProb)) < DBL_EPSILON) && (lhs.id == rhs.id) && (lhs.timeCreation == rhs.timeCreation) && (lhs.cls == rhs.cls) && (lhs.clsDescription == rhs.clsDescription) && (fabs(static_cast<double>(lhs.clsConfidence - rhs.clsConfidence)) < DBL_EPSILON) && (lhs.velocity == rhs.velocity) && (lhs.absVelocity == rhs.absVelocity) && (lhs.pointsInObject == rhs.pointsInObject) && (lhs.cipvFlag == rhs.cipvFlag) && (lhs.acceleration == rhs.acceleration) && (lhs.cameraStatus == rhs.cameraStatus) && (lhs.coordinate == rhs.coordinate) && (lhs.rect3d == rhs.rect3d) && (lhs.absoluteRect3d == rhs.absoluteRect3d) && (lhs.velocityCovariance == rhs.velocityCovariance) && (lhs.absVelocityCovariance == rhs.absVelocityCovariance) && (lhs.absAcceleration == rhs.absAcceleration) && (lhs.accelerationCovariance == rhs.accelerationCovariance) && (lhs.absAccelerationCovariance == rhs.absAccelerationCovariance) && (lhs.pathPoints == rhs.pathPoints) && (lhs.lastUpdateTime == rhs.lastUpdateTime) && (lhs.blinkerFlag == rhs.blinkerFlag) && (lhs.fusionType == rhs.fusionType) && (lhs.anchorPoint == rhs.anchorPoint) && (lhs.absAnchorPoint == rhs.absAnchorPoint);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_FUSIONOBJECT_H
