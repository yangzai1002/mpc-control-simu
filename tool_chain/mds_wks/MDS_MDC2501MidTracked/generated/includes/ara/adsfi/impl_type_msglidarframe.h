/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGLIDARFRAME_H
#define ARA_ADSFI_IMPL_TYPE_MSGLIDARFRAME_H

#include "ara/adsfi/impl_type_msghaftime.h"
#include "impl_type_uint32_t.h"
#include "impl_type_string.h"
#include "ara/adsfi/impl_type_msgpointxyzirtvec.h"

namespace ara {
namespace adsfi {
struct MsgLidarFrame {
    ::ara::adsfi::MsgHafTime timestamp;
    ::uint32_t seq{ 0U };
    ::String frameID;
    ::ara::adsfi::MsgPointXYZIRTVec pointCloud;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(timestamp);
        fun(seq);
        fun(frameID);
        fun(pointCloud);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(timestamp);
        fun(seq);
        fun(frameID);
        fun(pointCloud);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("timestamp", timestamp);
        fun("seq", seq);
        fun("frameID", frameID);
        fun("pointCloud", pointCloud);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("timestamp", timestamp);
        fun("seq", seq);
        fun("frameID", frameID);
        fun("pointCloud", pointCloud);
    }

    friend bool operator==(const ::ara::adsfi::MsgLidarFrame& lhs, const ::ara::adsfi::MsgLidarFrame& rhs) noexcept
    {
        return (lhs.timestamp == rhs.timestamp) && (lhs.seq == rhs.seq) && (lhs.frameID == rhs.frameID) && (lhs.pointCloud == rhs.pointCloud);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGLIDARFRAME_H
