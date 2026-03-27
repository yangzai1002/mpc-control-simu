/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_SCENE_H
#define IMPL_TYPE_SCENE_H

#include "impl_type_uint32.h"
#include "impl_type_scenendag.h"

struct Scene {
    ::UInt32 id{ 0U };
    ::ScenenDag sceneDag;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(sceneDag);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(sceneDag);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("sceneDag", sceneDag);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("sceneDag", sceneDag);
    }

    friend bool operator==(const ::Scene& lhs, const ::Scene& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.sceneDag == rhs.sceneDag);
    }
};


#endif // IMPL_TYPE_SCENE_H
