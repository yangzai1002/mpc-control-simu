/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_SINGLESCENE_H
#define ADSFI_IMPL_TYPE_SINGLESCENE_H

#include "impl_type_uint32.h"
#include "adsfi/impl_type_scenedag.h"

namespace adsfi {
struct SingleScene {
    ::UInt32 id{ 0U };
    ::adsfi::SceneDag dag;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(dag);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(dag);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("dag", dag);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("dag", dag);
    }

    friend bool operator==(const ::adsfi::SingleScene& lhs, const ::adsfi::SingleScene& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.dag == rhs.dag);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_SINGLESCENE_H
