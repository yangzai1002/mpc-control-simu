/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_PREDICTIONTRAJECTORYPOINT_H
#define ARA_ADSFI_IMPL_TYPE_PREDICTIONTRAJECTORYPOINT_H

#include "impl_type_point.h"
#include "ara/adsfi/impl_type_time.h"

namespace ara {
namespace adsfi {
struct PredictionTrajectoryPoint {
    ::Point point;
    ::ara::adsfi::Time timestamp;

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(point);
        fun(timestamp);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(point);
        fun(timestamp);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("point", point);
        fun("timestamp", timestamp);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("point", point);
        fun("timestamp", timestamp);
    }

    friend bool operator==(const ::ara::adsfi::PredictionTrajectoryPoint& lhs, const ::ara::adsfi::PredictionTrajectoryPoint& rhs) noexcept
    {
        return (lhs.point == rhs.point) && (lhs.timestamp == rhs.timestamp);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_PREDICTIONTRAJECTORYPOINT_H
