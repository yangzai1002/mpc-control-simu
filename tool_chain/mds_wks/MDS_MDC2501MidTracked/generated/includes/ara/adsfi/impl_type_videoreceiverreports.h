/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VIDEORECEIVERREPORTS_H
#define ARA_ADSFI_IMPL_TYPE_VIDEORECEIVERREPORTS_H

#include "ara/adsfi/impl_type_videoreceiverreportvec.h"

namespace ara {
namespace adsfi {
struct VideoReceiverReports {
    ::ara::adsfi::VideoReceiverReportVec reports;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(reports);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(reports);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("reports", reports);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("reports", reports);
    }

    friend bool operator==(const ::ara::adsfi::VideoReceiverReports& lhs, const ::ara::adsfi::VideoReceiverReports& rhs) noexcept
    {
        return (lhs.reports == rhs.reports);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VIDEORECEIVERREPORTS_H
