/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_OBJECTHEADER_H
#define ARA_ADSFI_IMPL_TYPE_OBJECTHEADER_H

#include "impl_type_uint32.h"
#include "ara/adsfi/impl_type_time.h"
#include "impl_type_string.h"

namespace ara {
namespace adsfi {
struct ObjectHeader {
    ::UInt32 seq{ 0U };
    ::ara::adsfi::Time stamp;
    ::String frameId;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(seq);
        fun(stamp);
        fun(frameId);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(seq);
        fun(stamp);
        fun(frameId);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("seq", seq);
        fun("stamp", stamp);
        fun("frameId", frameId);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("seq", seq);
        fun("stamp", stamp);
        fun("frameId", frameId);
    }

    friend bool operator==(const ::ara::adsfi::ObjectHeader& lhs, const ::ara::adsfi::ObjectHeader& rhs) noexcept
    {
        return (lhs.seq == rhs.seq) && (lhs.stamp == rhs.stamp) && (lhs.frameId == rhs.frameId);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_OBJECTHEADER_H
