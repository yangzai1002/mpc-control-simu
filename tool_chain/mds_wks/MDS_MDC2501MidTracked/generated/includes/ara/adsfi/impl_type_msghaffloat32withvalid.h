/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFFLOAT32WITHVALID_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFFLOAT32WITHVALID_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct MsgHafFloat32WithValid {
    float value{ 0.0F };
    ::uint8_t confidence{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(value);
        fun(confidence);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(value);
        fun(confidence);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("value", value);
        fun("confidence", confidence);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("value", value);
        fun("confidence", confidence);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafFloat32WithValid& lhs, const ::ara::adsfi::MsgHafFloat32WithValid& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.value - rhs.value)) < DBL_EPSILON) && (lhs.confidence == rhs.confidence);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFFLOAT32WITHVALID_H
