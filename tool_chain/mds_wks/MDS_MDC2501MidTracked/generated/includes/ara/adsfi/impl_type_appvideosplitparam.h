/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPVIDEOSPLITPARAM_H
#define ARA_ADSFI_IMPL_TYPE_APPVIDEOSPLITPARAM_H

#include "impl_type_string.h"
#include "ara/adsfi/impl_type_appvideosplitpoint.h"

namespace ara {
namespace adsfi {
struct AppVideoSplitParam {
    ::String channel_name;
    ::ara::adsfi::AppVideoSplitPoint view_area_points;
    ::ara::adsfi::AppVideoSplitPoint crop_area_points;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(channel_name);
        fun(view_area_points);
        fun(crop_area_points);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(channel_name);
        fun(view_area_points);
        fun(crop_area_points);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("channel_name", channel_name);
        fun("view_area_points", view_area_points);
        fun("crop_area_points", crop_area_points);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("channel_name", channel_name);
        fun("view_area_points", view_area_points);
        fun("crop_area_points", crop_area_points);
    }

    friend bool operator==(const ::ara::adsfi::AppVideoSplitParam& lhs, const ::ara::adsfi::AppVideoSplitParam& rhs) noexcept
    {
        return (lhs.channel_name == rhs.channel_name) && (lhs.view_area_points == rhs.view_area_points) && (lhs.crop_area_points == rhs.crop_area_points);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPVIDEOSPLITPARAM_H
