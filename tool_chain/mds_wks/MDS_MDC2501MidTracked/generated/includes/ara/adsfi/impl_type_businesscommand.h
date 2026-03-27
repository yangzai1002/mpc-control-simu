/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_BUSINESSCOMMAND_H
#define ARA_ADSFI_IMPL_TYPE_BUSINESSCOMMAND_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_int32_t.h"
#include "ara/adsfi/impl_type_businesspath.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_busniessremotecontrol.h"
#include "ara/adsfi/impl_type_businesscommandparameter.h"

namespace ara {
namespace adsfi {
struct BusinessCommand {
    ::ara::adsfi::CommonHeader header;
    ::int32_t business_mode{ 0 };
    ::ara::adsfi::BusinessPath business_path;
    ::ara::adsfi::MsgHafEgoTrajectory last_path;
    ::ara::adsfi::BusniessRemoteControl remote_ctl;
    ::ara::adsfi::BusinessCommandParameter param;
    ::int32_t estop_flag{ 0 };
    ::int32_t record_trigger_flag{ 0 };
    ::int32_t loc_strategy{ 0 };
    ::int32_t loc_control_mode{ 0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(business_mode);
        fun(business_path);
        fun(last_path);
        fun(remote_ctl);
        fun(param);
        fun(estop_flag);
        fun(record_trigger_flag);
        fun(loc_strategy);
        fun(loc_control_mode);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(business_mode);
        fun(business_path);
        fun(last_path);
        fun(remote_ctl);
        fun(param);
        fun(estop_flag);
        fun(record_trigger_flag);
        fun(loc_strategy);
        fun(loc_control_mode);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("business_mode", business_mode);
        fun("business_path", business_path);
        fun("last_path", last_path);
        fun("remote_ctl", remote_ctl);
        fun("param", param);
        fun("estop_flag", estop_flag);
        fun("record_trigger_flag", record_trigger_flag);
        fun("loc_strategy", loc_strategy);
        fun("loc_control_mode", loc_control_mode);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("business_mode", business_mode);
        fun("business_path", business_path);
        fun("last_path", last_path);
        fun("remote_ctl", remote_ctl);
        fun("param", param);
        fun("estop_flag", estop_flag);
        fun("record_trigger_flag", record_trigger_flag);
        fun("loc_strategy", loc_strategy);
        fun("loc_control_mode", loc_control_mode);
    }

    friend bool operator==(const ::ara::adsfi::BusinessCommand& lhs, const ::ara::adsfi::BusinessCommand& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.business_mode == rhs.business_mode) && (lhs.business_path == rhs.business_path) && (lhs.last_path == rhs.last_path) && (lhs.remote_ctl == rhs.remote_ctl) && (lhs.param == rhs.param) && (lhs.estop_flag == rhs.estop_flag) && (lhs.record_trigger_flag == rhs.record_trigger_flag) && (lhs.loc_strategy == rhs.loc_strategy) && (lhs.loc_control_mode == rhs.loc_control_mode);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_BUSINESSCOMMAND_H
