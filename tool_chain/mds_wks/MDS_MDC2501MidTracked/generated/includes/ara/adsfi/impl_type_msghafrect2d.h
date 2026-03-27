/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFRECT2D_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFRECT2D_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msgpoint2d.h"
#include "ara/adsfi/impl_type_msgpoint2dvec.h"

namespace ara {
namespace adsfi {
struct MsgHafRect2D {
    ::ara::adsfi::MsgPoint2D center;
    ::ara::adsfi::MsgPoint2D location;
    ::ara::adsfi::MsgPoint2D centerCovariance;
    ::ara::adsfi::MsgPoint2D size;
    ::ara::adsfi::MsgPoint2D sizeCovariance;
    float orientation{ 0.0F };
    float orientationCovariance{ 0.0F };
    ::ara::adsfi::MsgPoint2DVec corners;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(center);
        fun(location);
        fun(centerCovariance);
        fun(size);
        fun(sizeCovariance);
        fun(orientation);
        fun(orientationCovariance);
        fun(corners);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(center);
        fun(location);
        fun(centerCovariance);
        fun(size);
        fun(sizeCovariance);
        fun(orientation);
        fun(orientationCovariance);
        fun(corners);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("center", center);
        fun("location", location);
        fun("centerCovariance", centerCovariance);
        fun("size", size);
        fun("sizeCovariance", sizeCovariance);
        fun("orientation", orientation);
        fun("orientationCovariance", orientationCovariance);
        fun("corners", corners);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("center", center);
        fun("location", location);
        fun("centerCovariance", centerCovariance);
        fun("size", size);
        fun("sizeCovariance", sizeCovariance);
        fun("orientation", orientation);
        fun("orientationCovariance", orientationCovariance);
        fun("corners", corners);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafRect2D& lhs, const ::ara::adsfi::MsgHafRect2D& rhs) noexcept
    {
        return (lhs.center == rhs.center) && (lhs.location == rhs.location) && (lhs.centerCovariance == rhs.centerCovariance) && (lhs.size == rhs.size) && (lhs.sizeCovariance == rhs.sizeCovariance) && (fabs(static_cast<double>(lhs.orientation - rhs.orientation)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.orientationCovariance - rhs.orientationCovariance)) < DBL_EPSILON) && (lhs.corners == rhs.corners);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFRECT2D_H
