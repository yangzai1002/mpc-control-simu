/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_RECT3D_H
#define IMPL_TYPE_RECT3D_H

#include "impl_type_point.h"
#include "impl_type_pointarray.h"

struct Rect3d {
    ::Point center;
    ::Point centerCovariance;
    ::Point size;
    ::Point sizeCovariance;
    ::Point orientation;
    ::Point orientationCovariance;
    ::PointArray corners;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(center);
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
        fun("centerCovariance", centerCovariance);
        fun("size", size);
        fun("sizeCovariance", sizeCovariance);
        fun("orientation", orientation);
        fun("orientationCovariance", orientationCovariance);
        fun("corners", corners);
    }

    friend bool operator==(const ::Rect3d& lhs, const ::Rect3d& rhs) noexcept
    {
        return (lhs.center == rhs.center) && (lhs.centerCovariance == rhs.centerCovariance) && (lhs.size == rhs.size) && (lhs.sizeCovariance == rhs.sizeCovariance) && (lhs.orientation == rhs.orientation) && (lhs.orientationCovariance == rhs.orientationCovariance) && (lhs.corners == rhs.corners);
    }
};


#endif // IMPL_TYPE_RECT3D_H
