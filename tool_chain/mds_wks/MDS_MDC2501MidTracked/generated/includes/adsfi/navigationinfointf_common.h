/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_NAVIGATIONINFOINTF_COMMON_H
#define ADSFI_NAVIGATIONINFOINTF_COMMON_H

#include "ara/com/types.h"
#include "ara/com/init_config.h"
#include "adsfi/impl_type_navigationinfo.h"
#include <cfloat>
#include <cmath>

namespace adsfi {

class NavigationInfoIntf {
public:
    constexpr NavigationInfoIntf() = default;
    constexpr static ara::com::ServiceIdentifierType ServiceIdentifier = ara::com::ServiceIdentifierType("/HuaweiMDC/FunctionalSoftware/NavigationInfoServiceInterface/NavigationInfoIntf");
    constexpr static ara::com::ServiceVersionType ServiceVersion = ara::com::ServiceVersionType("1.1");
};
} // namespace adsfi

#endif // ADSFI_NAVIGATIONINFOINTF_COMMON_H
