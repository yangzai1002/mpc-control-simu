/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_COMMONHEADER_H
#define ARA_ADSFI_IMPL_TYPE_COMMONHEADER_H

#include "impl_type_int64_t.h"
#include "ara/adsfi/impl_type_msghaftime.h"
#include "impl_type_string.h"
#include "ara/adsfi/impl_type_commonfaultinfovec.h"
#include "ara/adsfi/impl_type_commontimechainvec.h"

namespace ara {
namespace adsfi {
struct CommonHeader {
    ::int64_t seq{ 0 };
    ::ara::adsfi::MsgHafTime time;
    ::String module_name;
    ::String data_name;
    ::String frame_id;
    ::ara::adsfi::CommonFaultInfoVec fault_vec;
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
        fun(time);
        fun(module_name);
        fun(data_name);
        fun(frame_id);
        fun(fault_vec);
        fun(time_chain_vec);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(seq);
        fun(time);
        fun(module_name);
        fun(data_name);
        fun(frame_id);
        fun(fault_vec);
        fun(time_chain_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("seq", seq);
        fun("time", time);
        fun("module_name", module_name);
        fun("data_name", data_name);
        fun("frame_id", frame_id);
        fun("fault_vec", fault_vec);
        fun("time_chain_vec", time_chain_vec);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("seq", seq);
        fun("time", time);
        fun("module_name", module_name);
        fun("data_name", data_name);
        fun("frame_id", frame_id);
        fun("fault_vec", fault_vec);
        fun("time_chain_vec", time_chain_vec);
    }

    friend bool operator==(const ::ara::adsfi::CommonHeader& lhs, const ::ara::adsfi::CommonHeader& rhs) noexcept
    {
        return (lhs.seq == rhs.seq) && (lhs.time == rhs.time) && (lhs.module_name == rhs.module_name) && (lhs.data_name == rhs.data_name) && (lhs.frame_id == rhs.frame_id) && (lhs.fault_vec == rhs.fault_vec) && (lhs.time_chain_vec == rhs.time_chain_vec);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_COMMONHEADER_H
