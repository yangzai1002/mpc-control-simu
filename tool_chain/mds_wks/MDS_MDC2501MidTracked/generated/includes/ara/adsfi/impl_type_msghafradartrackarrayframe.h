/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKARRAYFRAME_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKARRAYFRAME_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msghaftime.h"
#include "impl_type_string.h"
#include "impl_type_uint8_t.h"
#include "impl_type_uint32_t.h"
#include "ara/adsfi/impl_type_msghafradartrackdatavec.h"

namespace ara {
namespace adsfi {
struct MsgHafRadarTrackArrayFrame {
    ::ara::adsfi::MsgHafTime timestamp;
    ::String frameID;
    ::uint8_t sensorID{ 0U };
    ::uint32_t seq{ 0U };
    ::ara::adsfi::MsgHafRadarTrackDataVec trackList;
    double yawRate{ 0.0 };
    double vehicleSpeed{ 0.0 };

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
        fun(trackList);
        fun(yawRate);
        fun(vehicleSpeed);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(timestamp);
        fun(frameID);
        fun(sensorID);
        fun(seq);
        fun(trackList);
        fun(yawRate);
        fun(vehicleSpeed);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("timestamp", timestamp);
        fun("frameID", frameID);
        fun("sensorID", sensorID);
        fun("seq", seq);
        fun("trackList", trackList);
        fun("yawRate", yawRate);
        fun("vehicleSpeed", vehicleSpeed);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("timestamp", timestamp);
        fun("frameID", frameID);
        fun("sensorID", sensorID);
        fun("seq", seq);
        fun("trackList", trackList);
        fun("yawRate", yawRate);
        fun("vehicleSpeed", vehicleSpeed);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafRadarTrackArrayFrame& lhs, const ::ara::adsfi::MsgHafRadarTrackArrayFrame& rhs) noexcept
    {
        return (lhs.timestamp == rhs.timestamp) && (lhs.frameID == rhs.frameID) && (lhs.sensorID == rhs.sensorID) && (lhs.seq == rhs.seq) && (lhs.trackList == rhs.trackList) && (fabs(static_cast<double>(lhs.yawRate - rhs.yawRate)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.vehicleSpeed - rhs.vehicleSpeed)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKARRAYFRAME_H
