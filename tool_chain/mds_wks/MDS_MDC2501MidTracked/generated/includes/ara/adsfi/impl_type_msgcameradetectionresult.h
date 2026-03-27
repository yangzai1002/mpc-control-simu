/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGCAMERADETECTIONRESULT_H
#define ARA_ADSFI_IMPL_TYPE_MSGCAMERADETECTIONRESULT_H

#include "ara/adsfi/impl_type_msghafheader.h"
#include "ara/adsfi/impl_type_msghaf3ddetectionoutarray.h"
#include "ara/adsfi/impl_type_msgimagedatavec.h"

namespace ara {
namespace adsfi {
struct MsgCameraDetectionResult {
    ::ara::adsfi::MsgHafHeader header;
    ::ara::adsfi::MsgHaf3dDetectionOutArray image_objects;
    ::ara::adsfi::MsgImageDataVec image_segmentation_vec;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(image_objects);
        fun(image_segmentation_vec);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(image_objects);
        fun(image_segmentation_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("image_objects", image_objects);
        fun("image_segmentation_vec", image_segmentation_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("image_objects", image_objects);
        fun("image_segmentation_vec", image_segmentation_vec);
    }

    friend bool operator==(const ::ara::adsfi::MsgCameraDetectionResult& lhs, const ::ara::adsfi::MsgCameraDetectionResult& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.image_objects == rhs.image_objects) && (lhs.image_segmentation_vec == rhs.image_segmentation_vec);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGCAMERADETECTIONRESULT_H
