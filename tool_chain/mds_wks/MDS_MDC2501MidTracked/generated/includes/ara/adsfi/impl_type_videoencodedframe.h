/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VIDEOENCODEDFRAME_H
#define ARA_ADSFI_IMPL_TYPE_VIDEOENCODEDFRAME_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_int32_t.h"
#include "ara/adsfi/impl_type_idpuint8vec.h"

namespace ara {
namespace adsfi {
struct VideoEncodedFrame {
    ::ara::adsfi::CommonHeader header;
    ::int32_t encoding{ 0 };
    ::int32_t key_frame{ 0 };
    ::int32_t frame_size{ 0 };
    ::ara::adsfi::IdpUint8Vec frame_data;
    double fps{ 0.0 };
    double pts{ 0.0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(encoding);
        fun(key_frame);
        fun(frame_size);
        fun(frame_data);
        fun(fps);
        fun(pts);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(encoding);
        fun(key_frame);
        fun(frame_size);
        fun(frame_data);
        fun(fps);
        fun(pts);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("encoding", encoding);
        fun("key_frame", key_frame);
        fun("frame_size", frame_size);
        fun("frame_data", frame_data);
        fun("fps", fps);
        fun("pts", pts);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("encoding", encoding);
        fun("key_frame", key_frame);
        fun("frame_size", frame_size);
        fun("frame_data", frame_data);
        fun("fps", fps);
        fun("pts", pts);
    }

    friend bool operator==(const ::ara::adsfi::VideoEncodedFrame& lhs, const ::ara::adsfi::VideoEncodedFrame& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.encoding == rhs.encoding) && (lhs.key_frame == rhs.key_frame) && (lhs.frame_size == rhs.frame_size) && (lhs.frame_data == rhs.frame_data) && (fabs(static_cast<double>(lhs.fps - rhs.fps)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.pts - rhs.pts)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VIDEOENCODEDFRAME_H
