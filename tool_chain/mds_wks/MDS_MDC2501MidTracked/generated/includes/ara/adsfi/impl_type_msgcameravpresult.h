/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGCAMERAVPRESULT_H
#define ARA_ADSFI_IMPL_TYPE_MSGCAMERAVPRESULT_H

#include "ara/adsfi/impl_type_msghafheader.h"
#include "ara/adsfi/impl_type_msgcameradetectionresultvec.h"

namespace ara {
namespace adsfi {
struct MsgCameraVPResult {
    ::ara::adsfi::MsgHafHeader header;
    ::ara::adsfi::MsgCameraDetectionResultVec vp_result_vec;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(vp_result_vec);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(vp_result_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("vp_result_vec", vp_result_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("vp_result_vec", vp_result_vec);
    }

    friend bool operator==(const ::ara::adsfi::MsgCameraVPResult& lhs, const ::ara::adsfi::MsgCameraVPResult& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.vp_result_vec == rhs.vp_result_vec);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGCAMERAVPRESULT_H
