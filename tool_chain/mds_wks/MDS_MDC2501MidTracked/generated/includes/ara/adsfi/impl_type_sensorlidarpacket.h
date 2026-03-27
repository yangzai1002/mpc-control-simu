/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_SENSORLIDARPACKET_H
#define ARA_ADSFI_IMPL_TYPE_SENSORLIDARPACKET_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "ara/adsfi/impl_type_commonsensorcalibration.h"
#include "ara/adsfi/impl_type_commonudppacketvec.h"
#include "ara/adsfi/impl_type_commonudppacket.h"

namespace ara {
namespace adsfi {
struct SensorLidarPacket {
    ::ara::adsfi::CommonHeader header;
    ::ara::adsfi::CommonSensorCalibration calibration;
    ::ara::adsfi::CommonUdpPacketVec msop_packet;
    ::ara::adsfi::CommonUdpPacket difop_packet;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(calibration);
        fun(msop_packet);
        fun(difop_packet);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(calibration);
        fun(msop_packet);
        fun(difop_packet);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("calibration", calibration);
        fun("msop_packet", msop_packet);
        fun("difop_packet", difop_packet);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("calibration", calibration);
        fun("msop_packet", msop_packet);
        fun("difop_packet", difop_packet);
    }

    friend bool operator==(const ::ara::adsfi::SensorLidarPacket& lhs, const ::ara::adsfi::SensorLidarPacket& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.calibration == rhs.calibration) && (lhs.msop_packet == rhs.msop_packet) && (lhs.difop_packet == rhs.difop_packet);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_SENSORLIDARPACKET_H
