/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFFUSIONOUTARRAY_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFFUSIONOUTARRAY_H

#include "ara/adsfi/impl_type_msghaftime.h"
#include "ara/adsfi/impl_type_commontimechainvec.h"
#include "impl_type_int32_t.h"
#include "ara/adsfi/impl_type_msghaffusionoutvec.h"
#include "impl_type_string.h"
#include "impl_type_uint32_t.h"

namespace ara {
namespace adsfi {
struct MsgHafFusionOutArray {
    ::ara::adsfi::MsgHafTime stamp;
    ::ara::adsfi::CommonTimeChainVec time_chain_vec;
    ::int32_t count{ 0 };
    ::ara::adsfi::MsgHafFusionOutVec fusionOut;
    ::String frameID;
    ::uint32_t seq{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(stamp);
        fun(time_chain_vec);
        fun(count);
        fun(fusionOut);
        fun(frameID);
        fun(seq);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(stamp);
        fun(time_chain_vec);
        fun(count);
        fun(fusionOut);
        fun(frameID);
        fun(seq);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("stamp", stamp);
        fun("time_chain_vec", time_chain_vec);
        fun("count", count);
        fun("fusionOut", fusionOut);
        fun("frameID", frameID);
        fun("seq", seq);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("stamp", stamp);
        fun("time_chain_vec", time_chain_vec);
        fun("count", count);
        fun("fusionOut", fusionOut);
        fun("frameID", frameID);
        fun("seq", seq);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafFusionOutArray& lhs, const ::ara::adsfi::MsgHafFusionOutArray& rhs) noexcept
    {
        return (lhs.stamp == rhs.stamp) && (lhs.time_chain_vec == rhs.time_chain_vec) && (lhs.count == rhs.count) && (lhs.fusionOut == rhs.fusionOut) && (lhs.frameID == rhs.frameID) && (lhs.seq == rhs.seq);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFFUSIONOUTARRAY_H
