/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_VO_IMPL_TYPE_VIDEOPARAM_H
#define MDC_VO_IMPL_TYPE_VIDEOPARAM_H

#include "ara/display/impl_type_imageformattypee.h"
#include "impl_type_uint32_t.h"
#include "mdc/vo/impl_type_resolution.h"

namespace mdc {
namespace vo {
struct VideoParam {
    ::ara::display::ImageFormatTypeE imageFormat{ ::ara::display::ImageFormatTypeE::IMAGE_FORMAT_RGB888 };
    ::uint32_t frameRate{ 0U };
    ::mdc::vo::Resolution resolution;

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(imageFormat);
        fun(frameRate);
        fun(resolution);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(imageFormat);
        fun(frameRate);
        fun(resolution);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("imageFormat", imageFormat);
        fun("frameRate", frameRate);
        fun("resolution", resolution);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("imageFormat", imageFormat);
        fun("frameRate", frameRate);
        fun("resolution", resolution);
    }

    friend bool operator==(const ::mdc::vo::VideoParam& lhs, const ::mdc::vo::VideoParam& rhs) noexcept
    {
        return (lhs.imageFormat == rhs.imageFormat) && (lhs.frameRate == rhs.frameRate) && (lhs.resolution == rhs.resolution);
    }
};
} // namespace vo
} // namespace mdc


#endif // MDC_VO_IMPL_TYPE_VIDEOPARAM_H
