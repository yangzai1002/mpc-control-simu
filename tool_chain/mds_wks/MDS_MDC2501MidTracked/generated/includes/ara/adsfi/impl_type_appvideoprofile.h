/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPVIDEOPROFILE_H
#define ARA_ADSFI_IMPL_TYPE_APPVIDEOPROFILE_H

#include <cfloat>
#include <cmath>
#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_int32_t.h"
#include "impl_type_int64_t.h"
#include "ara/adsfi/impl_type_msgpoint3fvec.h"
#include "ara/adsfi/impl_type_appvideosplitparamvec.h"

namespace ara {
namespace adsfi {
struct AppVideoProfile {
    ::ara::adsfi::CommonHeader header;
    ::int32_t front_source{ 0 };
    ::int32_t output_mode{ 0 };
    ::int32_t adaptive_mode{ 0 };
    ::int32_t manual_bps{ 0 };
    double manual_width{ 0.0 };
    double manual_height{ 0.0 };
    ::int32_t remote_control_assist{ 0 };
    ::int32_t remote_control_alarm{ 0 };
    ::int32_t split_adaptive{ 0 };
    ::int32_t manual_split_main{ 0 };
    ::int32_t manual_split_left{ 0 };
    ::int32_t manual_split_right{ 0 };
    double receiver_reported_fps{ 0.0 };
    double receiver_reported_bps{ 0.0 };
    ::int64_t receiver_reported_ts{ 0 };
    ::ara::adsfi::MsgPoint3fVec points;
    ::int32_t video_third_person{ 0 };
    ::ara::adsfi::AppVideoSplitParamVec split_area_parames;
    ::int32_t auto_control_assist{ 0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(front_source);
        fun(output_mode);
        fun(adaptive_mode);
        fun(manual_bps);
        fun(manual_width);
        fun(manual_height);
        fun(remote_control_assist);
        fun(remote_control_alarm);
        fun(split_adaptive);
        fun(manual_split_main);
        fun(manual_split_left);
        fun(manual_split_right);
        fun(receiver_reported_fps);
        fun(receiver_reported_bps);
        fun(receiver_reported_ts);
        fun(points);
        fun(video_third_person);
        fun(split_area_parames);
        fun(auto_control_assist);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(front_source);
        fun(output_mode);
        fun(adaptive_mode);
        fun(manual_bps);
        fun(manual_width);
        fun(manual_height);
        fun(remote_control_assist);
        fun(remote_control_alarm);
        fun(split_adaptive);
        fun(manual_split_main);
        fun(manual_split_left);
        fun(manual_split_right);
        fun(receiver_reported_fps);
        fun(receiver_reported_bps);
        fun(receiver_reported_ts);
        fun(points);
        fun(video_third_person);
        fun(split_area_parames);
        fun(auto_control_assist);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("front_source", front_source);
        fun("output_mode", output_mode);
        fun("adaptive_mode", adaptive_mode);
        fun("manual_bps", manual_bps);
        fun("manual_width", manual_width);
        fun("manual_height", manual_height);
        fun("remote_control_assist", remote_control_assist);
        fun("remote_control_alarm", remote_control_alarm);
        fun("split_adaptive", split_adaptive);
        fun("manual_split_main", manual_split_main);
        fun("manual_split_left", manual_split_left);
        fun("manual_split_right", manual_split_right);
        fun("receiver_reported_fps", receiver_reported_fps);
        fun("receiver_reported_bps", receiver_reported_bps);
        fun("receiver_reported_ts", receiver_reported_ts);
        fun("points", points);
        fun("video_third_person", video_third_person);
        fun("split_area_parames", split_area_parames);
        fun("auto_control_assist", auto_control_assist);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("front_source", front_source);
        fun("output_mode", output_mode);
        fun("adaptive_mode", adaptive_mode);
        fun("manual_bps", manual_bps);
        fun("manual_width", manual_width);
        fun("manual_height", manual_height);
        fun("remote_control_assist", remote_control_assist);
        fun("remote_control_alarm", remote_control_alarm);
        fun("split_adaptive", split_adaptive);
        fun("manual_split_main", manual_split_main);
        fun("manual_split_left", manual_split_left);
        fun("manual_split_right", manual_split_right);
        fun("receiver_reported_fps", receiver_reported_fps);
        fun("receiver_reported_bps", receiver_reported_bps);
        fun("receiver_reported_ts", receiver_reported_ts);
        fun("points", points);
        fun("video_third_person", video_third_person);
        fun("split_area_parames", split_area_parames);
        fun("auto_control_assist", auto_control_assist);
    }

    friend bool operator==(const ::ara::adsfi::AppVideoProfile& lhs, const ::ara::adsfi::AppVideoProfile& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.front_source == rhs.front_source) && (lhs.output_mode == rhs.output_mode) && (lhs.adaptive_mode == rhs.adaptive_mode) && (lhs.manual_bps == rhs.manual_bps) && (fabs(static_cast<double>(lhs.manual_width - rhs.manual_width)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.manual_height - rhs.manual_height)) < DBL_EPSILON) && (lhs.remote_control_assist == rhs.remote_control_assist) && (lhs.remote_control_alarm == rhs.remote_control_alarm) && (lhs.split_adaptive == rhs.split_adaptive) && (lhs.manual_split_main == rhs.manual_split_main) && (lhs.manual_split_left == rhs.manual_split_left) && (lhs.manual_split_right == rhs.manual_split_right) && (fabs(static_cast<double>(lhs.receiver_reported_fps - rhs.receiver_reported_fps)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.receiver_reported_bps - rhs.receiver_reported_bps)) < DBL_EPSILON) && (lhs.receiver_reported_ts == rhs.receiver_reported_ts) && (lhs.points == rhs.points) && (lhs.video_third_person == rhs.video_third_person) && (lhs.split_area_parames == rhs.split_area_parames) && (lhs.auto_control_assist == rhs.auto_control_assist);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPVIDEOPROFILE_H
