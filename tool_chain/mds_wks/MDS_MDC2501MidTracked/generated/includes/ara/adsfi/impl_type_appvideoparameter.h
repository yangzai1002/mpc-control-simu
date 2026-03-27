/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPVIDEOPARAMETER_H
#define ARA_ADSFI_IMPL_TYPE_APPVIDEOPARAMETER_H

#include "ara/adsfi/impl_type_appvideoeditorparameter.h"
#include "ara/adsfi/impl_type_appvideoencodingparameter.h"

namespace ara {
namespace adsfi {
struct AppVideoParameter {
    ::ara::adsfi::AppVideoEditorParameter editor_parameter;
    ::ara::adsfi::AppVideoEncodingParameter encoding_parameter;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(editor_parameter);
        fun(encoding_parameter);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(editor_parameter);
        fun(encoding_parameter);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("editor_parameter", editor_parameter);
        fun("encoding_parameter", encoding_parameter);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("editor_parameter", editor_parameter);
        fun("encoding_parameter", encoding_parameter);
    }

    friend bool operator==(const ::ara::adsfi::AppVideoParameter& lhs, const ::ara::adsfi::AppVideoParameter& rhs) noexcept
    {
        return (lhs.editor_parameter == rhs.editor_parameter) && (lhs.encoding_parameter == rhs.encoding_parameter);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPVIDEOPARAMETER_H
