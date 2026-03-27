/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPSTATUS_H
#define ARA_ADSFI_IMPL_TYPE_APPSTATUS_H

#include "ara/adsfi/impl_type_appstatusmessage.h"

namespace ara {
namespace adsfi {
struct AppStatus {
    ::ara::adsfi::AppStatusMessage record_status_track;
    ::ara::adsfi::AppStatusMessage record_status_fence;
    ::ara::adsfi::AppStatusMessage record_status_map;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(record_status_track);
        fun(record_status_fence);
        fun(record_status_map);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(record_status_track);
        fun(record_status_fence);
        fun(record_status_map);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("record_status_track", record_status_track);
        fun("record_status_fence", record_status_fence);
        fun("record_status_map", record_status_map);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("record_status_track", record_status_track);
        fun("record_status_fence", record_status_fence);
        fun("record_status_map", record_status_map);
    }

    friend bool operator==(const ::ara::adsfi::AppStatus& lhs, const ::ara::adsfi::AppStatus& rhs) noexcept
    {
        return (lhs.record_status_track == rhs.record_status_track) && (lhs.record_status_fence == rhs.record_status_fence) && (lhs.record_status_map == rhs.record_status_map);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPSTATUS_H
