/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_FUSIONOBJECTARRAY_H
#define ADSFI_IMPL_TYPE_FUSIONOBJECTARRAY_H

#include "ara/common/impl_type_commonheader.h"
#include "adsfi/impl_type_fusionobjectvector.h"

namespace adsfi {
struct FusionObjectArray {
    ::ara::common::CommonHeader header;
    ::adsfi::FusionObjectVector object;

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

    friend bool operator==(const ::adsfi::FusionObjectArray& lhs, const ::adsfi::FusionObjectArray& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.object == rhs.object);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_FUSIONOBJECTARRAY_H
