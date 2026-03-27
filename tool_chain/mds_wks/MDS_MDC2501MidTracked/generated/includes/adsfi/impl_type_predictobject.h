/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_PREDICTOBJECT_H
#define ADSFI_IMPL_TYPE_PREDICTOBJECT_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint32.h"
#include "impl_type_point.h"
#include "impl_type_double.h"
#include "impl_type_boolean.h"
#include "impl_type_pointarray.h"
#include "impl_type_uint8.h"
#include "ara/common/impl_type_commontime.h"
#include "impl_type_matrix3d.h"
#include "adsfi/impl_type_trajinpredictionvector.h"

namespace adsfi {
struct PredictObject {
    ::UInt32 id{ 0U };
    ::Point position;
    ::Double theta{ 0.0 };
    ::Point velocity;
    ::Boolean hasVelocity{ false };
    ::Point size;
    ::PointArray polygonPoint;
    ::Double trackingTime{ 0.0 };
    ::UInt8 cls{ 0U };
    ::Double confidence{ 0.0 };
    ::ara::common::CommonTime gpsTime;
    ::Point acceleration;
    ::Point anchorPoint;
    ::PointArray boundingBox;
    ::Double heightAboveGround{ 0.0 };
    ::Matrix3d positionCovariance{};
    ::Matrix3d velocityCovariance{};
    ::Matrix3d accelerationCovariance{};
    ::UInt8 lightStatus{ 0U };
    ::Double predictedPeriod{ 0.0 };
    ::adsfi::TrajInPredictionVector trajectory;
    ::UInt8 intent{ 0U };
    ::UInt8 priority{ 0U };
    ::Boolean isStatic{ false };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(position);
        fun(theta);
        fun(velocity);
        fun(hasVelocity);
        fun(size);
        fun(polygonPoint);
        fun(trackingTime);
        fun(cls);
        fun(confidence);
        fun(gpsTime);
        fun(acceleration);
        fun(anchorPoint);
        fun(boundingBox);
        fun(heightAboveGround);
        fun(positionCovariance);
        fun(velocityCovariance);
        fun(accelerationCovariance);
        fun(lightStatus);
        fun(predictedPeriod);
        fun(trajectory);
        fun(intent);
        fun(priority);
        fun(isStatic);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(position);
        fun(theta);
        fun(velocity);
        fun(hasVelocity);
        fun(size);
        fun(polygonPoint);
        fun(trackingTime);
        fun(cls);
        fun(confidence);
        fun(gpsTime);
        fun(acceleration);
        fun(anchorPoint);
        fun(boundingBox);
        fun(heightAboveGround);
        fun(positionCovariance);
        fun(velocityCovariance);
        fun(accelerationCovariance);
        fun(lightStatus);
        fun(predictedPeriod);
        fun(trajectory);
        fun(intent);
        fun(priority);
        fun(isStatic);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("position", position);
        fun("theta", theta);
        fun("velocity", velocity);
        fun("hasVelocity", hasVelocity);
        fun("size", size);
        fun("polygonPoint", polygonPoint);
        fun("trackingTime", trackingTime);
        fun("cls", cls);
        fun("confidence", confidence);
        fun("gpsTime", gpsTime);
        fun("acceleration", acceleration);
        fun("anchorPoint", anchorPoint);
        fun("boundingBox", boundingBox);
        fun("heightAboveGround", heightAboveGround);
        fun("positionCovariance", positionCovariance);
        fun("velocityCovariance", velocityCovariance);
        fun("accelerationCovariance", accelerationCovariance);
        fun("lightStatus", lightStatus);
        fun("predictedPeriod", predictedPeriod);
        fun("trajectory", trajectory);
        fun("intent", intent);
        fun("priority", priority);
        fun("isStatic", isStatic);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("position", position);
        fun("theta", theta);
        fun("velocity", velocity);
        fun("hasVelocity", hasVelocity);
        fun("size", size);
        fun("polygonPoint", polygonPoint);
        fun("trackingTime", trackingTime);
        fun("cls", cls);
        fun("confidence", confidence);
        fun("gpsTime", gpsTime);
        fun("acceleration", acceleration);
        fun("anchorPoint", anchorPoint);
        fun("boundingBox", boundingBox);
        fun("heightAboveGround", heightAboveGround);
        fun("positionCovariance", positionCovariance);
        fun("velocityCovariance", velocityCovariance);
        fun("accelerationCovariance", accelerationCovariance);
        fun("lightStatus", lightStatus);
        fun("predictedPeriod", predictedPeriod);
        fun("trajectory", trajectory);
        fun("intent", intent);
        fun("priority", priority);
        fun("isStatic", isStatic);
    }

    friend bool operator==(const ::adsfi::PredictObject& lhs, const ::adsfi::PredictObject& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.position == rhs.position) && (fabs(static_cast<double>(lhs.theta - rhs.theta)) < DBL_EPSILON) && (lhs.velocity == rhs.velocity) && (lhs.hasVelocity == rhs.hasVelocity) && (lhs.size == rhs.size) && (lhs.polygonPoint == rhs.polygonPoint) && (fabs(static_cast<double>(lhs.trackingTime - rhs.trackingTime)) < DBL_EPSILON) && (lhs.cls == rhs.cls) && (fabs(static_cast<double>(lhs.confidence - rhs.confidence)) < DBL_EPSILON) && (lhs.gpsTime == rhs.gpsTime) && (lhs.acceleration == rhs.acceleration) && (lhs.anchorPoint == rhs.anchorPoint) && (lhs.boundingBox == rhs.boundingBox) && (fabs(static_cast<double>(lhs.heightAboveGround - rhs.heightAboveGround)) < DBL_EPSILON) && (lhs.positionCovariance == rhs.positionCovariance) && (lhs.velocityCovariance == rhs.velocityCovariance) && (lhs.accelerationCovariance == rhs.accelerationCovariance) && (lhs.lightStatus == rhs.lightStatus) && (fabs(static_cast<double>(lhs.predictedPeriod - rhs.predictedPeriod)) < DBL_EPSILON) && (lhs.trajectory == rhs.trajectory) && (lhs.intent == rhs.intent) && (lhs.priority == rhs.priority) && (lhs.isStatic == rhs.isStatic);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_PREDICTOBJECT_H
