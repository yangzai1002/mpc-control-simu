/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_VO_IMPL_TYPE_CHNINFO_H
#define MDC_VO_IMPL_TYPE_CHNINFO_H

#include "impl_type_uint8_t.h"
#include "mdc/vo/impl_type_slotid.h"
#include "impl_type_string.h"
#include "mdc/vo/impl_type_videoparam.h"

namespace mdc {
namespace vo {
struct ChnInfo {
    ::uint8_t chnId{ 0U };
    ::mdc::vo::SlotId slotId{ ::mdc::vo::SlotId::D4 };
    bool isEnabled{ false };
    ::String function;
    ::mdc::vo::VideoParam videoParam;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(chnId);
        fun(slotId);
        fun(isEnabled);
        fun(function);
        fun(videoParam);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(chnId);
        fun(slotId);
        fun(isEnabled);
        fun(function);
        fun(videoParam);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("chnId", chnId);
        fun("slotId", slotId);
        fun("isEnabled", isEnabled);
        fun("function", function);
        fun("videoParam", videoParam);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("chnId", chnId);
        fun("slotId", slotId);
        fun("isEnabled", isEnabled);
        fun("function", function);
        fun("videoParam", videoParam);
    }

    friend bool operator==(const ::mdc::vo::ChnInfo& lhs, const ::mdc::vo::ChnInfo& rhs) noexcept
    {
        return (lhs.chnId == rhs.chnId) && (lhs.slotId == rhs.slotId) && (lhs.isEnabled == rhs.isEnabled) && (lhs.function == rhs.function) && (lhs.videoParam == rhs.videoParam);
    }
};
} // namespace vo
} // namespace mdc


#endif // MDC_VO_IMPL_TYPE_CHNINFO_H
