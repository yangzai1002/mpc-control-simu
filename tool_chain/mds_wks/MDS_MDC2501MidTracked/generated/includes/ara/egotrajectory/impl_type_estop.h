/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_EGOTRAJECTORY_IMPL_TYPE_ESTOP_H
#define ARA_EGOTRAJECTORY_IMPL_TYPE_ESTOP_H

#include "impl_type_uint8.h"
#include "impl_type_string.h"
#include "ara/egotrajectory/impl_type_header.h"

namespace ara {
namespace egotrajectory {
struct Estop {
    ::UInt8 isStop{ 0U };
    ::String description;
    ::ara::egotrajectory::Header header;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(isStop);
        fun(description);
        fun(header);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(isStop);
        fun(description);
        fun(header);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("isStop", isStop);
        fun("description", description);
        fun("header", header);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("isStop", isStop);
        fun("description", description);
        fun("header", header);
    }

    friend bool operator==(const ::ara::egotrajectory::Estop& lhs, const ::ara::egotrajectory::Estop& rhs) noexcept
    {
        return (lhs.isStop == rhs.isStop) && (lhs.description == rhs.description) && (lhs.header == rhs.header);
    }
};
} // namespace egotrajectory
} // namespace ara


#endif // ARA_EGOTRAJECTORY_IMPL_TYPE_ESTOP_H
