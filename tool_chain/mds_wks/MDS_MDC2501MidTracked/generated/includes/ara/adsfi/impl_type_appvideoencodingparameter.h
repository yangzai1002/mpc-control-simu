/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPVIDEOENCODINGPARAMETER_H
#define ARA_ADSFI_IMPL_TYPE_APPVIDEOENCODINGPARAMETER_H

#include <cfloat>
#include <cmath>
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct AppVideoEncodingParameter {
    ::int32_t adaptive_mode{ 0 };
    ::int32_t rate_control_mode{ 0 };
    ::int32_t quality{ 0 };
    ::int32_t bps{ 0 };
    ::int32_t fps{ 0 };
    ::int32_t encoding_gop{ 0 };
    double width{ 0.0 };
    double height{ 0.0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(adaptive_mode);
        fun(rate_control_mode);
        fun(quality);
        fun(bps);
        fun(fps);
        fun(encoding_gop);
        fun(width);
        fun(height);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(adaptive_mode);
        fun(rate_control_mode);
        fun(quality);
        fun(bps);
        fun(fps);
        fun(encoding_gop);
        fun(width);
        fun(height);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("adaptive_mode", adaptive_mode);
        fun("rate_control_mode", rate_control_mode);
        fun("quality", quality);
        fun("bps", bps);
        fun("fps", fps);
        fun("encoding_gop", encoding_gop);
        fun("width", width);
        fun("height", height);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("adaptive_mode", adaptive_mode);
        fun("rate_control_mode", rate_control_mode);
        fun("quality", quality);
        fun("bps", bps);
        fun("fps", fps);
        fun("encoding_gop", encoding_gop);
        fun("width", width);
        fun("height", height);
    }

    friend bool operator==(const ::ara::adsfi::AppVideoEncodingParameter& lhs, const ::ara::adsfi::AppVideoEncodingParameter& rhs) noexcept
    {
        return (lhs.adaptive_mode == rhs.adaptive_mode) && (lhs.rate_control_mode == rhs.rate_control_mode) && (lhs.quality == rhs.quality) && (lhs.bps == rhs.bps) && (lhs.fps == rhs.fps) && (lhs.encoding_gop == rhs.encoding_gop) && (fabs(static_cast<double>(lhs.width - rhs.width)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.height - rhs.height)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPVIDEOENCODINGPARAMETER_H
