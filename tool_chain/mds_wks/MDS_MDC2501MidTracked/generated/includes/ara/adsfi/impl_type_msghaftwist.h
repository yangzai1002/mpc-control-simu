/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFTWIST_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFTWIST_H

#include "ara/adsfi/impl_type_msgpoint3d.h"

namespace ara {
namespace adsfi {
struct MsgHafTwist {
    ::ara::adsfi::MsgPoint3D linear;
    ::ara::adsfi::MsgPoint3D angular;

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(linear);
        fun(angular);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(linear);
        fun(angular);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("linear", linear);
        fun("angular", angular);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("linear", linear);
        fun("angular", angular);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafTwist& lhs, const ::ara::adsfi::MsgHafTwist& rhs) noexcept
    {
        return (lhs.linear == rhs.linear) && (lhs.angular == rhs.angular);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFTWIST_H
