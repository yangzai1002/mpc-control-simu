/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_LPM_IMPL_TYPE_MDCWAKEUPEVENTDATA_H
#define MDC_LPM_IMPL_TYPE_MDCWAKEUPEVENTDATA_H

#include "impl_type_int32_t.h"

namespace mdc {
namespace lpm {
struct MdcWakeupEventData {
    ::int32_t isWakeup{ 0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(isWakeup);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(isWakeup);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("isWakeup", isWakeup);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("isWakeup", isWakeup);
    }

    friend bool operator==(const ::mdc::lpm::MdcWakeupEventData& lhs, const ::mdc::lpm::MdcWakeupEventData& rhs) noexcept
    {
        return (lhs.isWakeup == rhs.isWakeup);
    }
};
} // namespace lpm
} // namespace mdc


#endif // MDC_LPM_IMPL_TYPE_MDCWAKEUPEVENTDATA_H
