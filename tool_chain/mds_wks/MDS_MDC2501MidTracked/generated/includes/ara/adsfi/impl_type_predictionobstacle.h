/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PREDICTIONOBSTACLE_H
#define ARA_ADSFI_IMPL_TYPE_PREDICTIONOBSTACLE_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_perceptionobstacle.h"
#include "ara/adsfi/impl_type_time.h"
#include "impl_type_double.h"
#include "ara/adsfi/impl_type_trajectoryinpredictionvector.h"
#include "impl_type_uint8_t.h"
#include "ara/adsfi/impl_type_obstaclefeature.h"

namespace ara {
namespace adsfi {
struct PredictionObstacle {
    ::ara::adsfi::PerceptionObstacle object;
    ::ara::adsfi::Time gpsTime;
    ::Double predictedPeriod{ 0.0 };
    ::ara::adsfi::TrajectoryInPredictionVector trajectory;
    ::uint8_t intent{ 0U };
    ::uint8_t priority{ 0U };
    bool isStatic{ false };
    ::ara::adsfi::ObstacleFeature feature;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(object);
        fun(gpsTime);
        fun(predictedPeriod);
        fun(trajectory);
        fun(intent);
        fun(priority);
        fun(isStatic);
        fun(feature);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(object);
        fun(gpsTime);
        fun(predictedPeriod);
        fun(trajectory);
        fun(intent);
        fun(priority);
        fun(isStatic);
        fun(feature);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("object", object);
        fun("gpsTime", gpsTime);
        fun("predictedPeriod", predictedPeriod);
        fun("trajectory", trajectory);
        fun("intent", intent);
        fun("priority", priority);
        fun("isStatic", isStatic);
        fun("feature", feature);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("object", object);
        fun("gpsTime", gpsTime);
        fun("predictedPeriod", predictedPeriod);
        fun("trajectory", trajectory);
        fun("intent", intent);
        fun("priority", priority);
        fun("isStatic", isStatic);
        fun("feature", feature);
    }

    friend bool operator==(const ::ara::adsfi::PredictionObstacle& lhs, const ::ara::adsfi::PredictionObstacle& rhs) noexcept
    {
        return (lhs.object == rhs.object) && (lhs.gpsTime == rhs.gpsTime) && (fabs(static_cast<double>(lhs.predictedPeriod - rhs.predictedPeriod)) < DBL_EPSILON) && (lhs.trajectory == rhs.trajectory) && (lhs.intent == rhs.intent) && (lhs.priority == rhs.priority) && (lhs.isStatic == rhs.isStatic) && (lhs.feature == rhs.feature);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PREDICTIONOBSTACLE_H
