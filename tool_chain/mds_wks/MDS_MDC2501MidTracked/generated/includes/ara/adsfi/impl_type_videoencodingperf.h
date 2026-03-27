/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VIDEOENCODINGPERF_H
#define ARA_ADSFI_IMPL_TYPE_VIDEOENCODINGPERF_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_msghafheader.h"

namespace ara {
namespace adsfi {
struct VideoEncodingPerf {
    ::ara::adsfi::MsgHafHeader header;
    double avg_ms_scaling{ 0.0 };
    double avg_ms_color_converting{ 0.0 };
    double avg_ms_encoding{ 0.0 };
    double avg_ms_sending{ 0.0 };
    double avg_ms_span_from_driver_to_scale{ 0.0 };
    float fps{ 0.0F };
    double avg_ms_span_from_driver_to_send_done{ 0.0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(avg_ms_scaling);
        fun(avg_ms_color_converting);
        fun(avg_ms_encoding);
        fun(avg_ms_sending);
        fun(avg_ms_span_from_driver_to_scale);
        fun(fps);
        fun(avg_ms_span_from_driver_to_send_done);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(avg_ms_scaling);
        fun(avg_ms_color_converting);
        fun(avg_ms_encoding);
        fun(avg_ms_sending);
        fun(avg_ms_span_from_driver_to_scale);
        fun(fps);
        fun(avg_ms_span_from_driver_to_send_done);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("avg_ms_scaling", avg_ms_scaling);
        fun("avg_ms_color_converting", avg_ms_color_converting);
        fun("avg_ms_encoding", avg_ms_encoding);
        fun("avg_ms_sending", avg_ms_sending);
        fun("avg_ms_span_from_driver_to_scale", avg_ms_span_from_driver_to_scale);
        fun("fps", fps);
        fun("avg_ms_span_from_driver_to_send_done", avg_ms_span_from_driver_to_send_done);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("avg_ms_scaling", avg_ms_scaling);
        fun("avg_ms_color_converting", avg_ms_color_converting);
        fun("avg_ms_encoding", avg_ms_encoding);
        fun("avg_ms_sending", avg_ms_sending);
        fun("avg_ms_span_from_driver_to_scale", avg_ms_span_from_driver_to_scale);
        fun("fps", fps);
        fun("avg_ms_span_from_driver_to_send_done", avg_ms_span_from_driver_to_send_done);
    }

    friend bool operator==(const ::ara::adsfi::VideoEncodingPerf& lhs, const ::ara::adsfi::VideoEncodingPerf& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.avg_ms_scaling - rhs.avg_ms_scaling)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.avg_ms_color_converting - rhs.avg_ms_color_converting)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.avg_ms_encoding - rhs.avg_ms_encoding)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.avg_ms_sending - rhs.avg_ms_sending)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.avg_ms_span_from_driver_to_scale - rhs.avg_ms_span_from_driver_to_scale)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.fps - rhs.fps)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.avg_ms_span_from_driver_to_send_done - rhs.avg_ms_span_from_driver_to_send_done)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VIDEOENCODINGPERF_H
