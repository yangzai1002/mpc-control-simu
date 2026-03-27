/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_SWM_IMPL_TYPE_RESULTTYPE_H
#define MDC_SWM_IMPL_TYPE_RESULTTYPE_H

#include "impl_type_uint8.h"
namespace mdc {
namespace swm {
enum class ResultType : UInt8
{
    kSuccessfull = 0,
    kFailed = 1
};
} // namespace swm
} // namespace mdc


#endif // MDC_SWM_IMPL_TYPE_RESULTTYPE_H
