/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_FAULTDATA_H
#define ARA_ADSFI_IMPL_TYPE_FAULTDATA_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "ara/adsfi/impl_type_faultinfovec.h"
#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct FaultData {
    ::ara::adsfi::CommonHeader header;
    ::ara::adsfi::FaultInfoVec fault_info;
    ::int32_t fault_level{ 0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(fault_info);
        fun(fault_level);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(fault_info);
        fun(fault_level);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("fault_info", fault_info);
        fun("fault_level", fault_level);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("fault_info", fault_info);
        fun("fault_level", fault_level);
    }

    friend bool operator==(const ::ara::adsfi::FaultData& lhs, const ::ara::adsfi::FaultData& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.fault_info == rhs.fault_info) && (lhs.fault_level == rhs.fault_level);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_FAULTDATA_H
