/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VIDEORTPPARAMETER_H
#define ARA_ADSFI_IMPL_TYPE_VIDEORTPPARAMETER_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_int32_t.h"
#include "impl_type_string.h"

namespace ara {
namespace adsfi {
struct VideoRTPParameter {
    ::ara::adsfi::CommonHeader header;
    ::int32_t ssrc{ 0 };
    ::String peer_host;
    ::int32_t peer_port{ 0 };
    ::int32_t push_enabled{ 0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(ssrc);
        fun(peer_host);
        fun(peer_port);
        fun(push_enabled);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(ssrc);
        fun(peer_host);
        fun(peer_port);
        fun(push_enabled);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("ssrc", ssrc);
        fun("peer_host", peer_host);
        fun("peer_port", peer_port);
        fun("push_enabled", push_enabled);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("ssrc", ssrc);
        fun("peer_host", peer_host);
        fun("peer_port", peer_port);
        fun("push_enabled", push_enabled);
    }

    friend bool operator==(const ::ara::adsfi::VideoRTPParameter& lhs, const ::ara::adsfi::VideoRTPParameter& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.ssrc == rhs.ssrc) && (lhs.peer_host == rhs.peer_host) && (lhs.peer_port == rhs.peer_port) && (lhs.push_enabled == rhs.push_enabled);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VIDEORTPPARAMETER_H
