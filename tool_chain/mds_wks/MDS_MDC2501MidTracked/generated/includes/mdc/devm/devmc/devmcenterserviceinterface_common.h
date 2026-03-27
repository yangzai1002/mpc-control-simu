/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_DEVM_DEVMC_DEVMCENTERSERVICEINTERFACE_COMMON_H
#define MDC_DEVM_DEVMC_DEVMCENTERSERVICEINTERFACE_COMMON_H

#include "ara/com/types.h"
#include "ara/com/init_config.h"
#include "mdc/devm/impl_type_uint8list.h"
#include <cfloat>
#include <cmath>

namespace mdc {
namespace devm {
namespace devmc {
namespace methods {
namespace MsgPipeQuery {
struct Output {
    ::mdc::devm::Uint8List out;

    static bool IsPlane()
    {
        return false;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(out);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(out);
    }

    bool operator==(const Output& t) const
    {
       return (out == t.out);
    }
};
} // namespace MsgPipeQuery
namespace MsgPipeCtrl {
struct Output {
    ::mdc::devm::Uint8List out;

    static bool IsPlane()
    {
        return false;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(out);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(out);
    }

    bool operator==(const Output& t) const
    {
       return (out == t.out);
    }
};
} // namespace MsgPipeCtrl
namespace MsgPipeUpdate {
struct Output {
    ::mdc::devm::Uint8List out;

    static bool IsPlane()
    {
        return false;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(out);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(out);
    }

    bool operator==(const Output& t) const
    {
       return (out == t.out);
    }
};
} // namespace MsgPipeUpdate
namespace MsgPipe1 {
struct Output {
    ::mdc::devm::Uint8List out;

    static bool IsPlane()
    {
        return false;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(out);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(out);
    }

    bool operator==(const Output& t) const
    {
       return (out == t.out);
    }
};
} // namespace MsgPipe1
namespace MsgPipe2 {
struct Output {
    ::mdc::devm::Uint8List out;

    static bool IsPlane()
    {
        return false;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(out);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(out);
    }

    bool operator==(const Output& t) const
    {
       return (out == t.out);
    }
};
} // namespace MsgPipe2
namespace MsgPipe3 {
struct Output {
    ::mdc::devm::Uint8List out;

    static bool IsPlane()
    {
        return false;
    }

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(out);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(out);
    }

    bool operator==(const Output& t) const
    {
       return (out == t.out);
    }
};
} // namespace MsgPipe3
} // namespace methods

class DevmCenterServiceInterface {
public:
    constexpr DevmCenterServiceInterface() = default;
    constexpr static ara::com::ServiceIdentifierType ServiceIdentifier = ara::com::ServiceIdentifierType("/HuaweiMDC/PlatformServiceInterface/DevmCenterServiceInterface/DevmCenterServiceInterface");
    constexpr static ara::com::ServiceVersionType ServiceVersion = ara::com::ServiceVersionType("1.1");
};
} // namespace devmc
} // namespace devm
} // namespace mdc

#endif // MDC_DEVM_DEVMC_DEVMCENTERSERVICEINTERFACE_COMMON_H
