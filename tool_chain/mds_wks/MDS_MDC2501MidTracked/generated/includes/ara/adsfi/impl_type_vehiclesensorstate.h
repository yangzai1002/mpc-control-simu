/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VEHICLESENSORSTATE_H
#define ARA_ADSFI_IMPL_TYPE_VEHICLESENSORSTATE_H

#include <cfloat>
#include <cmath>
#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct VehicleSensorState {
    ::uint8_t VCU_fault{ 0U };
    float vehicle_total_mile{ 0.0F };
    ::uint8_t vehicle_ready_status{ 0U };
    ::uint8_t offroad_working_mode{ 0U };
    ::uint8_t parking_mode{ 0U };
    ::uint8_t motor_mode{ 0U };
    ::uint8_t power_mode{ 0U };
    ::uint8_t speed_limit{ 0U };
    ::uint8_t power_status{ 0U };
    ::uint8_t low_beam_status{ 0U };
    ::uint8_t far_beam_status{ 0U };
    ::uint8_t turn_left_light_status{ 0U };
    ::uint8_t turn_right_light_status{ 0U };
    ::uint8_t lidar_power_status{ 0U };
    ::uint8_t radar_power_status{ 0U };
    ::uint8_t night_camera_power_status{ 0U };
    ::uint8_t horn_status{ 0U };
    float fuel_level{ 0.0F };
    float remaining_mileage{ 0.0F };
    ::uint8_t bms_battery_soc{ 0U };
    float voltage{ 0.0F };

    static bool IsPlane()
    {
        return true;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(VCU_fault);
        fun(vehicle_total_mile);
        fun(vehicle_ready_status);
        fun(offroad_working_mode);
        fun(parking_mode);
        fun(motor_mode);
        fun(power_mode);
        fun(speed_limit);
        fun(power_status);
        fun(low_beam_status);
        fun(far_beam_status);
        fun(turn_left_light_status);
        fun(turn_right_light_status);
        fun(lidar_power_status);
        fun(radar_power_status);
        fun(night_camera_power_status);
        fun(horn_status);
        fun(fuel_level);
        fun(remaining_mileage);
        fun(bms_battery_soc);
        fun(voltage);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(VCU_fault);
        fun(vehicle_total_mile);
        fun(vehicle_ready_status);
        fun(offroad_working_mode);
        fun(parking_mode);
        fun(motor_mode);
        fun(power_mode);
        fun(speed_limit);
        fun(power_status);
        fun(low_beam_status);
        fun(far_beam_status);
        fun(turn_left_light_status);
        fun(turn_right_light_status);
        fun(lidar_power_status);
        fun(radar_power_status);
        fun(night_camera_power_status);
        fun(horn_status);
        fun(fuel_level);
        fun(remaining_mileage);
        fun(bms_battery_soc);
        fun(voltage);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("VCU_fault", VCU_fault);
        fun("vehicle_total_mile", vehicle_total_mile);
        fun("vehicle_ready_status", vehicle_ready_status);
        fun("offroad_working_mode", offroad_working_mode);
        fun("parking_mode", parking_mode);
        fun("motor_mode", motor_mode);
        fun("power_mode", power_mode);
        fun("speed_limit", speed_limit);
        fun("power_status", power_status);
        fun("low_beam_status", low_beam_status);
        fun("far_beam_status", far_beam_status);
        fun("turn_left_light_status", turn_left_light_status);
        fun("turn_right_light_status", turn_right_light_status);
        fun("lidar_power_status", lidar_power_status);
        fun("radar_power_status", radar_power_status);
        fun("night_camera_power_status", night_camera_power_status);
        fun("horn_status", horn_status);
        fun("fuel_level", fuel_level);
        fun("remaining_mileage", remaining_mileage);
        fun("bms_battery_soc", bms_battery_soc);
        fun("voltage", voltage);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("VCU_fault", VCU_fault);
        fun("vehicle_total_mile", vehicle_total_mile);
        fun("vehicle_ready_status", vehicle_ready_status);
        fun("offroad_working_mode", offroad_working_mode);
        fun("parking_mode", parking_mode);
        fun("motor_mode", motor_mode);
        fun("power_mode", power_mode);
        fun("speed_limit", speed_limit);
        fun("power_status", power_status);
        fun("low_beam_status", low_beam_status);
        fun("far_beam_status", far_beam_status);
        fun("turn_left_light_status", turn_left_light_status);
        fun("turn_right_light_status", turn_right_light_status);
        fun("lidar_power_status", lidar_power_status);
        fun("radar_power_status", radar_power_status);
        fun("night_camera_power_status", night_camera_power_status);
        fun("horn_status", horn_status);
        fun("fuel_level", fuel_level);
        fun("remaining_mileage", remaining_mileage);
        fun("bms_battery_soc", bms_battery_soc);
        fun("voltage", voltage);
    }

    friend bool operator==(const ::ara::adsfi::VehicleSensorState& lhs, const ::ara::adsfi::VehicleSensorState& rhs) noexcept
    {
        return (lhs.VCU_fault == rhs.VCU_fault) && (fabs(static_cast<double>(lhs.vehicle_total_mile - rhs.vehicle_total_mile)) < DBL_EPSILON) && (lhs.vehicle_ready_status == rhs.vehicle_ready_status) && (lhs.offroad_working_mode == rhs.offroad_working_mode) && (lhs.parking_mode == rhs.parking_mode) && (lhs.motor_mode == rhs.motor_mode) && (lhs.power_mode == rhs.power_mode) && (lhs.speed_limit == rhs.speed_limit) && (lhs.power_status == rhs.power_status) && (lhs.low_beam_status == rhs.low_beam_status) && (lhs.far_beam_status == rhs.far_beam_status) && (lhs.turn_left_light_status == rhs.turn_left_light_status) && (lhs.turn_right_light_status == rhs.turn_right_light_status) && (lhs.lidar_power_status == rhs.lidar_power_status) && (lhs.radar_power_status == rhs.radar_power_status) && (lhs.night_camera_power_status == rhs.night_camera_power_status) && (lhs.horn_status == rhs.horn_status) && (fabs(static_cast<double>(lhs.fuel_level - rhs.fuel_level)) < DBL_EPSILON) && (fabs(static_cast<double>(lhs.remaining_mileage - rhs.remaining_mileage)) < DBL_EPSILON) && (lhs.bms_battery_soc == rhs.bms_battery_soc) && (fabs(static_cast<double>(lhs.voltage - rhs.voltage)) < DBL_EPSILON);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VEHICLESENSORSTATE_H
