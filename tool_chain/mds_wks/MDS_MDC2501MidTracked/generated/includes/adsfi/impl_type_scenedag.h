/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_SCENEDAG_H
#define ADSFI_IMPL_TYPE_SCENEDAG_H

#include "impl_type_uint32.h"
#include "adsfi/impl_type_taskvector.h"
#include "impl_type_string.h"

namespace adsfi {
struct SceneDag {
    ::UInt32 id{ 0U };
    ::adsfi::TaskVector tasks;
    ::String name;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(tasks);
        fun(name);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(tasks);
        fun(name);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("tasks", tasks);
        fun("name", name);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("tasks", tasks);
        fun("name", name);
    }

    friend bool operator==(const ::adsfi::SceneDag& lhs, const ::adsfi::SceneDag& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.tasks == rhs.tasks) && (lhs.name == rhs.name);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_SCENEDAG_H
