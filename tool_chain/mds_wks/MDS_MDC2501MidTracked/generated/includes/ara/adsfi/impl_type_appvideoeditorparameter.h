/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPVIDEOEDITORPARAMETER_H
#define ARA_ADSFI_IMPL_TYPE_APPVIDEOEDITORPARAMETER_H

#include "impl_type_int32_t.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msgpoint3dvec.h"
#include "ara/adsfi/impl_type_msgpoint3d.h"
#include "ara/adsfi/impl_type_appvideosplitparamvec.h"

namespace ara {
namespace adsfi {
struct AppVideoEditorParameter {
    ::int32_t output_mode{ 0 };
    ::int32_t remote_control_alarm{ 0 };
    ::int32_t remote_control_assist{ 0 };
    ::int32_t front_source{ 0 };
    ::int32_t split_adaptive{ 0 };
    ::int32_t video_left_{ 0 };
    ::int32_t video_main_{ 0 };
    ::int32_t video_right_{ 0 };
    ::ara::adsfi::MsgHafFusionOutArray objects;
    ::ara::adsfi::MsgHafEgoTrajectory remote_control;
    ::ara::adsfi::MsgHafEgoTrajectory path;
    ::ara::adsfi::MsgPoint3DVec target_points;
    ::int32_t video_third_person{ 0 };
    ::ara::adsfi::MsgPoint3D target_point;
    ::ara::adsfi::AppVideoSplitParamVec split_parames_;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(output_mode);
        fun(remote_control_alarm);
        fun(remote_control_assist);
        fun(front_source);
        fun(split_adaptive);
        fun(video_left_);
        fun(video_main_);
        fun(video_right_);
        fun(objects);
        fun(remote_control);
        fun(path);
        fun(target_points);
        fun(video_third_person);
        fun(target_point);
        fun(split_parames_);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(output_mode);
        fun(remote_control_alarm);
        fun(remote_control_assist);
        fun(front_source);
        fun(split_adaptive);
        fun(video_left_);
        fun(video_main_);
        fun(video_right_);
        fun(objects);
        fun(remote_control);
        fun(path);
        fun(target_points);
        fun(video_third_person);
        fun(target_point);
        fun(split_parames_);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("output_mode", output_mode);
        fun("remote_control_alarm", remote_control_alarm);
        fun("remote_control_assist", remote_control_assist);
        fun("front_source", front_source);
        fun("split_adaptive", split_adaptive);
        fun("video_left_", video_left_);
        fun("video_main_", video_main_);
        fun("video_right_", video_right_);
        fun("objects", objects);
        fun("remote_control", remote_control);
        fun("path", path);
        fun("target_points", target_points);
        fun("video_third_person", video_third_person);
        fun("target_point", target_point);
        fun("split_parames_", split_parames_);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("output_mode", output_mode);
        fun("remote_control_alarm", remote_control_alarm);
        fun("remote_control_assist", remote_control_assist);
        fun("front_source", front_source);
        fun("split_adaptive", split_adaptive);
        fun("video_left_", video_left_);
        fun("video_main_", video_main_);
        fun("video_right_", video_right_);
        fun("objects", objects);
        fun("remote_control", remote_control);
        fun("path", path);
        fun("target_points", target_points);
        fun("video_third_person", video_third_person);
        fun("target_point", target_point);
        fun("split_parames_", split_parames_);
    }

    friend bool operator==(const ::ara::adsfi::AppVideoEditorParameter& lhs, const ::ara::adsfi::AppVideoEditorParameter& rhs) noexcept
    {
        return (lhs.output_mode == rhs.output_mode) && (lhs.remote_control_alarm == rhs.remote_control_alarm) && (lhs.remote_control_assist == rhs.remote_control_assist) && (lhs.front_source == rhs.front_source) && (lhs.split_adaptive == rhs.split_adaptive) && (lhs.video_left_ == rhs.video_left_) && (lhs.video_main_ == rhs.video_main_) && (lhs.video_right_ == rhs.video_right_) && (lhs.objects == rhs.objects) && (lhs.remote_control == rhs.remote_control) && (lhs.path == rhs.path) && (lhs.target_points == rhs.target_points) && (lhs.video_third_person == rhs.video_third_person) && (lhs.target_point == rhs.target_point) && (lhs.split_parames_ == rhs.split_parames_);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPVIDEOEDITORPARAMETER_H
