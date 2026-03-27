/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_EX_WDG_EXWDGLOWPOWERNOTIFYSERVICE_COMMON_H
#define MDC_EX_WDG_EXWDGLOWPOWERNOTIFYSERVICE_COMMON_H

#include "ara/com/types.h"
#include "ara/com/init_config.h"
#include "impl_type_int32.h"
#include "impl_type_uint32_t.h"
#include <cfloat>
#include <cmath>

namespace mdc {
namespace ex_wdg {
namespace methods {
namespace LowPowerNotify {
struct Output {
    ::Int32 result{ 0 };

    static bool IsPlane()
    {
        return true;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(result);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(result);
    }

    bool operator==(const Output& t) const
    {
       return (result == t.result);
    }
};
} // namespace LowPowerNotify
} // namespace methods

class ExwdgLowPowerNotifyService {
public:
    constexpr ExwdgLowPowerNotifyService() = default;
    constexpr static ara::com::ServiceIdentifierType ServiceIdentifier = ara::com::ServiceIdentifierType("/HuaweiMDC/MdcPlatformServices/PlatformServiceInterface/ExWdgServiceInterface/ExwdgLowPowerNotifyService");
    constexpr static ara::com::ServiceVersionType ServiceVersion = ara::com::ServiceVersionType("1.1");
};
} // namespace ex_wdg
} // namespace mdc

#endif // MDC_EX_WDG_EXWDGLOWPOWERNOTIFYSERVICE_COMMON_H
