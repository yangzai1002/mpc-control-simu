/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFPOSE_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFPOSE_H

#include "ara/adsfi/impl_type_msgpoint3d.h"

namespace ara {
namespace adsfi {
struct MsgHafPose {
    ::ara::adsfi::MsgPoint3D position;
    ::ara::adsfi::MsgPoint3D orientation;

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(position);
        fun(orientation);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(position);
        fun(orientation);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("position", position);
        fun("orientation", orientation);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("position", position);
        fun("orientation", orientation);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafPose& lhs, const ::ara::adsfi::MsgHafPose& rhs) noexcept
    {
        return (lhs.position == rhs.position) && (lhs.orientation == rhs.orientation);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFPOSE_H
