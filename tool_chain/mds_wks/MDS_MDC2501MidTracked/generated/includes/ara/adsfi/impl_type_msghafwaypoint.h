/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFWAYPOINT_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFWAYPOINT_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msghafheader.h"
#include "impl_type_string.h"

namespace ara {
namespace adsfi {
struct MsgHafWayPoint {
    ::ara::adsfi::MsgHafHeader header;
    double xg{ 0.0 };
    double yg{ 0.0 };
    double zg{ 0.0 };
    double x{ 0.0 };
    double y{ 0.0 };
    double angle{ 0.0 };
    double theta{ 0.0 };
    double curvature{ 0.0 };
    double s{ 0.0 };
    double deltaCurvature{ 0.0 };
    ::String laneId;
    double deltaX{ 0.0 };
    double deltaY{ 0.0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(xg);
        fun(yg);
        fun(zg);
        fun(x);
        fun(y);
        fun(angle);
        fun(theta);
        fun(curvature);
        fun(s);
        fun(deltaCurvature);
        fun(laneId);
        fun(deltaX);
        fun(deltaY);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(xg);
        fun(yg);
        fun(zg);
        fun(x);
        fun(y);
        fun(angle);
        fun(theta);
        fun(curvature);
        fun(s);
        fun(deltaCurvature);
        fun(laneId);
        fun(deltaX);
        fun(deltaY);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("xg", xg);
        fun("yg", yg);
        fun("zg", zg);
        fun("x", x);
        fun("y", y);
        fun("angle", angle);
        fun("theta", theta);
        fun("curvature", curvature);
        fun("s", s);
        fun("deltaCurvature", deltaCurvature);
        fun("laneId", laneId);
        fun("deltaX", deltaX);
        fun("deltaY", deltaY);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("xg", xg);
        fun("yg", yg);
        fun("zg", zg);
        fun("x", x);
        fun("y", y);
        fun("angle", angle);
        fun("theta", theta);
        fun("curvature", curvature);
        fun("s", s);
        fun("deltaCurvature", deltaCurvature);
        fun("laneId", laneId);
        fun("deltaX", deltaX);
        fun("deltaY", deltaY);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafWayPoint& lhs, const ::ara::adsfi::MsgHafWayPoint& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.xg - rhs.xg)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.yg - rhs.yg)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.zg - rhs.zg)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.angle - rhs.angle)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.theta - rhs.theta)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.curvature - rhs.curvature)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.s - rhs.s)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.deltaCurvature - rhs.deltaCurvature)) < DBL_EPSILON) && (lhs.laneId == rhs.laneId) && (fabs(static_cast<double>(lhs.deltaX - rhs.deltaX)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.deltaY - rhs.deltaY)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFWAYPOINT_H
