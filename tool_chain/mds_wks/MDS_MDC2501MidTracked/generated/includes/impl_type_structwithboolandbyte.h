/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_STRUCTWITHBOOLANDBYTE_H
#define IMPL_TYPE_STRUCTWITHBOOLANDBYTE_H

#include "impl_type_boolean.h"
#include "impl_type_uint8.h"

struct StructWithBoolAndByte {
    ::Boolean BoolData{ false };
    ::UInt8 ByteData{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(BoolData);
        fun(ByteData);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(BoolData);
        fun(ByteData);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("BoolData", BoolData);
        fun("ByteData", ByteData);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("BoolData", BoolData);
        fun("ByteData", ByteData);
    }

    friend bool operator==(const ::StructWithBoolAndByte& lhs, const ::StructWithBoolAndByte& rhs) noexcept
    {
        return (lhs.BoolData == rhs.BoolData) && (lhs.ByteData == rhs.ByteData);
    }
};


#endif // IMPL_TYPE_STRUCTWITHBOOLANDBYTE_H
