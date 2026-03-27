/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFPOSEWITHCOVARIANCE_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFPOSEWITHCOVARIANCE_H

#include "ara/adsfi/impl_type_msghafpose.h"
#include "ara/adsfi/impl_type_idpdoublevec.h"

namespace ara {
namespace adsfi {
struct MsgHafPoseWithCovariance {
    ::ara::adsfi::MsgHafPose pose;
    ::ara::adsfi::IdpDoubleVec covariance;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(pose);
        fun(covariance);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(pose);
        fun(covariance);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("pose", pose);
        fun("covariance", covariance);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("pose", pose);
        fun("covariance", covariance);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafPoseWithCovariance& lhs, const ::ara::adsfi::MsgHafPoseWithCovariance& rhs) noexcept
    {
        return (lhs.pose == rhs.pose) && (lhs.covariance == rhs.covariance);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFPOSEWITHCOVARIANCE_H
