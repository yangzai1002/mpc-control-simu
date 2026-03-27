/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_SCENENDAG_H
#define IMPL_TYPE_SCENENDAG_H

#include "impl_type_uint32.h"
#include "impl_type_taskvector.h"
#include "impl_type_string.h"

struct ScenenDag {
    ::UInt32 sceneId{ 0U };
    ::TaskVector tasks;
    ::String sceneName;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(sceneId);
        fun(tasks);
        fun(sceneName);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(sceneId);
        fun(tasks);
        fun(sceneName);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("sceneId", sceneId);
        fun("tasks", tasks);
        fun("sceneName", sceneName);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("sceneId", sceneId);
        fun("tasks", tasks);
        fun("sceneName", sceneName);
    }

    friend bool operator==(const ::ScenenDag& lhs, const ::ScenenDag& rhs) noexcept
    {
        return (lhs.sceneId == rhs.sceneId) && (lhs.tasks == rhs.tasks) && (lhs.sceneName == rhs.sceneName);
    }
};


#endif // IMPL_TYPE_SCENENDAG_H
