/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPVIDEOSPLITPOINT_H
#define ARA_ADSFI_IMPL_TYPE_APPVIDEOSPLITPOINT_H

#include "impl_type_int32_t.h"

namespace ara {
namespace adsfi {
struct AppVideoSplitPoint {
    ::int32_t left_top_point_x{ 0 };
    ::int32_t left_top_point_y{ 0 };
    ::int32_t right_bottom_point_x{ 0 };
    ::int32_t right_bottom_point_y{ 0 };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(left_top_point_x);
        fun(left_top_point_y);
        fun(right_bottom_point_x);
        fun(right_bottom_point_y);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(left_top_point_x);
        fun(left_top_point_y);
        fun(right_bottom_point_x);
        fun(right_bottom_point_y);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("left_top_point_x", left_top_point_x);
        fun("left_top_point_y", left_top_point_y);
        fun("right_bottom_point_x", right_bottom_point_x);
        fun("right_bottom_point_y", right_bottom_point_y);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("left_top_point_x", left_top_point_x);
        fun("left_top_point_y", left_top_point_y);
        fun("right_bottom_point_x", right_bottom_point_x);
        fun("right_bottom_point_y", right_bottom_point_y);
    }

    friend bool operator==(const ::ara::adsfi::AppVideoSplitPoint& lhs, const ::ara::adsfi::AppVideoSplitPoint& rhs) noexcept
    {
        return (lhs.left_top_point_x == rhs.left_top_point_x) && (lhs.left_top_point_y == rhs.left_top_point_y) && (lhs.right_bottom_point_x == rhs.right_bottom_point_x) && (lhs.right_bottom_point_y == rhs.right_bottom_point_y);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPVIDEOSPLITPOINT_H
