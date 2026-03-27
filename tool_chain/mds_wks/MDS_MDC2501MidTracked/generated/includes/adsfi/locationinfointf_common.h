/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_LOCATIONINFOINTF_COMMON_H
#define ADSFI_LOCATIONINFOINTF_COMMON_H

#include "ara/com/types.h"
#include "ara/com/init_config.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include <cfloat>
#include <cmath>

namespace adsfi {

class LocationInfoIntf {
public:
    constexpr LocationInfoIntf() = default;
    constexpr static ara::com::ServiceIdentifierType ServiceIdentifier = ara::com::ServiceIdentifierType("/HuaweiMDC/FunctionalSoftware/LocationInfoServiceInterface/LocationInfoIntf");
    constexpr static ara::com::ServiceVersionType ServiceVersion = ara::com::ServiceVersionType("1.1");
};
} // namespace adsfi

#endif // ADSFI_LOCATIONINFOINTF_COMMON_H
