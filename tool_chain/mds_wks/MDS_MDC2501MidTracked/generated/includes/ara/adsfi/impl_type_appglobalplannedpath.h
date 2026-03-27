/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPGLOBALPLANNEDPATH_H
#define ARA_ADSFI_IMPL_TYPE_APPGLOBALPLANNEDPATH_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "ara/adsfi/impl_type_businesspointvec.h"

namespace ara {
namespace adsfi {
struct AppGlobalPlannedPath {
    ::ara::adsfi::CommonHeader head;
    ::ara::adsfi::BusinessPointVec path;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(head);
        fun(path);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(head);
        fun(path);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("head", head);
        fun("path", path);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("head", head);
        fun("path", path);
    }

    friend bool operator==(const ::ara::adsfi::AppGlobalPlannedPath& lhs, const ::ara::adsfi::AppGlobalPlannedPath& rhs) noexcept
    {
        return (lhs.head == rhs.head) && (lhs.path == rhs.path);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPGLOBALPLANNEDPATH_H
