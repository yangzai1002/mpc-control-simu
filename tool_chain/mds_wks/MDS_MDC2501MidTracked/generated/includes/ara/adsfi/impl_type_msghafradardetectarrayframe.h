/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFRADARDETECTARRAYFRAME_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFRADARDETECTARRAYFRAME_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msghaftime.h"
#include "impl_type_string.h"
#include "impl_type_uint8_t.h"
#include "impl_type_uint32_t.h"
#include "ara/adsfi/impl_type_msghafradardetectdatavec.h"

namespace ara {
namespace adsfi {
struct MsgHafRadarDetectArrayFrame {
    ::ara::adsfi::MsgHafTime timestamp;
    ::String frameID;
    ::uint8_t sensorID{ 0U };
    ::uint32_t seq{ 0U };
    ::ara::adsfi::MsgHafRadarDetectDataVec detectList;
    double maxDistanceOverAzimuthList{ 0.0 };
    double azimuthForMaxDistanceList{ 0.0 };
    double factorDistanceOverElevationList{ 0.0 };
    double elevationForFactorMaxDistanceList{ 0.0 };
    double maxDistanceDueToProgram{ 0.0 };
    double minDistanceDueToProgram{ 0.0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(timestamp);
        fun(frameID);
        fun(sensorID);
        fun(seq);
        fun(detectList);
        fun(maxDistanceOverAzimuthList);
        fun(azimuthForMaxDistanceList);
        fun(factorDistanceOverElevationList);
        fun(elevationForFactorMaxDistanceList);
        fun(maxDistanceDueToProgram);
        fun(minDistanceDueToProgram);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(timestamp);
        fun(frameID);
        fun(sensorID);
        fun(seq);
        fun(detectList);
        fun(maxDistanceOverAzimuthList);
        fun(azimuthForMaxDistanceList);
        fun(factorDistanceOverElevationList);
        fun(elevationForFactorMaxDistanceList);
        fun(maxDistanceDueToProgram);
        fun(minDistanceDueToProgram);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("timestamp", timestamp);
        fun("frameID", frameID);
        fun("sensorID", sensorID);
        fun("seq", seq);
        fun("detectList", detectList);
        fun("maxDistanceOverAzimuthList", maxDistanceOverAzimuthList);
        fun("azimuthForMaxDistanceList", azimuthForMaxDistanceList);
        fun("factorDistanceOverElevationList", factorDistanceOverElevationList);
        fun("elevationForFactorMaxDistanceList", elevationForFactorMaxDistanceList);
        fun("maxDistanceDueToProgram", maxDistanceDueToProgram);
        fun("minDistanceDueToProgram", minDistanceDueToProgram);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("timestamp", timestamp);
        fun("frameID", frameID);
        fun("sensorID", sensorID);
        fun("seq", seq);
        fun("detectList", detectList);
        fun("maxDistanceOverAzimuthList", maxDistanceOverAzimuthList);
        fun("azimuthForMaxDistanceList", azimuthForMaxDistanceList);
        fun("factorDistanceOverElevationList", factorDistanceOverElevationList);
        fun("elevationForFactorMaxDistanceList", elevationForFactorMaxDistanceList);
        fun("maxDistanceDueToProgram", maxDistanceDueToProgram);
        fun("minDistanceDueToProgram", minDistanceDueToProgram);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafRadarDetectArrayFrame& lhs, const ::ara::adsfi::MsgHafRadarDetectArrayFrame& rhs) noexcept
    {
        return (lhs.timestamp == rhs.timestamp) && (lhs.frameID == rhs.frameID) && (lhs.sensorID == rhs.sensorID) && (lhs.seq == rhs.seq) && (lhs.detectList == rhs.detectList) && (fabs(static_cast<double>(lhs.maxDistanceOverAzimuthList - rhs.maxDistanceOverAzimuthList)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.azimuthForMaxDistanceList - rhs.azimuthForMaxDistanceList)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.factorDistanceOverElevationList - rhs.factorDistanceOverElevationList)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.elevationForFactorMaxDistanceList - rhs.elevationForFactorMaxDistanceList)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.maxDistanceDueToProgram - rhs.maxDistanceDueToProgram)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.minDistanceDueToProgram - rhs.minDistanceDueToProgram)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFRADARDETECTARRAYFRAME_H
