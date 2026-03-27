/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_SELECTPOINTS_H
#define ADSFI_IMPL_TYPE_SELECTPOINTS_H

#include "ara/common/impl_type_commonheader.h"
#include "impl_type_pointarray.h"

namespace adsfi {
struct SelectPoints {
    ::ara::common::CommonHeader header;
    ::PointArray points;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(points);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(points);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("points", points);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("points", points);
    }

    friend bool operator==(const ::adsfi::SelectPoints& lhs, const ::adsfi::SelectPoints& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.points == rhs.points);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_SELECTPOINTS_H
