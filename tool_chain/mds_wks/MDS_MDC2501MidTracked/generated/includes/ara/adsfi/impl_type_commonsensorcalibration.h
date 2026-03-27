/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_COMMONSENSORCALIBRATION_H
#define ARA_ADSFI_IMPL_TYPE_COMMONSENSORCALIBRATION_H

#include <cfloat>
#include <cmath>
#include "impl_type_string.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct CommonSensorCalibration {
    ::String name;
    ::int32_t prerotaxis_0{ 0 };
    ::int32_t prerotaxis_1{ 0 };
    ::int32_t prerotaxis_2{ 0 };
    float prerotdegree_0{ 0.0F };
    float prerotdegree_1{ 0.0F };
    float prerotdegree_2{ 0.0F };
    float xoffset{ 0.0F };
    float yoffset{ 0.0F };
    float zoffset{ 0.0F };
    float roll{ 0.0F };
    float pitch{ 0.0F };
    float yaw{ 0.0F };
    float z_ground_offset{ 0.0F };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(name);
        fun(prerotaxis_0);
        fun(prerotaxis_1);
        fun(prerotaxis_2);
        fun(prerotdegree_0);
        fun(prerotdegree_1);
        fun(prerotdegree_2);
        fun(xoffset);
        fun(yoffset);
        fun(zoffset);
        fun(roll);
        fun(pitch);
        fun(yaw);
        fun(z_ground_offset);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(name);
        fun(prerotaxis_0);
        fun(prerotaxis_1);
        fun(prerotaxis_2);
        fun(prerotdegree_0);
        fun(prerotdegree_1);
        fun(prerotdegree_2);
        fun(xoffset);
        fun(yoffset);
        fun(zoffset);
        fun(roll);
        fun(pitch);
        fun(yaw);
        fun(z_ground_offset);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("name", name);
        fun("prerotaxis_0", prerotaxis_0);
        fun("prerotaxis_1", prerotaxis_1);
        fun("prerotaxis_2", prerotaxis_2);
        fun("prerotdegree_0", prerotdegree_0);
        fun("prerotdegree_1", prerotdegree_1);
        fun("prerotdegree_2", prerotdegree_2);
        fun("xoffset", xoffset);
        fun("yoffset", yoffset);
        fun("zoffset", zoffset);
        fun("roll", roll);
        fun("pitch", pitch);
        fun("yaw", yaw);
        fun("z_ground_offset", z_ground_offset);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("name", name);
        fun("prerotaxis_0", prerotaxis_0);
        fun("prerotaxis_1", prerotaxis_1);
        fun("prerotaxis_2", prerotaxis_2);
        fun("prerotdegree_0", prerotdegree_0);
        fun("prerotdegree_1", prerotdegree_1);
        fun("prerotdegree_2", prerotdegree_2);
        fun("xoffset", xoffset);
        fun("yoffset", yoffset);
        fun("zoffset", zoffset);
        fun("roll", roll);
        fun("pitch", pitch);
        fun("yaw", yaw);
        fun("z_ground_offset", z_ground_offset);
    }

    friend bool operator==(const ::ara::adsfi::CommonSensorCalibration& lhs, const ::ara::adsfi::CommonSensorCalibration& rhs) noexcept
    {
        return (lhs.name == rhs.name) && (lhs.prerotaxis_0 == rhs.prerotaxis_0) && (lhs.prerotaxis_1 == rhs.prerotaxis_1) && (lhs.prerotaxis_2 == rhs.prerotaxis_2) && (fabs(static_cast<double>(lhs.prerotdegree_0 - rhs.prerotdegree_0)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.prerotdegree_1 - rhs.prerotdegree_1)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.prerotdegree_2 - rhs.prerotdegree_2)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.xoffset - rhs.xoffset)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.yoffset - rhs.yoffset)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.zoffset - rhs.zoffset)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.roll - rhs.roll)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.pitch - rhs.pitch)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.yaw - rhs.yaw)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.z_ground_offset - rhs.z_ground_offset)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_COMMONSENSORCALIBRATION_H
