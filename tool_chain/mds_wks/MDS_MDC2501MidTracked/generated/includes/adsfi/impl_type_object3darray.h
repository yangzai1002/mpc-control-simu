/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_OBJECT3DARRAY_H
#define ADSFI_IMPL_TYPE_OBJECT3DARRAY_H

#include "ara/common/impl_type_commonheader.h"
#include "adsfi/impl_type_objec3dvector.h"

namespace adsfi {
struct Object3dArray {
    ::ara::common::CommonHeader header;
    ::adsfi::Objec3dVector object;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(object);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(object);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("object", object);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("object", object);
    }

    friend bool operator==(const ::adsfi::Object3dArray& lhs, const ::adsfi::Object3dArray& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.object == rhs.object);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_OBJECT3DARRAY_H
