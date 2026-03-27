/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPSTATUSMESSAGE_H
#define ARA_ADSFI_IMPL_TYPE_APPSTATUSMESSAGE_H

#include "impl_type_string.h"

namespace ara {
namespace adsfi {
struct AppStatusMessage {
    ::String status;
    ::String error_msg;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(status);
        fun(error_msg);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(status);
        fun(error_msg);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("status", status);
        fun("error_msg", error_msg);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("status", status);
        fun("error_msg", error_msg);
    }

    friend bool operator==(const ::ara::adsfi::AppStatusMessage& lhs, const ::ara::adsfi::AppStatusMessage& rhs) noexcept
    {
        return (lhs.status == rhs.status) && (lhs.error_msg == rhs.error_msg);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPSTATUSMESSAGE_H
