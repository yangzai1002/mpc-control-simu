/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_DIAG_IMPL_TYPE_COMCTRLREQUESTPARAMSTYPE_H
#define ARA_DIAG_IMPL_TYPE_COMCTRLREQUESTPARAMSTYPE_H

#include "impl_type_uint8.h"
#include "impl_type_uint16.h"

namespace ara {
namespace diag {
struct ComCtrlRequestParamsType {
    ::UInt8 controlType{ 0U };
    ::UInt8 communicationType{ 0U };
    ::UInt8 subnetNumber{ 0U };
    ::UInt16 nodeIdentificationNumber{ 0U };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(controlType);
        fun(communicationType);
        fun(subnetNumber);
        fun(nodeIdentificationNumber);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(controlType);
        fun(communicationType);
        fun(subnetNumber);
        fun(nodeIdentificationNumber);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("controlType", controlType);
        fun("communicationType", communicationType);
        fun("subnetNumber", subnetNumber);
        fun("nodeIdentificationNumber", nodeIdentificationNumber);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("controlType", controlType);
        fun("communicationType", communicationType);
        fun("subnetNumber", subnetNumber);
        fun("nodeIdentificationNumber", nodeIdentificationNumber);
    }

    friend bool operator==(const ::ara::diag::ComCtrlRequestParamsType& lhs, const ::ara::diag::ComCtrlRequestParamsType& rhs) noexcept
    {
        return (lhs.controlType == rhs.controlType) && (lhs.communicationType == rhs.communicationType) && (lhs.subnetNumber == rhs.subnetNumber) && (lhs.nodeIdentificationNumber == rhs.nodeIdentificationNumber);
    }
};
} // namespace diag
} // namespace ara


#endif // ARA_DIAG_IMPL_TYPE_COMCTRLREQUESTPARAMSTYPE_H
