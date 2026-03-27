/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAF3DDETECTIONOUTARRAY_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAF3DDETECTIONOUTARRAY_H

#include "ara/adsfi/impl_type_msghaftime.h"
#include "impl_type_int32_t.h"
#include "ara/adsfi/impl_type_msghaf3ddetectionoutvec.h"
#include "ara/adsfi/impl_type_msghaf2ddetectionoutvec.h"
#include "impl_type_string.h"
#include "impl_type_uint32_t.h"
#include "ara/adsfi/impl_type_commontimechainvec.h"

namespace ara {
namespace adsfi {
struct MsgHaf3dDetectionOutArray {
    ::ara::adsfi::MsgHafTime timestamp;
    ::int32_t count{ 0 };
    ::ara::adsfi::MsgHaf3dDetectionOutVec detectionOut3d;
    ::ara::adsfi::MsgHaf2dDetectionOutVec detectionOut2d;
    ::String frameID;
    ::uint32_t seq{ 0U };
    ::ara::adsfi::CommonTimeChainVec time_chain_vec;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(timestamp);
        fun(count);
        fun(detectionOut3d);
        fun(detectionOut2d);
        fun(frameID);
        fun(seq);
        fun(time_chain_vec);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(timestamp);
        fun(count);
        fun(detectionOut3d);
        fun(detectionOut2d);
        fun(frameID);
        fun(seq);
        fun(time_chain_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("timestamp", timestamp);
        fun("count", count);
        fun("detectionOut3d", detectionOut3d);
        fun("detectionOut2d", detectionOut2d);
        fun("frameID", frameID);
        fun("seq", seq);
        fun("time_chain_vec", time_chain_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("timestamp", timestamp);
        fun("count", count);
        fun("detectionOut3d", detectionOut3d);
        fun("detectionOut2d", detectionOut2d);
        fun("frameID", frameID);
        fun("seq", seq);
        fun("time_chain_vec", time_chain_vec);
    }

    friend bool operator==(const ::ara::adsfi::MsgHaf3dDetectionOutArray& lhs, const ::ara::adsfi::MsgHaf3dDetectionOutArray& rhs) noexcept
    {
        return (lhs.timestamp == rhs.timestamp) && (lhs.count == rhs.count) && (lhs.detectionOut3d == rhs.detectionOut3d) && (lhs.detectionOut2d == rhs.detectionOut2d) && (lhs.frameID == rhs.frameID) && (lhs.seq == rhs.seq) && (lhs.time_chain_vec == rhs.time_chain_vec);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAF3DDETECTIONOUTARRAY_H
