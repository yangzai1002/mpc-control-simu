/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFUINT8WITHVALID_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFUINT8WITHVALID_H

#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct MsgHafUint8WithValid {
    ::uint8_t value{ 0U };
    ::uint8_t confidence{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(value);
        fun(confidence);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(value);
        fun(confidence);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("value", value);
        fun("confidence", confidence);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("value", value);
        fun("confidence", confidence);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafUint8WithValid& lhs, const ::ara::adsfi::MsgHafUint8WithValid& rhs) noexcept
    {
        return (lhs.value == rhs.value) && (lhs.confidence == rhs.confidence);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFUINT8WITHVALID_H
