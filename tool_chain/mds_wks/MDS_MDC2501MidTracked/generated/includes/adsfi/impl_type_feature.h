/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_FEATURE_H
#define ADSFI_IMPL_TYPE_FEATURE_H

#include "impl_type_uint32.h"
#include "impl_type_string.h"

namespace adsfi {
struct Feature {
    ::UInt32 id{ 0U };
    ::String name;
    ::String description;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(id);
        fun(name);
        fun(description);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(id);
        fun(name);
        fun(description);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("id", id);
        fun("name", name);
        fun("description", description);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("id", id);
        fun("name", name);
        fun("description", description);
    }

    friend bool operator==(const ::adsfi::Feature& lhs, const ::adsfi::Feature& rhs) noexcept
    {
        return (lhs.id == rhs.id) && (lhs.name == rhs.name) && (lhs.description == rhs.description);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_FEATURE_H
