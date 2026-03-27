/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGIMAGEDATALIST_H
#define ARA_ADSFI_IMPL_TYPE_MSGIMAGEDATALIST_H

#include "ara/adsfi/impl_type_msghafheader.h"
#include "ara/adsfi/impl_type_msgimagedatavec.h"

namespace ara {
namespace adsfi {
struct MsgImageDataList {
    ::ara::adsfi::MsgHafHeader header;
    ::ara::adsfi::MsgImageDataVec images;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(images);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(images);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("images", images);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("images", images);
    }

    friend bool operator==(const ::ara::adsfi::MsgImageDataList& lhs, const ::ara::adsfi::MsgImageDataList& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.images == rhs.images);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGIMAGEDATALIST_H
