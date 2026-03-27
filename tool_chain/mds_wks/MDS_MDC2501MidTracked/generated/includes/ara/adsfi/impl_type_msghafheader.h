/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGHAFHEADER_H
#define ARA_ADSFI_IMPL_TYPE_MSGHAFHEADER_H

#include "impl_type_uint32_t.h"
#include "ara/adsfi/impl_type_msghaftime.h"
#include "impl_type_string.h"
#include "ara/adsfi/impl_type_commontimechainvec.h"

namespace ara {
namespace adsfi {
struct MsgHafHeader {
    ::uint32_t seq{ 0U };
    ::ara::adsfi::MsgHafTime timestamp;
    ::String frameID;
    ::ara::adsfi::CommonTimeChainVec time_chain_vec;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(seq);
        fun(timestamp);
        fun(frameID);
        fun(time_chain_vec);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(seq);
        fun(timestamp);
        fun(frameID);
        fun(time_chain_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("seq", seq);
        fun("timestamp", timestamp);
        fun("frameID", frameID);
        fun("time_chain_vec", time_chain_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("seq", seq);
        fun("timestamp", timestamp);
        fun("frameID", frameID);
        fun("time_chain_vec", time_chain_vec);
    }

    friend bool operator==(const ::ara::adsfi::MsgHafHeader& lhs, const ::ara::adsfi::MsgHafHeader& rhs) noexcept
    {
        return (lhs.seq == rhs.seq) && (lhs.timestamp == rhs.timestamp) && (lhs.frameID == rhs.frameID) && (lhs.time_chain_vec == rhs.time_chain_vec);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGHAFHEADER_H
