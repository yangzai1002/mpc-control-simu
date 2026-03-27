/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFLOCATION_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFLOCATION_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msghafheader.h"
#include "ara/adsfi/impl_type_msgpoint3d.h"
#include "ara/adsfi/impl_type_msghafposewithcovariance.h"
#include "ara/adsfi/impl_type_msghaftwistwithcovariance.h"
#include "ara/adsfi/impl_type_msghafaccelwithcovariance.h"
#include "impl_type_uint16_t.h"
#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct MsgHafLocation {
    ::ara::adsfi::MsgHafHeader header;
    ::ara::adsfi::MsgPoint3D llh;
    ::ara::adsfi::MsgHafPoseWithCovariance pose;
    ::ara::adsfi::MsgHafPoseWithCovariance dr_pose;
    ::ara::adsfi::MsgHafTwistWithCovariance velocity;
    ::ara::adsfi::MsgHafAccelWithCovariance acceleration;
    double v{ 0.0 };
    ::uint16_t locationState{ 0U };
    ::uint16_t gnssState{ 0U };
    ::uint8_t odomType{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(llh);
        fun(pose);
        fun(dr_pose);
        fun(velocity);
        fun(acceleration);
        fun(v);
        fun(locationState);
        fun(gnssState);
        fun(odomType);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(llh);
        fun(pose);
        fun(dr_pose);
        fun(velocity);
        fun(acceleration);
        fun(v);
        fun(locationState);
        fun(gnssState);
        fun(odomType);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("llh", llh);
        fun("pose", pose);
        fun("dr_pose", dr_pose);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("v", v);
        fun("locationState", locationState);
        fun("gnssState", gnssState);
        fun("odomType", odomType);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("llh", llh);
        fun("pose", pose);
        fun("dr_pose", dr_pose);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("v", v);
        fun("locationState", locationState);
        fun("gnssState", gnssState);
        fun("odomType", odomType);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafLocation& lhs, const ::ara::adsfi::MsgHafLocation& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.llh == rhs.llh) && (lhs.pose == rhs.pose) && (lhs.dr_pose == rhs.dr_pose) && (lhs.velocity == rhs.velocity) && (lhs.acceleration == rhs.acceleration) && (fabs(static_cast<double>(lhs.v - rhs.v)) < DBL_EPSILON) && (lhs.locationState == rhs.locationState) && (lhs.gnssState == rhs.gnssState) && (lhs.odomType == rhs.odomType);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFLOCATION_H
