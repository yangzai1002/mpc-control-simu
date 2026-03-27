/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PERCEPTIONOBSTACLE_H
#define ARA_ADSFI_IMPL_TYPE_PERCEPTIONOBSTACLE_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint32_t.h"
#include "impl_type_point.h"
#include "impl_type_double.h"
#include "impl_type_pointarray.h"
#include "impl_type_uint8_t.h"
#include "ara/adsfi/impl_type_time.h"
#include "impl_type_doublearray9.h"

namespace ara {
namespace adsfi {
struct PerceptionObstacle {
    ::uint32_t id{ 0U };
    ::Point position;
    ::Double theta{ 0.0 };
    ::Point velocity;
    bool hasVelocity{ false };
    ::Point size;
    ::PointArray polygonPoint;
    ::Double trackingTime{ 0.0 };
    ::uint8_t type{ 0U };
    ::Double confidence{ 0.0 };
    ::ara::adsfi::Time gpsTime;
    ::uint8_t confidenceType{ 0U };
    ::PointArray drops;
    ::Point acceleration;
    ::Point anchorPoint;
    ::PointArray boundingBox;
    ::uint8_t subType{ 0U };
    ::Double heightAboveGround{ 0.0 };
    ::DoubleArray9 positionCovariance{};
    ::DoubleArray9 velocityCovariance{};
    ::DoubleArray9 accelerationCovariance{};
    ::uint8_t lightStatus{ 0U };

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
        fun(type);
        fun(confidence);
        fun(gpsTime);
        fun(confidenceType);
        fun(drops);
        fun(acceleration);
        fun(anchorPoint);
        fun(boundingBox);
        fun(subType);
        fun(heightAboveGround);
        fun(positionCovariance);
        fun(velocityCovariance);
        fun(accelerationCovariance);
        fun(lightStatus);
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
        fun(type);
        fun(confidence);
        fun(gpsTime);
        fun(confidenceType);
        fun(drops);
        fun(acceleration);
        fun(anchorPoint);
        fun(boundingBox);
        fun(subType);
        fun(heightAboveGround);
        fun(positionCovariance);
        fun(velocityCovariance);
        fun(accelerationCovariance);
        fun(lightStatus);
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
        fun("type", type);
        fun("confidence", confidence);
        fun("gpsTime", gpsTime);
        fun("confidenceType", confidenceType);
        fun("drops", drops);
        fun("acceleration", acceleration);
        fun("anchorPoint", anchorPoint);
        fun("boundingBox", boundingBox);
        fun("subType", subType);
        fun("heightAboveGround", heightAboveGround);
        fun("positionCovariance", positionCovariance);
        fun("velocityCovariance", velocityCovariance);
        fun("accelerationCovariance", accelerationCovariance);
        fun("lightStatus", lightStatus);
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
        fun("type", type);
        fun("confidence", confidence);
        fun("gpsTime", gpsTime);
        fun("confidenceType", confidenceType);
        fun("drops", drops);
        fun("acceleration", acceleration);
        fun("anchorPoint", anchorPoint);
        fun("boundingBox", boundingBox);
        fun("subType", subType);
        fun("heightAboveGround", heightAboveGround);
        fun("positionCovariance", positionCovariance);
        fun("velocityCovariance", velocityCovariance);
        fun("accelerationCovariance", accelerationCovariance);
        fun("lightStatus", lightStatus);
    }

    friend bool operator==(const ::ara::adsfi::PerceptionObstacle& lhs, const ::ara::adsfi::PerceptionObstacle& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.position == rhs.position) && (fabs(static_cast<double>(lhs.theta - rhs.theta)) < DBL_EPSILON) && (lhs.velocity == rhs.velocity) && (lhs.hasVelocity == rhs.hasVelocity) && (lhs.size == rhs.size) && (lhs.polygonPoint == rhs.polygonPoint) && (fabs(static_cast<double>(lhs.trackingTime - rhs.trackingTime)) < DBL_EPSILON) && (lhs.type == rhs.type) && (fabs(static_cast<double>(lhs.confidence - rhs.confidence)) < DBL_EPSILON) && (lhs.gpsTime == rhs.gpsTime) && (lhs.confidenceType == rhs.confidenceType) && (lhs.drops == rhs.drops) && (lhs.acceleration == rhs.acceleration) && (lhs.anchorPoint == rhs.anchorPoint) && (lhs.boundingBox == rhs.boundingBox) && (lhs.subType == rhs.subType) && (fabs(static_cast<double>(lhs.heightAboveGround - rhs.heightAboveGround)) < DBL_EPSILON) && (lhs.positionCovariance == rhs.positionCovariance) && (lhs.velocityCovariance == rhs.velocityCovariance) && (lhs.accelerationCovariance == rhs.accelerationCovariance) && (lhs.lightStatus == rhs.lightStatus);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PERCEPTIONOBSTACLE_H
