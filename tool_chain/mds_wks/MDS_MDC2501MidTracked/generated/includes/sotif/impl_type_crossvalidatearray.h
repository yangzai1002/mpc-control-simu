/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef SOTIF_IMPL_TYPE_CROSSVALIDATEARRAY_H
#define SOTIF_IMPL_TYPE_CROSSVALIDATEARRAY_H

#include "ara/common/impl_type_commonheader.h"
#include "sotif/impl_type_crossvalidatevector.h"

namespace sotif {
struct CrossValidateArray {
    ::ara::common::CommonHeader header;
    ::sotif::CrossValidateVector result;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(result);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(result);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("result", result);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("result", result);
    }

    friend bool operator==(const ::sotif::CrossValidateArray& lhs, const ::sotif::CrossValidateArray& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.result == rhs.result);
    }
};
} // namespace sotif


#endif // SOTIF_IMPL_TYPE_CROSSVALIDATEARRAY_H
