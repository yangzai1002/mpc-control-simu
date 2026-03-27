/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef SOTIF_IMPL_TYPE_DECISIONSAFETY_H
#define SOTIF_IMPL_TYPE_DECISIONSAFETY_H

#include "ara/common/impl_type_commonheader.h"
#include "impl_type_uint32.h"
#include "impl_type_uint8.h"
#include "impl_type_string.h"
#include "impl_type_floatarray.h"

namespace sotif {
struct DecisionSafety {
    ::ara::common::CommonHeader header;
    ::UInt32 eventID{ 0U };
    ::UInt8 eventLevel{ 0U };
    ::UInt32 type{ 0U };
    ::String actionName;
    ::FloatArray para;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(eventID);
        fun(eventLevel);
        fun(type);
        fun(actionName);
        fun(para);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(eventID);
        fun(eventLevel);
        fun(type);
        fun(actionName);
        fun(para);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("eventID", eventID);
        fun("eventLevel", eventLevel);
        fun("type", type);
        fun("actionName", actionName);
        fun("para", para);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("eventID", eventID);
        fun("eventLevel", eventLevel);
        fun("type", type);
        fun("actionName", actionName);
        fun("para", para);
    }

    friend bool operator==(const ::sotif::DecisionSafety& lhs, const ::sotif::DecisionSafety& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.eventID == rhs.eventID) && (lhs.eventLevel == rhs.eventLevel) && (lhs.type == rhs.type) && (lhs.actionName == rhs.actionName) && (lhs.para == rhs.para);
    }
};
} // namespace sotif


#endif // SOTIF_IMPL_TYPE_DECISIONSAFETY_H
