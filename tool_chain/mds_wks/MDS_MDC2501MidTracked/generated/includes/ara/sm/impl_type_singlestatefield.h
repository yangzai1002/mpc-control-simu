/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_SM_IMPL_TYPE_SINGLESTATEFIELD_H
#define ARA_SM_IMPL_TYPE_SINGLESTATEFIELD_H

#include "impl_type_string.h"
#include "impl_type_uint8_t.h"

namespace ara {
namespace sm {
struct SingleStateField {
    ::String functionGroupName;
    ::String stateName;
    ::uint8_t processResult{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(functionGroupName);
        fun(stateName);
        fun(processResult);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(functionGroupName);
        fun(stateName);
        fun(processResult);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("functionGroupName", functionGroupName);
        fun("stateName", stateName);
        fun("processResult", processResult);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("functionGroupName", functionGroupName);
        fun("stateName", stateName);
        fun("processResult", processResult);
    }

    friend bool operator==(const ::ara::sm::SingleStateField& lhs, const ::ara::sm::SingleStateField& rhs) noexcept
    {
        return (lhs.functionGroupName == rhs.functionGroupName) && (lhs.stateName == rhs.stateName) && (lhs.processResult == rhs.processResult);
    }
};
} // namespace sm
} // namespace ara


#endif // ARA_SM_IMPL_TYPE_SINGLESTATEFIELD_H
