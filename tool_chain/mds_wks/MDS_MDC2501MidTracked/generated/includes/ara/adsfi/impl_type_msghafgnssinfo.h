/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFGNSSINFO_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFGNSSINFO_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msghafheader.h"
#include "ara/adsfi/impl_type_msgpoint3d.h"
#include "impl_type_int32_t.h"
#include "impl_type_uint8_t.h"
#include "impl_type_uint16_t.h"

namespace ara {
namespace adsfi {
struct MsgHafGnssInfo {
    ::ara::adsfi::MsgHafHeader header;
    double latitude{ 0.0 };
    double longitude{ 0.0 };
    double elevation{ 0.0 };
    ::ara::adsfi::MsgPoint3D utmPosition;
    ::int32_t utmZoneNum{ 0 };
    ::uint8_t utmZoneChar{ 0U };
    ::uint8_t heading_valid{ 0U };
    ::uint8_t pos_valid{ 0U };
    ::ara::adsfi::MsgPoint3D attitude;
    ::ara::adsfi::MsgPoint3D sdPosition;
    ::ara::adsfi::MsgPoint3D sdVelocity;
    ::ara::adsfi::MsgPoint3D sdAttitude;
    double second{ 0.0 };
    ::int32_t satUseNum{ 0 };
    ::int32_t satInViewNum{ 0 };
    ::uint16_t solutionStatus{ 0U };
    ::uint16_t positionType{ 0U };
    ::ara::adsfi::MsgPoint3D linearVelocity;
    ::ara::adsfi::MsgPoint3D attitudeDual;
    ::ara::adsfi::MsgPoint3D sdAngleDual;
    double baseLineLengthDual{ 0.0 };
    ::int32_t solutionStatusDual{ 0 };
    ::int32_t positionTypeDual{ 0 };
    ::int32_t solutionSourceDual{ 0 };
    double cep68{ 0.0 };
    double cep95{ 0.0 };
    float pDop{ 0.0F };
    float hDop{ 0.0F };
    float vDop{ 0.0F };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(latitude);
        fun(longitude);
        fun(elevation);
        fun(utmPosition);
        fun(utmZoneNum);
        fun(utmZoneChar);
        fun(heading_valid);
        fun(pos_valid);
        fun(attitude);
        fun(sdPosition);
        fun(sdVelocity);
        fun(sdAttitude);
        fun(second);
        fun(satUseNum);
        fun(satInViewNum);
        fun(solutionStatus);
        fun(positionType);
        fun(linearVelocity);
        fun(attitudeDual);
        fun(sdAngleDual);
        fun(baseLineLengthDual);
        fun(solutionStatusDual);
        fun(positionTypeDual);
        fun(solutionSourceDual);
        fun(cep68);
        fun(cep95);
        fun(pDop);
        fun(hDop);
        fun(vDop);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(latitude);
        fun(longitude);
        fun(elevation);
        fun(utmPosition);
        fun(utmZoneNum);
        fun(utmZoneChar);
        fun(heading_valid);
        fun(pos_valid);
        fun(attitude);
        fun(sdPosition);
        fun(sdVelocity);
        fun(sdAttitude);
        fun(second);
        fun(satUseNum);
        fun(satInViewNum);
        fun(solutionStatus);
        fun(positionType);
        fun(linearVelocity);
        fun(attitudeDual);
        fun(sdAngleDual);
        fun(baseLineLengthDual);
        fun(solutionStatusDual);
        fun(positionTypeDual);
        fun(solutionSourceDual);
        fun(cep68);
        fun(cep95);
        fun(pDop);
        fun(hDop);
        fun(vDop);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("latitude", latitude);
        fun("longitude", longitude);
        fun("elevation", elevation);
        fun("utmPosition", utmPosition);
        fun("utmZoneNum", utmZoneNum);
        fun("utmZoneChar", utmZoneChar);
        fun("heading_valid", heading_valid);
        fun("pos_valid", pos_valid);
        fun("attitude", attitude);
        fun("sdPosition", sdPosition);
        fun("sdVelocity", sdVelocity);
        fun("sdAttitude", sdAttitude);
        fun("second", second);
        fun("satUseNum", satUseNum);
        fun("satInViewNum", satInViewNum);
        fun("solutionStatus", solutionStatus);
        fun("positionType", positionType);
        fun("linearVelocity", linearVelocity);
        fun("attitudeDual", attitudeDual);
        fun("sdAngleDual", sdAngleDual);
        fun("baseLineLengthDual", baseLineLengthDual);
        fun("solutionStatusDual", solutionStatusDual);
        fun("positionTypeDual", positionTypeDual);
        fun("solutionSourceDual", solutionSourceDual);
        fun("cep68", cep68);
        fun("cep95", cep95);
        fun("pDop", pDop);
        fun("hDop", hDop);
        fun("vDop", vDop);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("latitude", latitude);
        fun("longitude", longitude);
        fun("elevation", elevation);
        fun("utmPosition", utmPosition);
        fun("utmZoneNum", utmZoneNum);
        fun("utmZoneChar", utmZoneChar);
        fun("heading_valid", heading_valid);
        fun("pos_valid", pos_valid);
        fun("attitude", attitude);
        fun("sdPosition", sdPosition);
        fun("sdVelocity", sdVelocity);
        fun("sdAttitude", sdAttitude);
        fun("second", second);
        fun("satUseNum", satUseNum);
        fun("satInViewNum", satInViewNum);
        fun("solutionStatus", solutionStatus);
        fun("positionType", positionType);
        fun("linearVelocity", linearVelocity);
        fun("attitudeDual", attitudeDual);
        fun("sdAngleDual", sdAngleDual);
        fun("baseLineLengthDual", baseLineLengthDual);
        fun("solutionStatusDual", solutionStatusDual);
        fun("positionTypeDual", positionTypeDual);
        fun("solutionSourceDual", solutionSourceDual);
        fun("cep68", cep68);
        fun("cep95", cep95);
        fun("pDop", pDop);
        fun("hDop", hDop);
        fun("vDop", vDop);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafGnssInfo& lhs, const ::ara::adsfi::MsgHafGnssInfo& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.latitude - rhs.latitude)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.longitude - rhs.longitude)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.elevation - rhs.elevation)) < DBL_EPSILON) && (lhs.utmPosition == rhs.utmPosition) && (lhs.utmZoneNum == rhs.utmZoneNum) && (lhs.utmZoneChar == rhs.utmZoneChar) && (lhs.heading_valid == rhs.heading_valid) && (lhs.pos_valid == rhs.pos_valid) && (lhs.attitude == rhs.attitude) && (lhs.sdPosition == rhs.sdPosition) && (lhs.sdVelocity == rhs.sdVelocity) && (lhs.sdAttitude == rhs.sdAttitude) && (fabs(static_cast<double>(lhs.second - rhs.second)) < DBL_EPSILON) && (lhs.satUseNum == rhs.satUseNum) && (lhs.satInViewNum == rhs.satInViewNum) && (lhs.solutionStatus == rhs.solutionStatus) && (lhs.positionType == rhs.positionType) && (lhs.linearVelocity == rhs.linearVelocity) && (lhs.attitudeDual == rhs.attitudeDual) && (lhs.sdAngleDual == rhs.sdAngleDual) && (fabs(static_cast<double>(lhs.baseLineLengthDual - rhs.baseLineLengthDual)) < DBL_EPSILON) && (lhs.solutionStatusDual == rhs.solutionStatusDual) && (lhs.positionTypeDual == rhs.positionTypeDual) && (lhs.solutionSourceDual == rhs.solutionSourceDual) && (fabs(static_cast<double>(lhs.cep68 - rhs.cep68)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.cep95 - rhs.cep95)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.pDop - rhs.pDop)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.hDop - rhs.hDop)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.vDop - rhs.vDop)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFGNSSINFO_H
