/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_TRAJPREDICTARRAY_H
#define ADSFI_IMPL_TYPE_TRAJPREDICTARRAY_H

#include "ara/common/impl_type_commonheader.h"
#include "adsfi/impl_type_predictobjectvector.h"
#include "ara/common/impl_type_commontime.h"
#include "impl_type_uint8.h"
#include "impl_type_uint16.h"

namespace adsfi {
struct TrajPredictArray {
    ::ara::common::CommonHeader header;
    ::adsfi::PredictObjectVector object;
    ::ara::common::CommonTime startTime;
    ::ara::common::CommonTime endTime;
    ::UInt8 changeOriginFlag{ 0U };
    ::UInt8 selfIntent{ 0U };
    ::UInt16 scenario{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(object);
        fun(startTime);
        fun(endTime);
        fun(changeOriginFlag);
        fun(selfIntent);
        fun(scenario);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(object);
        fun(startTime);
        fun(endTime);
        fun(changeOriginFlag);
        fun(selfIntent);
        fun(scenario);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("object", object);
        fun("startTime", startTime);
        fun("endTime", endTime);
        fun("changeOriginFlag", changeOriginFlag);
        fun("selfIntent", selfIntent);
        fun("scenario", scenario);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("object", object);
        fun("startTime", startTime);
        fun("endTime", endTime);
        fun("changeOriginFlag", changeOriginFlag);
        fun("selfIntent", selfIntent);
        fun("scenario", scenario);
    }

    friend bool operator==(const ::adsfi::TrajPredictArray& lhs, const ::adsfi::TrajPredictArray& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.object == rhs.object) && (lhs.startTime == rhs.startTime) && (lhs.endTime == rhs.endTime) && (lhs.changeOriginFlag == rhs.changeOriginFlag) && (lhs.selfIntent == rhs.selfIntent) && (lhs.scenario == rhs.scenario);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_TRAJPREDICTARRAY_H
