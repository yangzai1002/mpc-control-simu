/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_SENSORRADARPOINTCLOUD_H
#define ARA_ADSFI_IMPL_TYPE_SENSORRADARPOINTCLOUD_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "ara/adsfi/impl_type_sensorradarpoint4dvec.h"

namespace ara {
namespace adsfi {
struct SensorRadarPointCloud {
    ::ara::adsfi::CommonHeader header;
    ::ara::adsfi::SensorRadarPoint4DVec objs;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(objs);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(objs);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("objs", objs);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("objs", objs);
    }

    friend bool operator==(const ::ara::adsfi::SensorRadarPointCloud& lhs, const ::ara::adsfi::SensorRadarPointCloud& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.objs == rhs.objs);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_SENSORRADARPOINTCLOUD_H
