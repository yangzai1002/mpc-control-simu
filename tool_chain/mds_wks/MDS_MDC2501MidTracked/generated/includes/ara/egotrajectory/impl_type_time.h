/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_EGOTRAJECTORY_IMPL_TYPE_TIME_H
#define ARA_EGOTRAJECTORY_IMPL_TYPE_TIME_H

#include "impl_type_uint32.h"

namespace ara {
namespace egotrajectory {
struct Time {
    ::UInt32 sec{ 0U };
    ::UInt32 nsec{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(sec);
        fun(nsec);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(sec);
        fun(nsec);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("sec", sec);
        fun("nsec", nsec);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("sec", sec);
        fun("nsec", nsec);
    }

    friend bool operator==(const ::ara::egotrajectory::Time& lhs, const ::ara::egotrajectory::Time& rhs) noexcept
    {
        return (lhs.sec == rhs.sec) && (lhs.nsec == rhs.nsec);
    }
};
} // namespace egotrajectory
} // namespace ara


#endif // ARA_EGOTRAJECTORY_IMPL_TYPE_TIME_H
