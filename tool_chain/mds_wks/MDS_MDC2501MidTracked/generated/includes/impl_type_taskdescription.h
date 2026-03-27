/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_TASKDESCRIPTION_H
#define IMPL_TYPE_TASKDESCRIPTION_H

#include "impl_type_string.h"
#include "impl_type_jobvector.h"

struct TaskDescription {
    ::String taskName;
    ::JobVector jobList;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(taskName);
        fun(jobList);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(taskName);
        fun(jobList);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("taskName", taskName);
        fun("jobList", jobList);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("taskName", taskName);
        fun("jobList", jobList);
    }

    friend bool operator==(const ::TaskDescription& lhs, const ::TaskDescription& rhs) noexcept
    {
        return (lhs.taskName == rhs.taskName) && (lhs.jobList == rhs.jobList);
    }
};


#endif // IMPL_TYPE_TASKDESCRIPTION_H
