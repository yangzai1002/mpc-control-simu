/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_SAMM_SAMMSCENESERVICEINTERFACE_COMMON_H
#define ARA_SAMM_SAMMSCENESERVICEINTERFACE_COMMON_H

#include "ara/com/types.h"
#include "ara/com/init_config.h"
#include "ara/samm/impl_type_sammscene.h"
#include <cfloat>
#include <cmath>

namespace ara {
namespace samm {

class SammSceneServiceInterface {
public:
    constexpr SammSceneServiceInterface() = default;
    constexpr static ara::com::ServiceIdentifierType ServiceIdentifier = ara::com::ServiceIdentifierType("/HuaweiMDC/FunctionalSoftware/SammSceneServiceInterface/SammSceneServiceInterface");
    constexpr static ara::com::ServiceVersionType ServiceVersion = ara::com::ServiceVersionType("1.1");
};
} // namespace samm
} // namespace ara

#endif // ARA_SAMM_SAMMSCENESERVICEINTERFACE_COMMON_H
