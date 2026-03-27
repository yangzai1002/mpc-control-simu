/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_LANETIME_H
#define ARA_ADSFI_IMPL_TYPE_LANETIME_H

#include "impl_type_uint32.h"

namespace ara {
namespace adsfi {
struct LaneTime {
    ::UInt32 sec{ 0U };
    ::UInt32 nsec{ 0U };

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

    friend bool operator==(const ::ara::adsfi::LaneTime& lhs, const ::ara::adsfi::LaneTime& rhs) noexcept
    {
        return (lhs.sec == rhs.sec) && (lhs.nsec == rhs.nsec);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_LANETIME_H
