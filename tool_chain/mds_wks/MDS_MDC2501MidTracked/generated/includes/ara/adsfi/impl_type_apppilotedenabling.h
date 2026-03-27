/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPPILOTEDENABLING_H
#define ARA_ADSFI_IMPL_TYPE_APPPILOTEDENABLING_H

#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct AppPilotedEnabling {
    ::int32_t enabling{ 0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(enabling);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(enabling);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("enabling", enabling);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("enabling", enabling);
    }

    friend bool operator==(const ::ara::adsfi::AppPilotedEnabling& lhs, const ::ara::adsfi::AppPilotedEnabling& rhs) noexcept
    {
        return (lhs.enabling == rhs.enabling);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPPILOTEDENABLING_H
