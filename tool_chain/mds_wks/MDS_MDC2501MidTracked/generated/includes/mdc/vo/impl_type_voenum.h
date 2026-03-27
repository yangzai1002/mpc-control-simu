/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_VO_IMPL_TYPE_VOENUM_H
#define MDC_VO_IMPL_TYPE_VOENUM_H

#include "impl_type_uint8_t.h"
namespace mdc {
namespace vo {
enum class VoEnum : uint8_t
{
    EXTERNAL = 0U,
    INTERNAL = 1U,
    VO3 = 2U
};
} // namespace vo
} // namespace mdc


#endif // MDC_VO_IMPL_TYPE_VOENUM_H
