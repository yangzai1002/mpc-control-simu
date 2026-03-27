/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAF3DDETECTIONOUT_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAF3DDETECTIONOUT_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32_t.h"
#include "impl_type_uint32_t.h"
#include "impl_type_string.h"
#include "ara/adsfi/impl_type_msghafrect3d.h"
#include "ara/adsfi/impl_type_msgpoint3f.h"
#include "ara/adsfi/impl_type_msgpoint3fvec.h"
#include "ara/adsfi/impl_type_msghaftime.h"
#include "ara/adsfi/impl_type_msgpoint3d.h"
#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct MsgHaf3dDetectionOut {
    ::int32_t objectId{ 0 };
    ::uint32_t cls{ 0U };
    ::uint32_t types{ 0U };
    float confidence{ 0.0F };
    float existenceProbability{ 0.0F };
    ::String coordinate;
    ::ara::adsfi::MsgHafRect3D rect;
    ::ara::adsfi::MsgPoint3f velocity;
    ::ara::adsfi::MsgPoint3fVec contourPoints;
    ::ara::adsfi::MsgHafTime timeCreation;
    ::ara::adsfi::MsgPoint3D acceleration;
    ::uint32_t blinkerStatus{ 0U };
    ::uint32_t cipvFlag{ 0U };
    ::uint8_t cameraStatus{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(objectId);
        fun(cls);
        fun(types);
        fun(confidence);
        fun(existenceProbability);
        fun(coordinate);
        fun(rect);
        fun(velocity);
        fun(contourPoints);
        fun(timeCreation);
        fun(acceleration);
        fun(blinkerStatus);
        fun(cipvFlag);
        fun(cameraStatus);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(objectId);
        fun(cls);
        fun(types);
        fun(confidence);
        fun(existenceProbability);
        fun(coordinate);
        fun(rect);
        fun(velocity);
        fun(contourPoints);
        fun(timeCreation);
        fun(acceleration);
        fun(blinkerStatus);
        fun(cipvFlag);
        fun(cameraStatus);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("objectId", objectId);
        fun("cls", cls);
        fun("types", types);
        fun("confidence", confidence);
        fun("existenceProbability", existenceProbability);
        fun("coordinate", coordinate);
        fun("rect", rect);
        fun("velocity", velocity);
        fun("contourPoints", contourPoints);
        fun("timeCreation", timeCreation);
        fun("acceleration", acceleration);
        fun("blinkerStatus", blinkerStatus);
        fun("cipvFlag", cipvFlag);
        fun("cameraStatus", cameraStatus);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("objectId", objectId);
        fun("cls", cls);
        fun("types", types);
        fun("confidence", confidence);
        fun("existenceProbability", existenceProbability);
        fun("coordinate", coordinate);
        fun("rect", rect);
        fun("velocity", velocity);
        fun("contourPoints", contourPoints);
        fun("timeCreation", timeCreation);
        fun("acceleration", acceleration);
        fun("blinkerStatus", blinkerStatus);
        fun("cipvFlag", cipvFlag);
        fun("cameraStatus", cameraStatus);
    }

    friend bool operator==(const ::ara::adsfi::MsgHaf3dDetectionOut& lhs, const ::ara::adsfi::MsgHaf3dDetectionOut& rhs) noexcept
    {
        return (lhs.objectId == rhs.objectId) && (lhs.cls == rhs.cls) && (lhs.types == rhs.types) && (fabs(static_cast<double>(lhs.confidence - rhs.confidence)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.existenceProbability - rhs.existenceProbability)) < DBL_EPSILON) && (lhs.coordinate == rhs.coordinate) && (lhs.rect == rhs.rect) && (lhs.velocity == rhs.velocity) && (lhs.contourPoints == rhs.contourPoints) && (lhs.timeCreation == rhs.timeCreation) && (lhs.acceleration == rhs.acceleration) && (lhs.blinkerStatus == rhs.blinkerStatus) && (lhs.cipvFlag == rhs.cipvFlag) && (lhs.cameraStatus == rhs.cameraStatus);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAF3DDETECTIONOUT_H
