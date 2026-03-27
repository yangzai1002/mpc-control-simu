/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_VO_IMPL_TYPE_RESOLUTION_H
#define MDC_VO_IMPL_TYPE_RESOLUTION_H

#include "impl_type_uint32_t.h"

namespace mdc {
namespace vo {
struct Resolution {
    ::uint32_t width{ 0U };
    ::uint32_t height{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(width);
        fun(height);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(width);
        fun(height);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("width", width);
        fun("height", height);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("width", width);
        fun("height", height);
    }

    friend bool operator==(const ::mdc::vo::Resolution& lhs, const ::mdc::vo::Resolution& rhs) noexcept
    {
        return (lhs.width == rhs.width) && (lhs.height == rhs.height);
    }
};
} // namespace vo
} // namespace mdc


#endif // MDC_VO_IMPL_TYPE_RESOLUTION_H
