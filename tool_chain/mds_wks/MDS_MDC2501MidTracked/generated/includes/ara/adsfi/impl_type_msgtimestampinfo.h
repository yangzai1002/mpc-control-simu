/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGTIMESTAMPINFO_H
#define ARA_ADSFI_IMPL_TYPE_MSGTIMESTAMPINFO_H

#include "impl_type_uint16_t.h"
#include "impl_type_int64_t.h"

namespace ara {
namespace adsfi {
struct MsgTimeStampInfo {
    ::uint16_t timeStampStatus{ 0U };
    ::int64_t timeStamp{ 0 };
    ::int64_t autoSarTimeStamp{ 0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(timeStampStatus);
        fun(timeStamp);
        fun(autoSarTimeStamp);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(timeStampStatus);
        fun(timeStamp);
        fun(autoSarTimeStamp);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("timeStampStatus", timeStampStatus);
        fun("timeStamp", timeStamp);
        fun("autoSarTimeStamp", autoSarTimeStamp);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("timeStampStatus", timeStampStatus);
        fun("timeStamp", timeStamp);
        fun("autoSarTimeStamp", autoSarTimeStamp);
    }

    friend bool operator==(const ::ara::adsfi::MsgTimeStampInfo& lhs, const ::ara::adsfi::MsgTimeStampInfo& rhs) noexcept
    {
        return (lhs.timeStampStatus == rhs.timeStampStatus) && (lhs.timeStamp == rhs.timeStamp) && (lhs.autoSarTimeStamp == rhs.autoSarTimeStamp);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGTIMESTAMPINFO_H
