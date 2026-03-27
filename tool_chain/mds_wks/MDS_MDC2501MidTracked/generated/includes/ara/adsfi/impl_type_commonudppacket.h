/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_COMMONUDPPACKET_H
#define ARA_ADSFI_IMPL_TYPE_COMMONUDPPACKET_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "ara/adsfi/impl_type_idpuint8vec.h"

namespace ara {
namespace adsfi {
struct CommonUdpPacket {
    ::ara::adsfi::CommonHeader header;
    ::ara::adsfi::IdpUint8Vec udp_packet;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(udp_packet);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(udp_packet);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("udp_packet", udp_packet);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("udp_packet", udp_packet);
    }

    friend bool operator==(const ::ara::adsfi::CommonUdpPacket& lhs, const ::ara::adsfi::CommonUdpPacket& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.udp_packet == rhs.udp_packet);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_COMMONUDPPACKET_H
