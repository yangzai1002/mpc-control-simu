/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_SAMM_IMPL_TYPE_SAMMFEATURE_H
#define ARA_SAMM_IMPL_TYPE_SAMMFEATURE_H

#include "impl_type_defaultfeaturevector.h"
#include "ara/selectpoint/impl_type_header.h"
#include "impl_type_customfeaturevector.h"

namespace ara {
namespace samm {
struct SammFeature {
    ::DefaultFeatureVector defaultFeatures;
    ::ara::selectpoint::Header header;
    ::CustomFeatureVector customFeatures;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(defaultFeatures);
        fun(header);
        fun(customFeatures);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(defaultFeatures);
        fun(header);
        fun(customFeatures);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("defaultFeatures", defaultFeatures);
        fun("header", header);
        fun("customFeatures", customFeatures);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("defaultFeatures", defaultFeatures);
        fun("header", header);
        fun("customFeatures", customFeatures);
    }

    friend bool operator==(const ::ara::samm::SammFeature& lhs, const ::ara::samm::SammFeature& rhs) noexcept
    {
        return (lhs.defaultFeatures == rhs.defaultFeatures) && (lhs.header == rhs.header) && (lhs.customFeatures == rhs.customFeatures);
    }
};
} // namespace samm
} // namespace ara


#endif // ARA_SAMM_IMPL_TYPE_SAMMFEATURE_H
