/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_VEHICLESENSORCONTROLSERVICEINTERFACE_COMMON_H
#define ARA_ADSFI_VEHICLESENSORCONTROLSERVICEINTERFACE_COMMON_H

#include "ara/com/types.h"
#include "ara/com/init_config.h"
#include "ara/adsfi/impl_type_vehiclesensorcontrol.h"
#include <cfloat>
#include <cmath>

namespace ara {
namespace adsfi {

class VehicleSensorControlServiceInterface {
public:
    constexpr VehicleSensorControlServiceInterface() = default;
    constexpr static ara::com::ServiceIdentifierType ServiceIdentifier = ara::com::ServiceIdentifierType("/HuaweiMDC/interface_manifests/interfaces/VehicleSensorControlServiceInterface");
    constexpr static ara::com::ServiceVersionType ServiceVersion = ara::com::ServiceVersionType("1.1");
};
} // namespace adsfi
} // namespace ara

#endif // ARA_ADSFI_VEHICLESENSORCONTROLSERVICEINTERFACE_COMMON_H
