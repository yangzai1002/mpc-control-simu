/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFTIME_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFTIME_H

#include "impl_type_uint32_t.h"

namespace ara {
namespace adsfi {
struct MsgHafTime {
    ::uint32_t sec{ 0U };
    ::uint32_t nsec{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(sec);
        fun(nsec);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(sec);
        fun(nsec);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("sec", sec);
        fun("nsec", nsec);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("sec", sec);
        fun("nsec", nsec);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafTime& lhs, const ::ara::adsfi::MsgHafTime& rhs) noexcept
    {
        return (lhs.sec == rhs.sec) && (lhs.nsec == rhs.nsec);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFTIME_H
