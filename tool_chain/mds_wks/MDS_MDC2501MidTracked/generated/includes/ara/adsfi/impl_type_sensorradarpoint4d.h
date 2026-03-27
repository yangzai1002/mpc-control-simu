/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_SENSORRADARPOINT4D_H
#define ARA_ADSFI_IMPL_TYPE_SENSORRADARPOINT4D_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32_t.h"
#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct SensorRadarPoint4D {
    ::int32_t id{ 0 };
    ::int32_t motionStatus{ 0 };
    float azimuth{ 0.0F };
    float elevation{ 0.0F };
    float range{ 0.0F };
    float range_rate{ 0.0F };
    float rangeStdDev{ 0.0F };
    float rangeRateStdDev{ 0.0F };
    float azimuthStdDev{ 0.0F };
    float elevationStdDev{ 0.0F };
    ::int32_t existStatus{ 0 };
    ::int32_t falseAlarmProb{ 0 };
    ::int32_t heightConfidence{ 0 };
    ::uint8_t snr{ 0U };
    ::int32_t attribState{ 0 };
    ::uint8_t ambg_state{ 0U };
    ::int32_t modeFlag{ 0 };
    float x{ 0.0F };
    float y{ 0.0F };
    float z{ 0.0F };
    float vx{ 0.0F };
    float vy{ 0.0F };
    float vz{ 0.0F };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(motionStatus);
        fun(azimuth);
        fun(elevation);
        fun(range);
        fun(range_rate);
        fun(rangeStdDev);
        fun(rangeRateStdDev);
        fun(azimuthStdDev);
        fun(elevationStdDev);
        fun(existStatus);
        fun(falseAlarmProb);
        fun(heightConfidence);
        fun(snr);
        fun(attribState);
        fun(ambg_state);
        fun(modeFlag);
        fun(x);
        fun(y);
        fun(z);
        fun(vx);
        fun(vy);
        fun(vz);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(motionStatus);
        fun(azimuth);
        fun(elevation);
        fun(range);
        fun(range_rate);
        fun(rangeStdDev);
        fun(rangeRateStdDev);
        fun(azimuthStdDev);
        fun(elevationStdDev);
        fun(existStatus);
        fun(falseAlarmProb);
        fun(heightConfidence);
        fun(snr);
        fun(attribState);
        fun(ambg_state);
        fun(modeFlag);
        fun(x);
        fun(y);
        fun(z);
        fun(vx);
        fun(vy);
        fun(vz);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("motionStatus", motionStatus);
        fun("azimuth", azimuth);
        fun("elevation", elevation);
        fun("range", range);
        fun("range_rate", range_rate);
        fun("rangeStdDev", rangeStdDev);
        fun("rangeRateStdDev", rangeRateStdDev);
        fun("azimuthStdDev", azimuthStdDev);
        fun("elevationStdDev", elevationStdDev);
        fun("existStatus", existStatus);
        fun("falseAlarmProb", falseAlarmProb);
        fun("heightConfidence", heightConfidence);
        fun("snr", snr);
        fun("attribState", attribState);
        fun("ambg_state", ambg_state);
        fun("modeFlag", modeFlag);
        fun("x", x);
        fun("y", y);
        fun("z", z);
        fun("vx", vx);
        fun("vy", vy);
        fun("vz", vz);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("motionStatus", motionStatus);
        fun("azimuth", azimuth);
        fun("elevation", elevation);
        fun("range", range);
        fun("range_rate", range_rate);
        fun("rangeStdDev", rangeStdDev);
        fun("rangeRateStdDev", rangeRateStdDev);
        fun("azimuthStdDev", azimuthStdDev);
        fun("elevationStdDev", elevationStdDev);
        fun("existStatus", existStatus);
        fun("falseAlarmProb", falseAlarmProb);
        fun("heightConfidence", heightConfidence);
        fun("snr", snr);
        fun("attribState", attribState);
        fun("ambg_state", ambg_state);
        fun("modeFlag", modeFlag);
        fun("x", x);
        fun("y", y);
        fun("z", z);
        fun("vx", vx);
        fun("vy", vy);
        fun("vz", vz);
    }

    friend bool operator==(const ::ara::adsfi::SensorRadarPoint4D& lhs, const ::ara::adsfi::SensorRadarPoint4D& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.motionStatus == rhs.motionStatus) && (fabs(static_cast<double>(lhs.azimuth - rhs.azimuth)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.elevation - rhs.elevation)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.range - rhs.range)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.range_rate - rhs.range_rate)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.rangeStdDev - rhs.rangeStdDev)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.rangeRateStdDev - rhs.rangeRateStdDev)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.azimuthStdDev - rhs.azimuthStdDev)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.elevationStdDev - rhs.elevationStdDev)) < DBL_EPSILON) && (lhs.existStatus == rhs.existStatus) && (lhs.falseAlarmProb == rhs.falseAlarmProb) && (lhs.heightConfidence == rhs.heightConfidence) && (lhs.snr == rhs.snr) && (lhs.attribState == rhs.attribState) && (lhs.ambg_state == rhs.ambg_state) && (lhs.modeFlag == rhs.modeFlag) && (fabs(static_cast<double>(lhs.x - rhs.x)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.y - rhs.y)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.z - rhs.z)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.vx - rhs.vx)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.vy - rhs.vy)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.vz - rhs.vz)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_SENSORRADARPOINT4D_H
