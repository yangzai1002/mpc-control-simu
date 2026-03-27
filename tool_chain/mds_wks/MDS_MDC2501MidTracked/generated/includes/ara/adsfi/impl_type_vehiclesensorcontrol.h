/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_VEHICLESENSORCONTROL_H
#define ARA_ADSFI_IMPL_TYPE_VEHICLESENSORCONTROL_H

#include "ara/adsfi/impl_type_commonheader.h"
#include "impl_type_uint8_t.h"

namespace ara {
namespace adsfi {
struct VehicleSensorControl {
    ::ara::adsfi::CommonHeader header;
    ::uint8_t chassis_enable{ 0U };
    ::uint8_t operate_mode{ 0U };
    ::uint8_t motor_mode{ 0U };
    ::uint8_t motor_power{ 0U };
    ::uint8_t limit_speed{ 0U };
    ::uint8_t high_pressure{ 0U };
    ::uint8_t motor_drive_mode{ 0U };
    ::uint8_t offroad_mode{ 0U };
    ::uint8_t parking_mode{ 0U };
    ::uint8_t pump_control1{ 0U };
    ::uint8_t pump_control2{ 0U };
    ::uint8_t flann_control{ 0U };
    ::uint8_t headlights_low_beam{ 0U };
    ::uint8_t headlights_hight_beam{ 0U };
    ::uint8_t turnleft_light{ 0U };
    ::uint8_t turnright_light{ 0U };
    ::uint8_t position_light{ 0U };
    ::uint8_t double_floash{ 0U };
    ::uint8_t horn{ 0U };
    ::uint8_t drainage_pump{ 0U };
    ::uint8_t radar_control{ 0U };
    ::uint8_t lidar_control{ 0U };
    ::uint8_t night_vision_control{ 0U };
    ::uint8_t domain_control{ 0U };
    ::uint8_t MDC_temp{ 0U };
    ::uint8_t reserve1{ 0U };
    ::uint8_t reserve2{ 0U };
    ::uint8_t battery_heating{ 0U };
    ::uint8_t power_out_control{ 0U };
    ::uint8_t battery_warming{ 0U };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(header);
        fun(chassis_enable);
        fun(operate_mode);
        fun(motor_mode);
        fun(motor_power);
        fun(limit_speed);
        fun(high_pressure);
        fun(motor_drive_mode);
        fun(offroad_mode);
        fun(parking_mode);
        fun(pump_control1);
        fun(pump_control2);
        fun(flann_control);
        fun(headlights_low_beam);
        fun(headlights_hight_beam);
        fun(turnleft_light);
        fun(turnright_light);
        fun(position_light);
        fun(double_floash);
        fun(horn);
        fun(drainage_pump);
        fun(radar_control);
        fun(lidar_control);
        fun(night_vision_control);
        fun(domain_control);
        fun(MDC_temp);
        fun(reserve1);
        fun(reserve2);
        fun(battery_heating);
        fun(power_out_control);
        fun(battery_warming);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(header);
        fun(chassis_enable);
        fun(operate_mode);
        fun(motor_mode);
        fun(motor_power);
        fun(limit_speed);
        fun(high_pressure);
        fun(motor_drive_mode);
        fun(offroad_mode);
        fun(parking_mode);
        fun(pump_control1);
        fun(pump_control2);
        fun(flann_control);
        fun(headlights_low_beam);
        fun(headlights_hight_beam);
        fun(turnleft_light);
        fun(turnright_light);
        fun(position_light);
        fun(double_floash);
        fun(horn);
        fun(drainage_pump);
        fun(radar_control);
        fun(lidar_control);
        fun(night_vision_control);
        fun(domain_control);
        fun(MDC_temp);
        fun(reserve1);
        fun(reserve2);
        fun(battery_heating);
        fun(power_out_control);
        fun(battery_warming);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("header", header);
        fun("chassis_enable", chassis_enable);
        fun("operate_mode", operate_mode);
        fun("motor_mode", motor_mode);
        fun("motor_power", motor_power);
        fun("limit_speed", limit_speed);
        fun("high_pressure", high_pressure);
        fun("motor_drive_mode", motor_drive_mode);
        fun("offroad_mode", offroad_mode);
        fun("parking_mode", parking_mode);
        fun("pump_control1", pump_control1);
        fun("pump_control2", pump_control2);
        fun("flann_control", flann_control);
        fun("headlights_low_beam", headlights_low_beam);
        fun("headlights_hight_beam", headlights_hight_beam);
        fun("turnleft_light", turnleft_light);
        fun("turnright_light", turnright_light);
        fun("position_light", position_light);
        fun("double_floash", double_floash);
        fun("horn", horn);
        fun("drainage_pump", drainage_pump);
        fun("radar_control", radar_control);
        fun("lidar_control", lidar_control);
        fun("night_vision_control", night_vision_control);
        fun("domain_control", domain_control);
        fun("MDC_temp", MDC_temp);
        fun("reserve1", reserve1);
        fun("reserve2", reserve2);
        fun("battery_heating", battery_heating);
        fun("power_out_control", power_out_control);
        fun("battery_warming", battery_warming);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("header", header);
        fun("chassis_enable", chassis_enable);
        fun("operate_mode", operate_mode);
        fun("motor_mode", motor_mode);
        fun("motor_power", motor_power);
        fun("limit_speed", limit_speed);
        fun("high_pressure", high_pressure);
        fun("motor_drive_mode", motor_drive_mode);
        fun("offroad_mode", offroad_mode);
        fun("parking_mode", parking_mode);
        fun("pump_control1", pump_control1);
        fun("pump_control2", pump_control2);
        fun("flann_control", flann_control);
        fun("headlights_low_beam", headlights_low_beam);
        fun("headlights_hight_beam", headlights_hight_beam);
        fun("turnleft_light", turnleft_light);
        fun("turnright_light", turnright_light);
        fun("position_light", position_light);
        fun("double_floash", double_floash);
        fun("horn", horn);
        fun("drainage_pump", drainage_pump);
        fun("radar_control", radar_control);
        fun("lidar_control", lidar_control);
        fun("night_vision_control", night_vision_control);
        fun("domain_control", domain_control);
        fun("MDC_temp", MDC_temp);
        fun("reserve1", reserve1);
        fun("reserve2", reserve2);
        fun("battery_heating", battery_heating);
        fun("power_out_control", power_out_control);
        fun("battery_warming", battery_warming);
    }

    friend bool operator==(const ::ara::adsfi::VehicleSensorControl& lhs, const ::ara::adsfi::VehicleSensorControl& rhs) noexcept
    {
        return (lhs.header == rhs.header) && (lhs.chassis_enable == rhs.chassis_enable) && (lhs.operate_mode == rhs.operate_mode) && (lhs.motor_mode == rhs.motor_mode) && (lhs.motor_power == rhs.motor_power) && (lhs.limit_speed == rhs.limit_speed) && (lhs.high_pressure == rhs.high_pressure) && (lhs.motor_drive_mode == rhs.motor_drive_mode) && (lhs.offroad_mode == rhs.offroad_mode) && (lhs.parking_mode == rhs.parking_mode) && (lhs.pump_control1 == rhs.pump_control1) && (lhs.pump_control2 == rhs.pump_control2) && (lhs.flann_control == rhs.flann_control) && (lhs.headlights_low_beam == rhs.headlights_low_beam) && (lhs.headlights_hight_beam == rhs.headlights_hight_beam) && (lhs.turnleft_light == rhs.turnleft_light) && (lhs.turnright_light == rhs.turnright_light) && (lhs.position_light == rhs.position_light) && (lhs.double_floash == rhs.double_floash) && (lhs.horn == rhs.horn) && (lhs.drainage_pump == rhs.drainage_pump) && (lhs.radar_control == rhs.radar_control) && (lhs.lidar_control == rhs.lidar_control) && (lhs.night_vision_control == rhs.night_vision_control) && (lhs.domain_control == rhs.domain_control) && (lhs.MDC_temp == rhs.MDC_temp) && (lhs.reserve1 == rhs.reserve1) && (lhs.reserve2 == rhs.reserve2) && (lhs.battery_heating == rhs.battery_heating) && (lhs.power_out_control == rhs.power_out_control) && (lhs.battery_warming == rhs.battery_warming);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_VEHICLESENSORCONTROL_H
