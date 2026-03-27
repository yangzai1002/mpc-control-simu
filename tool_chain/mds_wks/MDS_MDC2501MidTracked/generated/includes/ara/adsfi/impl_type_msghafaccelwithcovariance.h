/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFACCELWITHCOVARIANCE_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFACCELWITHCOVARIANCE_H

#include "ara/adsfi/impl_type_msghafaccel.h"
#include "ara/adsfi/impl_type_idpdoublevec.h"

namespace ara {
namespace adsfi {
struct MsgHafAccelWithCovariance {
    ::ara::adsfi::MsgHafAccel accel;
    ::ara::adsfi::IdpDoubleVec covariance;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(accel);
        fun(covariance);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(accel);
        fun(covariance);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("accel", accel);
        fun("covariance", covariance);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("accel", accel);
        fun("covariance", covariance);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafAccelWithCovariance& lhs, const ::ara::adsfi::MsgHafAccelWithCovariance& rhs) noexcept
    {
        return (lhs.accel == rhs.accel) && (lhs.covariance == rhs.covariance);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFACCELWITHCOVARIANCE_H
