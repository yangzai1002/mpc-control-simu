/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFESTOP_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFESTOP_H

#include "ara/adsfi/impl_type_msghafheader.h"
#include "impl_type_uint8_t.h"
#include "impl_type_string.h"

namespace ara {
namespace adsfi {
struct MsgHafEstop {
    ::ara::adsfi::MsgHafHeader header;
    ::uint8_t isEstop{ 0U };
    ::String description;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(isEstop);
        fun(description);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(isEstop);
        fun(description);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("isEstop", isEstop);
        fun("description", description);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("isEstop", isEstop);
        fun("description", description);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafEstop& lhs, const ::ara::adsfi::MsgHafEstop& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.isEstop == rhs.isEstop) && (lhs.description == rhs.description);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFESTOP_H
