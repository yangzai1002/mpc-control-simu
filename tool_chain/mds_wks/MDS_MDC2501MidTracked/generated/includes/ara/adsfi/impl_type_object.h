/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_OBJECT_H
#define ARA_ADSFI_IMPL_TYPE_OBJECT_H

#include <cfloat>
#include <cmath>
#include "ara/common/impl_type_commonheader.h"
#include "impl_type_float.h"
#include "impl_type_int32.h"
#include "impl_type_uint8.h"
#include "impl_type_string.h"
#include "impl_type_point.h"
#include "impl_type_rect.h"
#include "impl_type_polygon.h"
#include "impl_type_pointarray.h"
#include "impl_type_twistwithcovariance.h"
#include "impl_type_pathwithcovariancearray.h"
#include "impl_type_int32array.h"
#include "impl_type_uint8array.h"
#include "impl_type_floatarray.h"
#include "impl_type_matrix3d.h"

namespace ara {
namespace adsfi {
struct Object {
    ::ara::common::CommonHeader header;
    ::Float existence_probability{ 0.0F };
    ::Int32 id{ 0 };
    ::ara::common::CommonHeader time_last_update;
    ::ara::common::CommonHeader time_creation;
    ::Int32 measurement_age_count{ 0 };
    ::UInt8 classification{ 0U };
    ::String classification_description;
    ::Float classification_confidence{ 0.0F };
    ::Float classification_age_seconds{ 0.0F };
    ::Int32 classification_age_count{ 0 };
    ::Point object_box_center;
    ::Point object_box_center_absolute;
    ::Point object_box_center_covariance;
    ::Point object_box_size;
    ::Point object_box_size_covariance;
    ::Rect box_image;
    ::Float object_box_orientation{ 0.0F };
    ::Float object_box_orientation_absolute{ 0.0F };
    ::Float object_box_orientation_covariance{ 0.0F };
    ::Polygon box_polygon;
    ::PointArray box_polygon_absolute;
    ::Point reference_point;
    ::Point reference_point_covariance;
    ::TwistWithCovariance relative_velocity;
    ::TwistWithCovariance absolute_velocity;
    ::TwistWithCovariance enu_velocity;
    ::PointArray sl_velocity;
    ::PointArray contour_points_absolute;
    ::Polygon contour_points;
    ::PathWithCovarianceArray intention_paths;
    ::Float intention_time_step{ 0.0F };
    ::Int32Array lane_id;
    ::Uint8Array lane_type;
    ::PointArray position_in_lane;
    ::FloatArray length_in_lane;
    ::FloatArray width_in_lane;
    ::Int32 road_id{ 0 };
    ::UInt8 road_type{ 0U };
    ::UInt8 types{ 0U };
    ::UInt8 cipv_flag{ 0U };
    ::UInt8 fusion_type{ 0U };
    ::UInt8 blinker_flag{ 0U };
    ::Point acceleration;
    ::Point abs_acceleration;
    ::Point anchor_point;
    ::Point abs_anchor_point;
    ::Matrix3d acceleration_covariance{};
    ::Matrix3d abs_acceleration_covariance{};
    ::Matrix3d velocity_covariance{};
    ::Matrix3d abs_velocity_covariance{};
    ::UInt8 blinkerStatus{ 0U };
    ::UInt8 cameraStatus{ 0U };
    ::UInt8 coordinate{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(existence_probability);
        fun(id);
        fun(time_last_update);
        fun(time_creation);
        fun(measurement_age_count);
        fun(classification);
        fun(classification_description);
        fun(classification_confidence);
        fun(classification_age_seconds);
        fun(classification_age_count);
        fun(object_box_center);
        fun(object_box_center_absolute);
        fun(object_box_center_covariance);
        fun(object_box_size);
        fun(object_box_size_covariance);
        fun(box_image);
        fun(object_box_orientation);
        fun(object_box_orientation_absolute);
        fun(object_box_orientation_covariance);
        fun(box_polygon);
        fun(box_polygon_absolute);
        fun(reference_point);
        fun(reference_point_covariance);
        fun(relative_velocity);
        fun(absolute_velocity);
        fun(enu_velocity);
        fun(sl_velocity);
        fun(contour_points_absolute);
        fun(contour_points);
        fun(intention_paths);
        fun(intention_time_step);
        fun(lane_id);
        fun(lane_type);
        fun(position_in_lane);
        fun(length_in_lane);
        fun(width_in_lane);
        fun(road_id);
        fun(road_type);
        fun(types);
        fun(cipv_flag);
        fun(fusion_type);
        fun(blinker_flag);
        fun(acceleration);
        fun(abs_acceleration);
        fun(anchor_point);
        fun(abs_anchor_point);
        fun(acceleration_covariance);
        fun(abs_acceleration_covariance);
        fun(velocity_covariance);
        fun(abs_velocity_covariance);
        fun(blinkerStatus);
        fun(cameraStatus);
        fun(coordinate);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(existence_probability);
        fun(id);
        fun(time_last_update);
        fun(time_creation);
        fun(measurement_age_count);
        fun(classification);
        fun(classification_description);
        fun(classification_confidence);
        fun(classification_age_seconds);
        fun(classification_age_count);
        fun(object_box_center);
        fun(object_box_center_absolute);
        fun(object_box_center_covariance);
        fun(object_box_size);
        fun(object_box_size_covariance);
        fun(box_image);
        fun(object_box_orientation);
        fun(object_box_orientation_absolute);
        fun(object_box_orientation_covariance);
        fun(box_polygon);
        fun(box_polygon_absolute);
        fun(reference_point);
        fun(reference_point_covariance);
        fun(relative_velocity);
        fun(absolute_velocity);
        fun(enu_velocity);
        fun(sl_velocity);
        fun(contour_points_absolute);
        fun(contour_points);
        fun(intention_paths);
        fun(intention_time_step);
        fun(lane_id);
        fun(lane_type);
        fun(position_in_lane);
        fun(length_in_lane);
        fun(width_in_lane);
        fun(road_id);
        fun(road_type);
        fun(types);
        fun(cipv_flag);
        fun(fusion_type);
        fun(blinker_flag);
        fun(acceleration);
        fun(abs_acceleration);
        fun(anchor_point);
        fun(abs_anchor_point);
        fun(acceleration_covariance);
        fun(abs_acceleration_covariance);
        fun(velocity_covariance);
        fun(abs_velocity_covariance);
        fun(blinkerStatus);
        fun(cameraStatus);
        fun(coordinate);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("existence_probability", existence_probability);
        fun("id", id);
        fun("time_last_update", time_last_update);
        fun("time_creation", time_creation);
        fun("measurement_age_count", measurement_age_count);
        fun("classification", classification);
        fun("classification_description", classification_description);
        fun("classification_confidence", classification_confidence);
        fun("classification_age_seconds", classification_age_seconds);
        fun("classification_age_count", classification_age_count);
        fun("object_box_center", object_box_center);
        fun("object_box_center_absolute", object_box_center_absolute);
        fun("object_box_center_covariance", object_box_center_covariance);
        fun("object_box_size", object_box_size);
        fun("object_box_size_covariance", object_box_size_covariance);
        fun("box_image", box_image);
        fun("object_box_orientation", object_box_orientation);
        fun("object_box_orientation_absolute", object_box_orientation_absolute);
        fun("object_box_orientation_covariance", object_box_orientation_covariance);
        fun("box_polygon", box_polygon);
        fun("box_polygon_absolute", box_polygon_absolute);
        fun("reference_point", reference_point);
        fun("reference_point_covariance", reference_point_covariance);
        fun("relative_velocity", relative_velocity);
        fun("absolute_velocity", absolute_velocity);
        fun("enu_velocity", enu_velocity);
        fun("sl_velocity", sl_velocity);
        fun("contour_points_absolute", contour_points_absolute);
        fun("contour_points", contour_points);
        fun("intention_paths", intention_paths);
        fun("intention_time_step", intention_time_step);
        fun("lane_id", lane_id);
        fun("lane_type", lane_type);
        fun("position_in_lane", position_in_lane);
        fun("length_in_lane", length_in_lane);
        fun("width_in_lane", width_in_lane);
        fun("road_id", road_id);
        fun("road_type", road_type);
        fun("types", types);
        fun("cipv_flag", cipv_flag);
        fun("fusion_type", fusion_type);
        fun("blinker_flag", blinker_flag);
        fun("acceleration", acceleration);
        fun("abs_acceleration", abs_acceleration);
        fun("anchor_point", anchor_point);
        fun("abs_anchor_point", abs_anchor_point);
        fun("acceleration_covariance", acceleration_covariance);
        fun("abs_acceleration_covariance", abs_acceleration_covariance);
        fun("velocity_covariance", velocity_covariance);
        fun("abs_velocity_covariance", abs_velocity_covariance);
        fun("blinkerStatus", blinkerStatus);
        fun("cameraStatus", cameraStatus);
        fun("coordinate", coordinate);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("existence_probability", existence_probability);
        fun("id", id);
        fun("time_last_update", time_last_update);
        fun("time_creation", time_creation);
        fun("measurement_age_count", measurement_age_count);
        fun("classification", classification);
        fun("classification_description", classification_description);
        fun("classification_confidence", classification_confidence);
        fun("classification_age_seconds", classification_age_seconds);
        fun("classification_age_count", classification_age_count);
        fun("object_box_center", object_box_center);
        fun("object_box_center_absolute", object_box_center_absolute);
        fun("object_box_center_covariance", object_box_center_covariance);
        fun("object_box_size", object_box_size);
        fun("object_box_size_covariance", object_box_size_covariance);
        fun("box_image", box_image);
        fun("object_box_orientation", object_box_orientation);
        fun("object_box_orientation_absolute", object_box_orientation_absolute);
        fun("object_box_orientation_covariance", object_box_orientation_covariance);
        fun("box_polygon", box_polygon);
        fun("box_polygon_absolute", box_polygon_absolute);
        fun("reference_point", reference_point);
        fun("reference_point_covariance", reference_point_covariance);
        fun("relative_velocity", relative_velocity);
        fun("absolute_velocity", absolute_velocity);
        fun("enu_velocity", enu_velocity);
        fun("sl_velocity", sl_velocity);
        fun("contour_points_absolute", contour_points_absolute);
        fun("contour_points", contour_points);
        fun("intention_paths", intention_paths);
        fun("intention_time_step", intention_time_step);
        fun("lane_id", lane_id);
        fun("lane_type", lane_type);
        fun("position_in_lane", position_in_lane);
        fun("length_in_lane", length_in_lane);
        fun("width_in_lane", width_in_lane);
        fun("road_id", road_id);
        fun("road_type", road_type);
        fun("types", types);
        fun("cipv_flag", cipv_flag);
        fun("fusion_type", fusion_type);
        fun("blinker_flag", blinker_flag);
        fun("acceleration", acceleration);
        fun("abs_acceleration", abs_acceleration);
        fun("anchor_point", anchor_point);
        fun("abs_anchor_point", abs_anchor_point);
        fun("acceleration_covariance", acceleration_covariance);
        fun("abs_acceleration_covariance", abs_acceleration_covariance);
        fun("velocity_covariance", velocity_covariance);
        fun("abs_velocity_covariance", abs_velocity_covariance);
        fun("blinkerStatus", blinkerStatus);
        fun("cameraStatus", cameraStatus);
        fun("coordinate", coordinate);
    }

    friend bool operator==(const ::ara::adsfi::Object& lhs, const ::ara::adsfi::Object& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (fabs(static_cast<double>(lhs.existence_probability - rhs.existence_probability)) < DBL_EPSILON) && (lhs.id == rhs.id) && (lhs.time_last_update == rhs.time_last_update) && (lhs.time_creation == rhs.time_creation) && (lhs.measurement_age_count == rhs.measurement_age_count) && (lhs.classification == rhs.classification) && (lhs.classification_description == rhs.classification_description) && (fabs(static_cast<double>(lhs.classification_confidence - rhs.classification_confidence)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.classification_age_seconds - rhs.classification_age_seconds)) < DBL_EPSILON) && (lhs.classification_age_count == rhs.classification_age_count) && (lhs.object_box_center == rhs.object_box_center) && (lhs.object_box_center_absolute == rhs.object_box_center_absolute) && (lhs.object_box_center_covariance == rhs.object_box_center_covariance) && (lhs.object_box_size == rhs.object_box_size) && (lhs.object_box_size_covariance == rhs.object_box_size_covariance) && (lhs.box_image == rhs.box_image) && (fabs(static_cast<double>(lhs.object_box_orientation - rhs.object_box_orientation)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.object_box_orientation_absolute - rhs.object_box_orientation_absolute)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.object_box_orientation_covariance - rhs.object_box_orientation_covariance)) < DBL_EPSILON) && (lhs.box_polygon == rhs.box_polygon) && (lhs.box_polygon_absolute == rhs.box_polygon_absolute) && (lhs.reference_point == rhs.reference_point) && (lhs.reference_point_covariance == rhs.reference_point_covariance) && (lhs.relative_velocity == rhs.relative_velocity) && (lhs.absolute_velocity == rhs.absolute_velocity) && (lhs.enu_velocity == rhs.enu_velocity) && (lhs.sl_velocity == rhs.sl_velocity) && (lhs.contour_points_absolute == rhs.contour_points_absolute) && (lhs.contour_points == rhs.contour_points) && (lhs.intention_paths == rhs.intention_paths) && (fabs(static_cast<double>(lhs.intention_time_step - rhs.intention_time_step)) < DBL_EPSILON) && (lhs.lane_id == rhs.lane_id) && (lhs.lane_type == rhs.lane_type) && (lhs.position_in_lane == rhs.position_in_lane) && (lhs.length_in_lane == rhs.length_in_lane) && (lhs.width_in_lane == rhs.width_in_lane) && (lhs.road_id == rhs.road_id) && (lhs.road_type == rhs.road_type) && (lhs.types == rhs.types) && (lhs.cipv_flag == rhs.cipv_flag) && (lhs.fusion_type == rhs.fusion_type) && (lhs.blinker_flag == rhs.blinker_flag) && (lhs.acceleration == rhs.acceleration) && (lhs.abs_acceleration == rhs.abs_acceleration) && (lhs.anchor_point == rhs.anchor_point) && (lhs.abs_anchor_point == rhs.abs_anchor_point) && (lhs.acceleration_covariance == rhs.acceleration_covariance) && (lhs.abs_acceleration_covariance == rhs.abs_acceleration_covariance) && (lhs.velocity_covariance == rhs.velocity_covariance) && (lhs.abs_velocity_covariance == rhs.abs_velocity_covariance) && (lhs.blinkerStatus == rhs.blinkerStatus) && (lhs.cameraStatus == rhs.cameraStatus) && (lhs.coordinate == rhs.coordinate);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_OBJECT_H
