/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_TRAFFICSIGNALARRAY_H
#define ADSFI_IMPL_TYPE_TRAFFICSIGNALARRAY_H

#include "ara/common/impl_type_commonheader.h"
#include "adsfi/impl_type_trafficsignalvector.h"

namespace adsfi {
struct TrafficSignalArray {
    ::ara::common::CommonHeader header;
    ::adsfi::TrafficSignalVector tlObject;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(tlObject);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(tlObject);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("tlObject", tlObject);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("tlObject", tlObject);
    }

    friend bool operator==(const ::adsfi::TrafficSignalArray& lhs, const ::adsfi::TrafficSignalArray& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.tlObject == rhs.tlObject);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_TRAFFICSIGNALARRAY_H
