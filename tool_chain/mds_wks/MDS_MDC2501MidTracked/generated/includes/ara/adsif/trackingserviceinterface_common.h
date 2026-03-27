/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSIF_TRACKINGSERVICEINTERFACE_COMMON_H
#define ARA_ADSIF_TRACKINGSERVICEINTERFACE_COMMON_H

#include "ara/com/types.h"
#include "ara/com/init_config.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include <cfloat>
#include <cmath>

namespace ara {
namespace adsif {

class TrackingServiceInterface {
public:
    constexpr TrackingServiceInterface() = default;
    constexpr static ara::com::ServiceIdentifierType ServiceIdentifier = ara::com::ServiceIdentifierType("/HuaweiMDC/interface_manifests/interfaces/TrackingServiceInterface");
    constexpr static ara::com::ServiceVersionType ServiceVersion = ara::com::ServiceVersionType("1.1");
};
} // namespace adsif
} // namespace ara

#endif // ARA_ADSIF_TRACKINGSERVICEINTERFACE_COMMON_H
