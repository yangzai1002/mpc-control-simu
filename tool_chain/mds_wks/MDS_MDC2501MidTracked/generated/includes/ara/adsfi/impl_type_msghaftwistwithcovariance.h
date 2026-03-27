/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFTWISTWITHCOVARIANCE_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFTWISTWITHCOVARIANCE_H

#include "ara/adsfi/impl_type_msghaftwist.h"
#include "ara/adsfi/impl_type_idpdoublevec.h"

namespace ara {
namespace adsfi {
struct MsgHafTwistWithCovariance {
    ::ara::adsfi::MsgHafTwist twist;
    ::ara::adsfi::IdpDoubleVec covariance;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(twist);
        fun(covariance);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(twist);
        fun(covariance);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("twist", twist);
        fun("covariance", covariance);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("twist", twist);
        fun("covariance", covariance);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafTwistWithCovariance& lhs, const ::ara::adsfi::MsgHafTwistWithCovariance& rhs) noexcept
    {
        return (lhs.twist == rhs.twist) && (lhs.covariance == rhs.covariance);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFTWISTWITHCOVARIANCE_H
