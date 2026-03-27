/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_TRAFFICLIGHT_IMPL_TYPE_TRAFFICLIGHTARRAY_H
#define ARA_TRAFFICLIGHT_IMPL_TYPE_TRAFFICLIGHTARRAY_H

#include "ara/trafficlight/impl_type_trafficlightheader.h"
#include "ara/trafficlight/impl_type_trafficlightvector.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace trafficlight {
struct TrafficLightArray {
    ::ara::trafficlight::TrafficLightHeader header;
    ::ara::trafficlight::TrafficLightVector object_list;
    ::int32_t count{ 0 };
    bool is_valid{ false };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(object_list);
        fun(count);
        fun(is_valid);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(object_list);
        fun(count);
        fun(is_valid);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("object_list", object_list);
        fun("count", count);
        fun("is_valid", is_valid);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("object_list", object_list);
        fun("count", count);
        fun("is_valid", is_valid);
    }

    friend bool operator==(const ::ara::trafficlight::TrafficLightArray& lhs, const ::ara::trafficlight::TrafficLightArray& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.object_list == rhs.object_list) && (lhs.count == rhs.count) && (lhs.is_valid == rhs.is_valid);
    }
};
} // namespace trafficlight
} // namespace ara


#endif // ARA_TRAFFICLIGHT_IMPL_TYPE_TRAFFICLIGHTARRAY_H
