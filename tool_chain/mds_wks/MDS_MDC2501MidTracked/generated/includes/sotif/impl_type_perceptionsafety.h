/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef SOTIF_IMPL_TYPE_PERCEPTIONSAFETY_H
#define SOTIF_IMPL_TYPE_PERCEPTIONSAFETY_H

#include "ara/common/impl_type_commontime.h"
#include "impl_type_uint32.h"
#include "impl_type_string.h"
#include "impl_type_uint8.h"
#include "impl_type_floatarray.h"

namespace sotif {
struct PerceptionSafety {
    ::ara::common::CommonTime timeStampFirst;
    ::UInt32 eventId{ 0U };
    ::String sensorId;
    ::UInt8 eventLevel{ 0U };
    ::UInt32 type{ 0U };
    ::String actionName;
    ::FloatArray para;
    ::ara::common::CommonTime timeStampLast;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(timeStampFirst);
        fun(eventId);
        fun(sensorId);
        fun(eventLevel);
        fun(type);
        fun(actionName);
        fun(para);
        fun(timeStampLast);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(timeStampFirst);
        fun(eventId);
        fun(sensorId);
        fun(eventLevel);
        fun(type);
        fun(actionName);
        fun(para);
        fun(timeStampLast);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("timeStampFirst", timeStampFirst);
        fun("eventId", eventId);
        fun("sensorId", sensorId);
        fun("eventLevel", eventLevel);
        fun("type", type);
        fun("actionName", actionName);
        fun("para", para);
        fun("timeStampLast", timeStampLast);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("timeStampFirst", timeStampFirst);
        fun("eventId", eventId);
        fun("sensorId", sensorId);
        fun("eventLevel", eventLevel);
        fun("type", type);
        fun("actionName", actionName);
        fun("para", para);
        fun("timeStampLast", timeStampLast);
    }

    friend bool operator==(const ::sotif::PerceptionSafety& lhs, const ::sotif::PerceptionSafety& rhs) noexcept
    {
        return (lhs.timeStampFirst == rhs.timeStampFirst) && (lhs.eventId == rhs.eventId) && (lhs.sensorId == rhs.sensorId) && (lhs.eventLevel == rhs.eventLevel) && (lhs.type == rhs.type) && (lhs.actionName == rhs.actionName) && (lhs.para == rhs.para) && (lhs.timeStampLast == rhs.timeStampLast);
    }
};
} // namespace sotif


#endif // SOTIF_IMPL_TYPE_PERCEPTIONSAFETY_H
