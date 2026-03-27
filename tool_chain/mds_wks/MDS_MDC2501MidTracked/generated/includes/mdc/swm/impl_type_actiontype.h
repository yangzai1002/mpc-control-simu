/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_SWM_IMPL_TYPE_ACTIONTYPE_H
#define MDC_SWM_IMPL_TYPE_ACTIONTYPE_H

#include "impl_type_uint8.h"
namespace mdc {
namespace swm {
enum class ActionType : UInt8
{
    kUpdate = 0,
    kInstall = 1,
    kRemove = 2
};
} // namespace swm
} // namespace mdc


#endif // MDC_SWM_IMPL_TYPE_ACTIONTYPE_H
