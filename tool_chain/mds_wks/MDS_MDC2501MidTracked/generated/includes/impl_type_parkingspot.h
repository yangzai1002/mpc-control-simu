/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef IMPL_TYPE_PARKINGSPOT_H
#define IMPL_TYPE_PARKINGSPOT_H

#include <cfloat>
#include <cmath>
#include "ara/common/impl_type_commonheader.h"
#include "impl_type_uint32.h"
#include "impl_type_uint8.h"
#include "impl_type_float.h"
#include "impl_type_pointarray.h"
#include "impl_type_uint8array.h"

struct ParkingSpot {
    ::ara::common::CommonHeader header;
    ::UInt32 id{ 0U };
    ::UInt8 parking_class{ 0U };
    ::Float class_confidence{ 0.0F };
    ::PointArray parking_spot_ego;
    ::PointArray parking_spot_enu;
    ::Uint8Array entry;
    ::UInt8 parking_spot_type{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(id);
        fun(parking_class);
        fun(class_confidence);
        fun(parking_spot_ego);
        fun(parking_spot_enu);
        fun(entry);
        fun(parking_spot_type);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(id);
        fun(parking_class);
        fun(class_confidence);
        fun(parking_spot_ego);
        fun(parking_spot_enu);
        fun(entry);
        fun(parking_spot_type);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("id", id);
        fun("parking_class", parking_class);
        fun("class_confidence", class_confidence);
        fun("parking_spot_ego", parking_spot_ego);
        fun("parking_spot_enu", parking_spot_enu);
        fun("entry", entry);
        fun("parking_spot_type", parking_spot_type);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("id", id);
        fun("parking_class", parking_class);
        fun("class_confidence", class_confidence);
        fun("parking_spot_ego", parking_spot_ego);
        fun("parking_spot_enu", parking_spot_enu);
        fun("entry", entry);
        fun("parking_spot_type", parking_spot_type);
    }

    friend bool operator==(const ::ParkingSpot& lhs, const ::ParkingSpot& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.id == rhs.id) && (lhs.parking_class == rhs.parking_class) && (fabs(static_cast<double>(lhs.class_confidence - rhs.class_confidence)) < DBL_EPSILON) && (lhs.parking_spot_ego == rhs.parking_spot_ego) && (lhs.parking_spot_enu == rhs.parking_spot_enu) && (lhs.entry == rhs.entry) && (lhs.parking_spot_type == rhs.parking_spot_type);
    }
};


#endif // IMPL_TYPE_PARKINGSPOT_H
