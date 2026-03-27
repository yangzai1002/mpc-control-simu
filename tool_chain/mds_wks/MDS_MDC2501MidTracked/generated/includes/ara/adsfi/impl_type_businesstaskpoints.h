/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_BUSINESSTASKPOINTS_H
#define ARA_ADSFI_IMPL_TYPE_BUSINESSTASKPOINTS_H

#include "ara/adsfi/impl_type_businesstaskpointvec.h"

namespace ara {
namespace adsfi {
struct BusinessTaskPoints {
    ::ara::adsfi::BusinessTaskPointVec task_points;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(task_points);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(task_points);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("task_points", task_points);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("task_points", task_points);
    }

    friend bool operator==(const ::ara::adsfi::BusinessTaskPoints& lhs, const ::ara::adsfi::BusinessTaskPoints& rhs) noexcept
    {
        return (lhs.task_points == rhs.task_points);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_BUSINESSTASKPOINTS_H
