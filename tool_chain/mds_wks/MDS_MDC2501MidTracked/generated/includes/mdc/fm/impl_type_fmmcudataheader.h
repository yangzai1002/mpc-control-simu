/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_IMPL_TYPE_FMMCUDATAHEADER_H
#define MDC_FM_IMPL_TYPE_FMMCUDATAHEADER_H

#include "impl_type_uint8_t.h"
#include "impl_type_uint32_t.h"
#include "impl_type_uint64_t.h"

namespace mdc {
namespace fm {
struct FmMcuDataHeader {
    ::uint8_t type{ 0U };
    ::uint8_t version{ 0U };
    ::uint32_t checkSum{ 0U };
    ::uint32_t seqId{ 0U };
    ::uint64_t sendTime{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(type);
        fun(version);
        fun(checkSum);
        fun(seqId);
        fun(sendTime);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(type);
        fun(version);
        fun(checkSum);
        fun(seqId);
        fun(sendTime);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("type", type);
        fun("version", version);
        fun("checkSum", checkSum);
        fun("seqId", seqId);
        fun("sendTime", sendTime);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("type", type);
        fun("version", version);
        fun("checkSum", checkSum);
        fun("seqId", seqId);
        fun("sendTime", sendTime);
    }

    friend bool operator==(const ::mdc::fm::FmMcuDataHeader& lhs, const ::mdc::fm::FmMcuDataHeader& rhs) noexcept
    {
        return (lhs.type == rhs.type) && (lhs.version == rhs.version) && (lhs.checkSum == rhs.checkSum) && (lhs.seqId == rhs.seqId) && (lhs.sendTime == rhs.sendTime);
    }
};
} // namespace fm
} // namespace mdc


#endif // MDC_FM_IMPL_TYPE_FMMCUDATAHEADER_H
