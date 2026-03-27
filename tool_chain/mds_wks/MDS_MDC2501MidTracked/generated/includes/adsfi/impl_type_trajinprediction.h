/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_IMPL_TYPE_TRAJINPREDICTION_H
#define ADSFI_IMPL_TYPE_TRAJINPREDICTION_H

#include <cfloat>
#include <cmath>
#include "impl_type_double.h"
#include "adsfi/impl_type_trajpointvector.h"

namespace adsfi {
struct TrajInPrediction {
    ::Double probability{ 0.0 };
    ::adsfi::TrajPointVector trajectoryPoints;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(probability);
        fun(trajectoryPoints);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(probability);
        fun(trajectoryPoints);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("probability", probability);
        fun("trajectoryPoints", trajectoryPoints);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("probability", probability);
        fun("trajectoryPoints", trajectoryPoints);
    }

    friend bool operator==(const ::adsfi::TrajInPrediction& lhs, const ::adsfi::TrajInPrediction& rhs) noexcept
    {
        return (fabs(static_cast<double>(lhs.probability - rhs.probability)) < DBL_EPSILON) && (lhs.trajectoryPoints == rhs.trajectoryPoints);
    }
};
} // namespace adsfi


#endif // ADSFI_IMPL_TYPE_TRAJINPREDICTION_H
