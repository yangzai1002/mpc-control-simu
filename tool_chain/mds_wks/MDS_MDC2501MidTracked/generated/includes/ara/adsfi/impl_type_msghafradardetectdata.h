/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFRADARDETECTDATA_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFRADARDETECTDATA_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint8_t.h"
#include "ara/adsfi/impl_type_msghafradarmodedatainfo.h"

namespace ara {
namespace adsfi {
struct MsgHafRadarDetectData {
    ::uint8_t id{ 0U };
    ::uint8_t idPair{ 0U };
    ::uint8_t coordinate{ 0U };
    ::ara::adsfi::MsgHafRadarModeDatainfo position;
    ::ara::adsfi::MsgHafRadarModeDatainfo velocity;
    ::ara::adsfi::MsgHafRadarModeDatainfo acceleration;
    double rcs{ 0.0 };
    double snr{ 0.0 };
    ::uint8_t existProbability{ 0U };
    ::uint8_t falseProbability{ 0U };
    ::uint8_t movProperty{ 0U };
    ::uint8_t invalidState{ 0U };
    ::uint8_t ambiguity{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(idPair);
        fun(coordinate);
        fun(position);
        fun(velocity);
        fun(acceleration);
        fun(rcs);
        fun(snr);
        fun(existProbability);
        fun(falseProbability);
        fun(movProperty);
        fun(invalidState);
        fun(ambiguity);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(idPair);
        fun(coordinate);
        fun(position);
        fun(velocity);
        fun(acceleration);
        fun(rcs);
        fun(snr);
        fun(existProbability);
        fun(falseProbability);
        fun(movProperty);
        fun(invalidState);
        fun(ambiguity);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("idPair", idPair);
        fun("coordinate", coordinate);
        fun("position", position);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("rcs", rcs);
        fun("snr", snr);
        fun("existProbability", existProbability);
        fun("falseProbability", falseProbability);
        fun("movProperty", movProperty);
        fun("invalidState", invalidState);
        fun("ambiguity", ambiguity);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("idPair", idPair);
        fun("coordinate", coordinate);
        fun("position", position);
        fun("velocity", velocity);
        fun("acceleration", acceleration);
        fun("rcs", rcs);
        fun("snr", snr);
        fun("existProbability", existProbability);
        fun("falseProbability", falseProbability);
        fun("movProperty", movProperty);
        fun("invalidState", invalidState);
        fun("ambiguity", ambiguity);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafRadarDetectData& lhs, const ::ara::adsfi::MsgHafRadarDetectData& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.idPair == rhs.idPair) && (lhs.coordinate == rhs.coordinate) && (lhs.position == rhs.position) && (lhs.velocity == rhs.velocity) && (lhs.acceleration == rhs.acceleration) && (fabs(static_cast<double>(lhs.rcs - rhs.rcs)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.snr - rhs.snr)) < DBL_EPSILON) && (lhs.existProbability == rhs.existProbability) && (lhs.falseProbability == rhs.falseProbability) && (lhs.movProperty == rhs.movProperty) && (lhs.invalidState == rhs.invalidState) && (lhs.ambiguity == rhs.ambiguity);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFRADARDETECTDATA_H
