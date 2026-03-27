/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_SM_IMPL_TYPE_PLATFORMSTATEMSG_H
#define ARA_SM_IMPL_TYPE_PLATFORMSTATEMSG_H

#include "impl_type_uint8_t.h"

namespace ara {
namespace sm {
struct PlatformStateMsg {
    ::uint8_t platformState{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(platformState);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(platformState);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("platformState", platformState);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("platformState", platformState);
    }

    friend bool operator==(const ::ara::sm::PlatformStateMsg& lhs, const ::ara::sm::PlatformStateMsg& rhs) noexcept
    {
        return (lhs.platformState == rhs.platformState);
    }
};
} // namespace sm
} // namespace ara


#endif // ARA_SM_IMPL_TYPE_PLATFORMSTATEMSG_H
