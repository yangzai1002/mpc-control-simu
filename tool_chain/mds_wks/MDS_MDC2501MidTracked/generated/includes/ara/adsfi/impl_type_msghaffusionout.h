/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFFUSIONOUT_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFFUSIONOUT_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32_t.h"
#include "impl_type_uint32_t.h"
#include "ara/adsfi/impl_type_msghafrect3d.h"
#include "ara/adsfi/impl_type_msghafrect2d.h"
#include "ara/adsfi/impl_type_msgpoint3f.h"
#include "ara/adsfi/impl_type_idpdoublevec.h"
#include "ara/adsfi/impl_type_msgpoint3d.h"
#include "ara/adsfi/impl_type_msgpoint3dvec.h"
#include "ara/adsfi/impl_type_msghaftime.h"
#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct MsgHafFusionOut {
    ::int32_t objectID{ 0 };
    ::uint32_t cs{ 0U };
    float confidence{ 0.0F };
    float existenceProbability{ 0.0F };
    ::ara::adsfi::MsgHafRect3D rect;
    ::ara::adsfi::MsgHafRect2D rect_2d;
    ::ara::adsfi::MsgHafRect3D absRect;
    ::ara::adsfi::MsgPoint3f velocity;
    ::ara::adsfi::MsgPoint3f absVelocity;
    ::ara::adsfi::IdpDoubleVec velocityCovariance;
    ::ara::adsfi::IdpDoubleVec absVelocityCovariance;
    ::ara::adsfi::MsgPoint3D acceleration;
    ::ara::adsfi::MsgPoint3D absAcceleration;
    ::ara::adsfi::IdpDoubleVec accelerationCovariance;
    ::ara::adsfi::IdpDoubleVec absAccelerationCovariance;
    ::ara::adsfi::MsgPoint3DVec pathPoints;
    ::ara::adsfi::MsgHafTime timeCreation;
    ::ara::adsfi::MsgHafTime lastUpdatedTime;
    ::uint8_t cipvFlag{ 0U };
    ::uint8_t blinkerFlag{ 0U };
    ::uint8_t fusionType{ 0U };
    ::ara::adsfi::MsgPoint3D anchorPoint;
    ::ara::adsfi::MsgPoint3D absAnchorPoint;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(objectID);
        fun(cs);
        fun(confidence);
        fun(existenceProbability);
        fun(rect);
        fun(rect_2d);
        fun(absRect);
        fun(velocity);
        fun(absVelocity);
        fun(velocityCovariance);
        fun(absVelocityCovariance);
        fun(acceleration);
        fun(absAcceleration);
        fun(accelerationCovariance);
        fun(absAccelerationCovariance);
        fun(pathPoints);
        fun(timeCreation);
        fun(lastUpdatedTime);
        fun(cipvFlag);
        fun(blinkerFlag);
        fun(fusionType);
        fun(anchorPoint);
        fun(absAnchorPoint);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(objectID);
        fun(cs);
        fun(confidence);
        fun(existenceProbability);
        fun(rect);
        fun(rect_2d);
        fun(absRect);
        fun(velocity);
        fun(absVelocity);
        fun(velocityCovariance);
        fun(absVelocityCovariance);
        fun(acceleration);
        fun(absAcceleration);
        fun(accelerationCovariance);
        fun(absAccelerationCovariance);
        fun(pathPoints);
        fun(timeCreation);
        fun(lastUpdatedTime);
        fun(cipvFlag);
        fun(blinkerFlag);
        fun(fusionType);
        fun(anchorPoint);
        fun(absAnchorPoint);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("objectID", objectID);
        fun("cs", cs);
        fun("confidence", confidence);
        fun("existenceProbability", existenceProbability);
        fun("rect", rect);
        fun("rect_2d", rect_2d);
        fun("absRect", absRect);
        fun("velocity", velocity);
        fun("absVelocity", absVelocity);
        fun("velocityCovariance", velocityCovariance);
        fun("absVelocityCovariance", absVelocityCovariance);
        fun("acceleration", acceleration);
        fun("absAcceleration", absAcceleration);
        fun("accelerationCovariance", accelerationCovariance);
        fun("absAccelerationCovariance", absAccelerationCovariance);
        fun("pathPoints", pathPoints);
        fun("timeCreation", timeCreation);
        fun("lastUpdatedTime", lastUpdatedTime);
        fun("cipvFlag", cipvFlag);
        fun("blinkerFlag", blinkerFlag);
        fun("fusionType", fusionType);
        fun("anchorPoint", anchorPoint);
        fun("absAnchorPoint", absAnchorPoint);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("objectID", objectID);
        fun("cs", cs);
        fun("confidence", confidence);
        fun("existenceProbability", existenceProbability);
        fun("rect", rect);
        fun("rect_2d", rect_2d);
        fun("absRect", absRect);
        fun("velocity", velocity);
        fun("absVelocity", absVelocity);
        fun("velocityCovariance", velocityCovariance);
        fun("absVelocityCovariance", absVelocityCovariance);
        fun("acceleration", acceleration);
        fun("absAcceleration", absAcceleration);
        fun("accelerationCovariance", accelerationCovariance);
        fun("absAccelerationCovariance", absAccelerationCovariance);
        fun("pathPoints", pathPoints);
        fun("timeCreation", timeCreation);
        fun("lastUpdatedTime", lastUpdatedTime);
        fun("cipvFlag", cipvFlag);
        fun("blinkerFlag", blinkerFlag);
        fun("fusionType", fusionType);
        fun("anchorPoint", anchorPoint);
        fun("absAnchorPoint", absAnchorPoint);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafFusionOut& lhs, const ::ara::adsfi::MsgHafFusionOut& rhs) noexcept
    {
        return (lhs.objectID == rhs.objectID) && (lhs.cs == rhs.cs) && (fabs(static_cast<double>(lhs.confidence - rhs.confidence)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.existenceProbability - rhs.existenceProbability)) < DBL_EPSILON) && (lhs.rect == rhs.rect) && (lhs.rect_2d == rhs.rect_2d) && (lhs.absRect == rhs.absRect) && (lhs.velocity == rhs.velocity) && (lhs.absVelocity == rhs.absVelocity) && (lhs.velocityCovariance == rhs.velocityCovariance) && (lhs.absVelocityCovariance == rhs.absVelocityCovariance) && (lhs.acceleration == rhs.acceleration) && (lhs.absAcceleration == rhs.absAcceleration) && (lhs.accelerationCovariance == rhs.accelerationCovariance) && (lhs.absAccelerationCovariance == rhs.absAccelerationCovariance) && (lhs.pathPoints == rhs.pathPoints) && (lhs.timeCreation == rhs.timeCreation) && (lhs.lastUpdatedTime == rhs.lastUpdatedTime) && (lhs.cipvFlag == rhs.cipvFlag) && (lhs.blinkerFlag == rhs.blinkerFlag) && (lhs.fusionType == rhs.fusionType) && (lhs.anchorPoint == rhs.anchorPoint) && (lhs.absAnchorPoint == rhs.absAnchorPoint);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFFUSIONOUT_H
