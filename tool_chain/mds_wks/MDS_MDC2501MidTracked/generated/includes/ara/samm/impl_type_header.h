/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_SAMM_IMPL_TYPE_HEADER_H
#define ARA_SAMM_IMPL_TYPE_HEADER_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"
#include "ara/location/impl_type_time.h"
#include "impl_type_string.h"

namespace ara {
namespace samm {
struct Header {
    ::Double seq{ 0.0 };
    ::ara::location::Time stamp;
    ::String frameId;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(seq);
        fun(stamp);
        fun(frameId);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(seq);
        fun(stamp);
        fun(frameId);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("seq", seq);
        fun("stamp", stamp);
        fun("frameId", frameId);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("seq", seq);
        fun("stamp", stamp);
        fun("frameId", frameId);
    }

    friend bool operator==(const ::ara::samm::Header& lhs, const ::ara::samm::Header& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.seq - rhs.seq)) < DBL_EPSILON) && (lhs.stamp == rhs.stamp) && (lhs.frameId == rhs.frameId);
    }
};
} // namespace samm
} // namespace ara


#endif // ARA_SAMM_IMPL_TYPE_HEADER_H
