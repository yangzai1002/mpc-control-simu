/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_IMPL_TYPE_FMMCUDATARETINFO_H
#define MDC_FM_IMPL_TYPE_FMMCUDATARETINFO_H

#include "impl_type_uint8_t.h"
#include "impl_type_uint32_t.h"

namespace mdc {
namespace fm {
struct FmMcuDataRetInfo {
    ::uint8_t type{ 0U };
    ::uint32_t seqId{ 0U };
    ::uint32_t retCode{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(type);
        fun(seqId);
        fun(retCode);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(type);
        fun(seqId);
        fun(retCode);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("type", type);
        fun("seqId", seqId);
        fun("retCode", retCode);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("type", type);
        fun("seqId", seqId);
        fun("retCode", retCode);
    }

    friend bool operator==(const ::mdc::fm::FmMcuDataRetInfo& lhs, const ::mdc::fm::FmMcuDataRetInfo& rhs) noexcept
    {
        return (lhs.type == rhs.type) && (lhs.seqId == rhs.seqId) && (lhs.retCode == rhs.retCode);
    }
};
} // namespace fm
} // namespace mdc


#endif // MDC_FM_IMPL_TYPE_FMMCUDATARETINFO_H
