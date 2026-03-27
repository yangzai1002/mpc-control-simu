/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_FMQUERYSERVICE_COMMON_H
#define MDC_FM_FMQUERYSERVICE_COMMON_H

#include "ara/com/types.h"
#include "ara/com/init_config.h"
#include "mdc/fm/impl_type_fmfaultdetailvector.h"
#include "mdc/fm/impl_type_fmfaultfullvec.h"
#include "impl_type_uint32.h"
#include "mdc/fm/impl_type_fmfaultstatdata.h"
#include "impl_type_int32.h"
#include <cfloat>
#include <cmath>

namespace mdc {
namespace fm {
namespace methods {
namespace QueryFaultDetail {
struct Output {
    ::mdc::fm::FmFaultDetailVector faultDetailVec;

    static bool IsPlane()
    {
        return false;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(faultDetailVec);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(faultDetailVec);
    }

    bool operator==(const Output& t) const
    {
       return (faultDetailVec == t.faultDetailVec);
    }
};
} // namespace QueryFaultDetail
namespace QueryFaultOnFlag {
struct Output {
    ::mdc::fm::FmFaultFullVec fmFltFullVec;

    static bool IsPlane()
    {
        return false;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(fmFltFullVec);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(fmFltFullVec);
    }

    bool operator==(const Output& t) const
    {
       return (fmFltFullVec == t.fmFltFullVec);
    }
};
} // namespace QueryFaultOnFlag
namespace QueryFaultStatistic {
struct Output {
    ::mdc::fm::FmFaultStatData fmFltStatic;

    static bool IsPlane()
    {
        return true;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(fmFltStatic);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(fmFltStatic);
    }

    bool operator==(const Output& t) const
    {
       return (fmFltStatic == t.fmFltStatic);
    }
};
} // namespace QueryFaultStatistic
namespace QuerySystemFaultStatus {
struct Output {
    ::Int32 result{ 0 };

    static bool IsPlane()
    {
        return true;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(result);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(result);
    }

    bool operator==(const Output& t) const
    {
       return (result == t.result);
    }
};
} // namespace QuerySystemFaultStatus
} // namespace methods

class FmQueryService {
public:
    constexpr FmQueryService() = default;
    constexpr static ara::com::ServiceIdentifierType ServiceIdentifier = ara::com::ServiceIdentifierType("/HuaweiMDC/MdcPlatformServices/PlatformServiceInterface/FmServiceInterface/FmQueryService");
    constexpr static ara::com::ServiceVersionType ServiceVersion = ara::com::ServiceVersionType("1.1");
};
} // namespace fm
} // namespace mdc

#endif // MDC_FM_FMQUERYSERVICE_COMMON_H
