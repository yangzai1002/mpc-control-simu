/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPIMAGE_H
#define ARA_ADSFI_IMPL_TYPE_APPIMAGE_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_int32_t.h"
#include "impl_type_string.h"
#include "ara/adsfi/impl_type_idpuint8vec.h"

namespace ara {
namespace adsfi {
struct AppImage {
    ::ara::adsfi::CommonHeader header;
    ::int32_t height{ 0 };
    ::int32_t width{ 0 };
    ::String encoding;
    ::int32_t step{ 0 };
    ::ara::adsfi::IdpUint8Vec data;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(height);
        fun(width);
        fun(encoding);
        fun(step);
        fun(data);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(height);
        fun(width);
        fun(encoding);
        fun(step);
        fun(data);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("height", height);
        fun("width", width);
        fun("encoding", encoding);
        fun("step", step);
        fun("data", data);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("height", height);
        fun("width", width);
        fun("encoding", encoding);
        fun("step", step);
        fun("data", data);
    }

    friend bool operator==(const ::ara::adsfi::AppImage& lhs, const ::ara::adsfi::AppImage& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.height == rhs.height) && (lhs.width == rhs.width) && (lhs.encoding == rhs.encoding) && (lhs.step == rhs.step) && (lhs.data == rhs.data);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPIMAGE_H
