/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKDATA_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKDATA_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint8_t.h"
#include "ara/adsfi/impl_type_msghafradarmodedatainfo.h"
#include "ara/adsfi/impl_type_msghafradartrackorient.h"
#include "ara/adsfi/impl_type_msghafradartrackyaw.h"

namespace ara {
namespace adsfi {
struct MsgHafRadarTrackData {
    ::uint8_t id{ 0U };
    ::uint8_t idState{ 0U };
    ::uint8_t lifetime{ 0U };
    ::ara::adsfi::MsgHafRadarModeDatainfo position;
    ::ara::adsfi::MsgHafRadarModeDatainfo velocity;
    ::ara::adsfi::MsgHafRadarModeDatainfo acceleration;
    ::ara::adsfi::MsgHafRadarModeDatainfo size;
    ::ara::adsfi::MsgHafRadarTrackOrient orient;
    ::ara::adsfi::MsgHafRadarTrackYaw yaw;
    double rcs{ 0.0 };
    double snr{ 0.0 };
    double underpassProbability{ 0.0 };
    double overpassProbability{ 0.0 };
    ::uint8_t existProbability{ 0U };
    ::uint8_t movProperty{ 0U };
    ::uint8_t trackState{ 0U };
    ::uint8_t trackType{ 0U };
    double heading{ 0.0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(idState);
        fun(lifetime);
        fun(position);
        fun(velocity);
        fun(acceleration);
        fun(size);
        fun(orient);
        fun(yaw);
        fun(rcs);
        fun(snr);
        fun(underpassProbability);
        fun(overpassProbability);
        fun(existProbability);
        fun(movProperty);
        fun(trackState);
        fun(trackType);
        fun(heading);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(idState);
        fun(lifetime);
        fun(position);
        fun(velocity);
        fun(acceleration);
        fun(size);
        fun(orient);
        fun(yaw);
        fun(rcs);
        fun(snr);
        fun(underpassProbability);
        fun(overpassProbability);
        fun(existProbability);
        fun(movProperty);
        fun(trackState);
        fun(trackType);
        fun(heading);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("idState", idState);
        fun("lifetime", lifetime);
        fun("position", position);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("size", size);
        fun("orient", orient);
        fun("yaw", yaw);
        fun("rcs", rcs);
        fun("snr", snr);
        fun("underpassProbability", underpassProbability);
        fun("overpassProbability", overpassProbability);
        fun("existProbability", existProbability);
        fun("movProperty", movProperty);
        fun("trackState", trackState);
        fun("trackType", trackType);
        fun("heading", heading);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("idState", idState);
        fun("lifetime", lifetime);
        fun("position", position);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("size", size);
        fun("orient", orient);
        fun("yaw", yaw);
        fun("rcs", rcs);
        fun("snr", snr);
        fun("underpassProbability", underpassProbability);
        fun("overpassProbability", overpassProbability);
        fun("existProbability", existProbability);
        fun("movProperty", movProperty);
        fun("trackState", trackState);
        fun("trackType", trackType);
        fun("heading", heading);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafRadarTrackData& lhs, const ::ara::adsfi::MsgHafRadarTrackData& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.idState == rhs.idState) && (lhs.lifetime == rhs.lifetime) && (lhs.position == rhs.position) && (lhs.velocity == rhs.velocity) && (lhs.acceleration == rhs.acceleration) && (lhs.size == rhs.size) && (lhs.orient == rhs.orient) && (lhs.yaw == rhs.yaw) && (fabs(static_cast<double>(lhs.rcs - rhs.rcs)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.snr - rhs.snr)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.underpassProbability - rhs.underpassProbability)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.overpassProbability - rhs.overpassProbability)) < DBL_EPSILON) && (lhs.existProbability == rhs.existProbability) && (lhs.movProperty == rhs.movProperty) && (lhs.trackState == rhs.trackState) && (lhs.trackType == rhs.trackType) && (fabs(static_cast<double>(lhs.heading - rhs.heading)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFRADARTRACKDATA_H
