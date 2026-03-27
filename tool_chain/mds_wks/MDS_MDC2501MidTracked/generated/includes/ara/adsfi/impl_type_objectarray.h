/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_OBJECTARRAY_H
#define ARA_ADSFI_IMPL_TYPE_OBJECTARRAY_H

#include "ara/common/impl_type_commonheader.h"
#include "impl_type_uint8.h"
#include "ara/adsfi/impl_type_objectvector.h"

namespace ara {
namespace adsfi {
struct ObjectArray {
    ::ara::common::CommonHeader header;
    ::UInt8 package_type{ 0U };
    ::ara::adsfi::ObjectVector object_list;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(package_type);
        fun(object_list);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(package_type);
        fun(object_list);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("package_type", package_type);
        fun("object_list", object_list);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("package_type", package_type);
        fun("object_list", object_list);
    }

    friend bool operator==(const ::ara::adsfi::ObjectArray& lhs, const ::ara::adsfi::ObjectArray& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.package_type == rhs.package_type) && (lhs.object_list == rhs.object_list);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_OBJECTARRAY_H
