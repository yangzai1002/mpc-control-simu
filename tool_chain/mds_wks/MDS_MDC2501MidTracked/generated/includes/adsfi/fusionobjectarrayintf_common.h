/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_FUSIONOBJECTARRAYINTF_COMMON_H
#define ADSFI_FUSIONOBJECTARRAYINTF_COMMON_H

#include "ara/com/types.h"
#include "ara/com/init_config.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include <cfloat>
#include <cmath>

namespace adsfi {

class FusionObjectArrayIntf {
public:
    constexpr FusionObjectArrayIntf() = default;
    constexpr static ara::com::ServiceIdentifierType ServiceIdentifier = ara::com::ServiceIdentifierType("/HuaweiMDC/FunctionalSoftware/FusionObjectArrayServiceInterface/FusionObjectArrayIntf");
    constexpr static ara::com::ServiceVersionType ServiceVersion = ara::com::ServiceVersionType("1.1");
};
} // namespace adsfi

#endif // ADSFI_FUSIONOBJECTARRAYINTF_COMMON_H
