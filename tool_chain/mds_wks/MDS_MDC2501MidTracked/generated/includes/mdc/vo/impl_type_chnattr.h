/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_VO_IMPL_TYPE_CHNATTR_H
#define MDC_VO_IMPL_TYPE_CHNATTR_H


namespace mdc {
namespace vo {
struct ChnAttr {
    bool isEnabled{ false };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(isEnabled);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(isEnabled);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("isEnabled", isEnabled);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("isEnabled", isEnabled);
    }

    friend bool operator==(const ::mdc::vo::ChnAttr& lhs, const ::mdc::vo::ChnAttr& rhs) noexcept
    {
        return (lhs.isEnabled == rhs.isEnabled);
    }
};
} // namespace vo
} // namespace mdc


#endif // MDC_VO_IMPL_TYPE_CHNATTR_H
