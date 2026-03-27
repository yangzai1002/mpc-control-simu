/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_TASK_H
#define ADSFI_IMPL_TYPE_TASK_H

#include "impl_type_string.h"
#include "adsfi/impl_type_jobvector.h"

namespace adsfi {
struct Task {
    ::String name;
    ::adsfi::JobVector jobList;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(name);
        fun(jobList);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(name);
        fun(jobList);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("name", name);
        fun("jobList", jobList);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("name", name);
        fun("jobList", jobList);
    }

    friend bool operator==(const ::adsfi::Task& lhs, const ::adsfi::Task& rhs) noexcept
    {
        return (lhs.name == rhs.name) && (lhs.jobList == rhs.jobList);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_TASK_H
