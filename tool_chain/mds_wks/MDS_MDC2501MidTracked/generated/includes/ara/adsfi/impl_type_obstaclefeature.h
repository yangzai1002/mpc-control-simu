/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_OBSTACLEFEATURE_H
#define ARA_ADSFI_IMPL_TYPE_OBSTACLEFEATURE_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint32_t.h"
#include "impl_type_pointarray.h"
#include "impl_type_point.h"
#include "impl_type_double.h"
#include "ara/adsfi/impl_type_time.h"
#include "impl_type_uint8_t.h"
#include "ara/adsfi/impl_type_predictiontrajectorypointvector.h"
#include "ara/adsfi/impl_type_trajectorypointvector.h"
#include "ara/adsfi/impl_type_trajectoryinpredictionvector.h"

namespace ara {
namespace adsfi {
struct ObstacleFeature {
    ::uint32_t id{ 0U };
    ::PointArray polygonPoint;
    ::Point position;
    ::Point frontPosition;
    ::Point velocity;
    ::Point rawVelocity;
    ::Point acceleration;
    ::Double velocityHeading{ 0.0 };
    ::Double speed{ 0.0 };
    ::Double acc{ 0.0 };
    ::Double theta{ 0.0 };
    ::Double length{ 0.0 };
    ::Double width{ 0.0 };
    ::Double height{ 0.0 };
    ::Double trackingTime{ 0.0 };
    ::ara::adsfi::Time timestamp;
    ::Point tPosition;
    ::Point tVelocity;
    ::Double tVelocityHeading{ 0.0 };
    ::Double tSpeed{ 0.0 };
    ::Point tAcceleration;
    ::Double tAcc{ 0.0 };
    bool isStill{ false };
    ::uint8_t type{ 0U };
    ::Double labelUpdateTimeDelta{ 0.0 };
    ::uint8_t priority{ 0U };
    bool isNearJunction{ false };
    ::ara::adsfi::PredictionTrajectoryPointVector futureTrajectoryPoints;
    ::ara::adsfi::TrajectoryPointVector shortTermPredictedTrajectoryPoints;
    ::ara::adsfi::TrajectoryInPredictionVector predictedTrajectory;
    ::ara::adsfi::TrajectoryPointVector adcTrajectoryPoint;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(polygonPoint);
        fun(position);
        fun(frontPosition);
        fun(velocity);
        fun(rawVelocity);
        fun(acceleration);
        fun(velocityHeading);
        fun(speed);
        fun(acc);
        fun(theta);
        fun(length);
        fun(width);
        fun(height);
        fun(trackingTime);
        fun(timestamp);
        fun(tPosition);
        fun(tVelocity);
        fun(tVelocityHeading);
        fun(tSpeed);
        fun(tAcceleration);
        fun(tAcc);
        fun(isStill);
        fun(type);
        fun(labelUpdateTimeDelta);
        fun(priority);
        fun(isNearJunction);
        fun(futureTrajectoryPoints);
        fun(shortTermPredictedTrajectoryPoints);
        fun(predictedTrajectory);
        fun(adcTrajectoryPoint);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(polygonPoint);
        fun(position);
        fun(frontPosition);
        fun(velocity);
        fun(rawVelocity);
        fun(acceleration);
        fun(velocityHeading);
        fun(speed);
        fun(acc);
        fun(theta);
        fun(length);
        fun(width);
        fun(height);
        fun(trackingTime);
        fun(timestamp);
        fun(tPosition);
        fun(tVelocity);
        fun(tVelocityHeading);
        fun(tSpeed);
        fun(tAcceleration);
        fun(tAcc);
        fun(isStill);
        fun(type);
        fun(labelUpdateTimeDelta);
        fun(priority);
        fun(isNearJunction);
        fun(futureTrajectoryPoints);
        fun(shortTermPredictedTrajectoryPoints);
        fun(predictedTrajectory);
        fun(adcTrajectoryPoint);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("polygonPoint", polygonPoint);
        fun("position", position);
        fun("frontPosition", frontPosition);
        fun("velocity", velocity);
        fun("rawVelocity", rawVelocity);
        fun("acceleration", acceleration);
        fun("velocityHeading", velocityHeading);
        fun("speed", speed);
        fun("acc", acc);
        fun("theta", theta);
        fun("length", length);
        fun("width", width);
        fun("height", height);
        fun("trackingTime", trackingTime);
        fun("timestamp", timestamp);
        fun("tPosition", tPosition);
        fun("tVelocity", tVelocity);
        fun("tVelocityHeading", tVelocityHeading);
        fun("tSpeed", tSpeed);
        fun("tAcceleration", tAcceleration);
        fun("tAcc", tAcc);
        fun("isStill", isStill);
        fun("type", type);
        fun("labelUpdateTimeDelta", labelUpdateTimeDelta);
        fun("priority", priority);
        fun("isNearJunction", isNearJunction);
        fun("futureTrajectoryPoints", futureTrajectoryPoints);
        fun("shortTermPredictedTrajectoryPoints", shortTermPredictedTrajectoryPoints);
        fun("predictedTrajectory", predictedTrajectory);
        fun("adcTrajectoryPoint", adcTrajectoryPoint);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("polygonPoint", polygonPoint);
        fun("position", position);
        fun("frontPosition", frontPosition);
        fun("velocity", velocity);
        fun("rawVelocity", rawVelocity);
        fun("acceleration", acceleration);
        fun("velocityHeading", velocityHeading);
        fun("speed", speed);
        fun("acc", acc);
        fun("theta", theta);
        fun("length", length);
        fun("width", width);
        fun("height", height);
        fun("trackingTime", trackingTime);
        fun("timestamp", timestamp);
        fun("tPosition", tPosition);
        fun("tVelocity", tVelocity);
        fun("tVelocityHeading", tVelocityHeading);
        fun("tSpeed", tSpeed);
        fun("tAcceleration", tAcceleration);
        fun("tAcc", tAcc);
        fun("isStill", isStill);
        fun("type", type);
        fun("labelUpdateTimeDelta", labelUpdateTimeDelta);
        fun("priority", priority);
        fun("isNearJunction", isNearJunction);
        fun("futureTrajectoryPoints", futureTrajectoryPoints);
        fun("shortTermPredictedTrajectoryPoints", shortTermPredictedTrajectoryPoints);
        fun("predictedTrajectory", predictedTrajectory);
        fun("adcTrajectoryPoint", adcTrajectoryPoint);
    }

    friend bool operator==(const ::ara::adsfi::ObstacleFeature& lhs, const ::ara::adsfi::ObstacleFeature& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.polygonPoint == rhs.polygonPoint) && (lhs.position == rhs.position) && (lhs.frontPosition == rhs.frontPosition) && (lhs.velocity == rhs.velocity) && (lhs.rawVelocity == rhs.rawVelocity) && (lhs.acceleration == rhs.acceleration) && (fabs(static_cast<double>(lhs.velocityHeading - rhs.velocityHeading)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.speed - rhs.speed)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.acc - rhs.acc)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.theta - rhs.theta)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.length - rhs.length)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.width - rhs.width)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.height - rhs.height)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.trackingTime - rhs.trackingTime)) < DBL_EPSILON) && (lhs.timestamp == rhs.timestamp) && (lhs.tPosition == rhs.tPosition) && (lhs.tVelocity == rhs.tVelocity) && (fabs(static_cast<double>(lhs.tVelocityHeading - rhs.tVelocityHeading)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.tSpeed - rhs.tSpeed)) < DBL_EPSILON) && (lhs.tAcceleration == rhs.tAcceleration) && (fabs(static_cast<double>(lhs.tAcc - rhs.tAcc)) < DBL_EPSILON) && (lhs.isStill == rhs.isStill) && (lhs.type == rhs.type) && (fabs(static_cast<double>(lhs.labelUpdateTimeDelta - rhs.labelUpdateTimeDelta)) < DBL_EPSILON) && (lhs.priority == rhs.priority) && (lhs.isNearJunction == rhs.isNearJunction) && (lhs.futureTrajectoryPoints == rhs.futureTrajectoryPoints) && (lhs.shortTermPredictedTrajectoryPoints == rhs.shortTermPredictedTrajectoryPoints) && (lhs.predictedTrajectory == rhs.predictedTrajectory) && (lhs.adcTrajectoryPoint == rhs.adcTrajectoryPoint);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_OBSTACLEFEATURE_H
