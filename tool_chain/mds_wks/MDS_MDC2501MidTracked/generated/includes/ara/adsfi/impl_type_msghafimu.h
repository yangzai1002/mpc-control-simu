/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFIMU_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFIMU_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msghafheader.h"
#include "ara/adsfi/impl_type_msgpoint3d.h"
#include "impl_type_uint16_t.h"

namespace ara {
namespace adsfi {
struct MsgHafIMU {
    ::ara::adsfi::MsgHafHeader imuHeader;
    ::ara::adsfi::MsgPoint3D angularVelocity;
    ::ara::adsfi::MsgPoint3D linearAcceleration;
    ::uint16_t imuStatus{ 0U };
    float temperature{ 0.0F };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(imuHeader);
        fun(angularVelocity);
        fun(linearAcceleration);
        fun(imuStatus);
        fun(temperature);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(imuHeader);
        fun(angularVelocity);
        fun(linearAcceleration);
        fun(imuStatus);
        fun(temperature);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("imuHeader", imuHeader);
        fun("angularVelocity", angularVelocity);
        fun("linearAcceleration", linearAcceleration);
        fun("imuStatus", imuStatus);
        fun("temperature", temperature);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("imuHeader", imuHeader);
        fun("angularVelocity", angularVelocity);
        fun("linearAcceleration", linearAcceleration);
        fun("imuStatus", imuStatus);
        fun("temperature", temperature);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafIMU& lhs, const ::ara::adsfi::MsgHafIMU& rhs) noexcept
    {
        return (lhs.imuHeader == rhs.imuHeader) && (lhs.angularVelocity == rhs.angularVelocity) && (lhs.linearAcceleration == rhs.linearAcceleration) && (lhs.imuStatus == rhs.imuStatus) && (fabs(static_cast<double>(lhs.temperature - rhs.temperature)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFIMU_H
