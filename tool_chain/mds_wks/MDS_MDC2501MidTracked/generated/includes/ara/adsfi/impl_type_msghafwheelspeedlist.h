/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFWHEELSPEEDLIST_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFWHEELSPEEDLIST_H

#include "ara/adsfi/impl_type_msghafheader.h"
#include "ara/adsfi/impl_type_msghafwheelspeedvec.h"

namespace ara {
namespace adsfi {
struct MsgHafWheelSpeedList {
    ::ara::adsfi::MsgHafHeader header;
    ::ara::adsfi::MsgHafWheelSpeedVec wheel_speed_vec;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(wheel_speed_vec);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(wheel_speed_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("wheel_speed_vec", wheel_speed_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("wheel_speed_vec", wheel_speed_vec);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafWheelSpeedList& lhs, const ::ara::adsfi::MsgHafWheelSpeedList& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.wheel_speed_vec == rhs.wheel_speed_vec);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFWHEELSPEEDLIST_H
