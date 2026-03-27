#ifndef __AIAN_CANBUS_SEND_H__
#define __AIAN_CANBUS_SEND_H__

#include <mutex>
#include <thread>

#include "SocketCan.hpp"
#include "aion_can_send_protocol.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "config_loader/custom_stack.h"
#include "log/log.h"

#define CPU_41 0x41 // TBD
#define CPU_40 0x40 // TBD
#define CPU_31 0x31 // TBD
#define CPU_30 0x30 // TBD

class AionCanbusDataInteract {
private:
    AionCanbusDataInteract() {
    }

public:
    void SetVehicleActControl(const ara::adsfi::VehicleActControl& act_ctl, bool estop) {
        std::unique_lock<std::mutex> locker(mutex_);

        lon_ctrl_.target_torque = act_ctl.lon_control.target_torque;
        lon_ctrl_.target_pressure = act_ctl.lon_control.target_pressure;
        lon_ctrl_.actuator_mode = act_ctl.lon_control.actuator_mode;
        lon_ctrl_.shift_position = act_ctl.lon_control.shift_position;

        lat_ctrl_.target_angle = act_ctl.lat_control.target_angle;
        lat_ctrl_.target_torque = act_ctl.lat_control.target_torque;

        if (estop) {
            lat_ctrl_.target_torque = 500;

            lon_ctrl_.target_torque = 0;
            lon_ctrl_.target_pressure = 50;
            lon_ctrl_.actuator_mode = 0;
            lon_ctrl_.shift_position = 0;
        }

        // static int i = 0;
        // i++;
        // if (i < 100) {
        // 	lon_ctrl_.target_pressure = 50;
        // 	lon_ctrl_.actuator_mode = 0;
        // 	lon_ctrl_.shift_position = 1;
        // } else {
        // 	lon_ctrl_.target_torque = 300;
        // 	lon_ctrl_.actuator_mode = 1;
        // 	lon_ctrl_.shift_position = 1;
        // }

        AINFO << "lon_ctrl_.target_torque:" << lon_ctrl_.target_torque;
        AINFO << "lon_ctrl_.target_pressure:" << lon_ctrl_.target_pressure;
        AINFO << "lon_ctrl_.actuator_mode:" << lon_ctrl_.actuator_mode;
        AINFO << "lon_ctrl_.shift_position:" << lon_ctrl_.shift_position;
        AINFO << "lat_ctrl_.target_angle:" << lat_ctrl_.target_angle;
        AINFO << "lat_ctrl_.target_torque:" << lat_ctrl_.target_torque;
    }

    void SetVehicleSensorControl(const ara::adsfi::VehicleSensorControl& sensor_ctl) {
        // light_ctrl_.brake_light = sensor_ctl.brake_light;
        // light_ctrl_.double_flash_light = sensor_ctl.double_flash_light;
        // light_ctrl_.low_beam = sensor_ctl.low_beam;
        // light_ctrl_.high_beam = sensor_ctl.high_beam;
        // light_ctrl_.turn_left_light = sensor_ctl.turn_left_light;
        // light_ctrl_.turn_right_light = sensor_ctl.turn_right_light;
        // light_ctrl_.auto_horn = sensor_ctl.auto_horn;
        // light_ctrl_.front_fog_light = sensor_ctl.front_fog_light;
        // light_ctrl_.rear_fog_light = sensor_ctl.rear_fog_light;
        // light_ctrl_.tail_light = sensor_ctl.tail_light;
        // light_ctrl_.wiper = sensor_ctl.wiper;
        // light_ctrl_.door_lock = sensor_ctl.door_lock;
    }

    void GetLonCtrl(CPU_30_LonCtrl& lon_ctrl) {
        std::unique_lock<std::mutex> locker(mutex_);
        lon_ctrl = lon_ctrl_;
    }

    void GetLatCtrl(CPU_31_LatCtrl& lat_ctrl) {
        std::unique_lock<std::mutex> locker(mutex_);
        lat_ctrl = lat_ctrl_;
    }
    void GetLightCtrl(CPU_41_LightCtrl& light_ctrl) {
        std::unique_lock<std::mutex> locker(mutex_);
        light_ctrl = light_ctrl_;
    }

private:
    std::mutex       mutex_;
    CPU_30_LonCtrl   lon_ctrl_;
    CPU_31_LatCtrl   lat_ctrl_;
    CPU_41_LightCtrl light_ctrl_;

public:
    static AionCanbusDataInteract* Instance() {            // 静态公有方法，用于获取唯一实例
        static AionCanbusDataInteract* instance = nullptr; // 静态成员变量，用于存储唯一实例
        if (instance == nullptr) {
            instance = new AionCanbusDataInteract();
        }
        return instance;
    }
};

class AionCanbusSendData {
public:
    AionCanbusSendData() : _enabled(true) {
    }
    void Init() {
        std::vector<struct can_filter> rfilter;
        struct can_filter              temp_filter;
        temp_filter.can_id = CPU_41;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);

        temp_filter.can_id = CPU_40;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);

        temp_filter.can_id = CPU_31;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);

        temp_filter.can_id = CPU_30;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);

        socket_can_transmitter_ptr = std::make_shared<SocketcanTransmitter>(config, rfilter);

        std::thread([&]() {
            while (1) {
                if (_enabled) {
                    SendLonCtrlData(); // 30
                    usleep(5 * 1000);
                    SendLatCtrlData(); // 31
                    usleep(8 * 1000);
                    SendLightData(); // 41
                    usleep(7 * 1000);
                    SendLonCtrlData(); // 30
                    usleep(5 * 1000);
                    SendLatCtrlData(); // 31
                    usleep(15 * 1000);
                    SendLonCtrlData(); // 30
                    usleep(5 * 1000);
                    SendLatCtrlData(); // 31
                    usleep(15 * 1000);
                    SendLonCtrlData(); // 30
                    usleep(3 * 1000);
                    SendLightData(); // 41
                    usleep(2 * 1000);
                    SendLatCtrlData(); // 31
                    usleep(15 * 1000);
                    SendLonCtrlData(); // 30
                    usleep(5 * 1000);
                    SendLatCtrlData(); // 31
                    usleep(15 * 1000);
                } else {
                    AINFO << "disabled";
                    usleep(20 * 1000);
                }
            }
        }).detach();
    }

    void enable() {
        _enabled = true;
    }

    void disable() {
        _enabled = false;
    }

    ~AionCanbusSendData() {
    }

    void SendLightData() {
        // auto& light_info = AionCanbusDataInteract::Instance()->GetLightCtrlRef();
        CPU_41_LightCtrl light_info;
        AionCanbusDataInteract::Instance()->GetLightCtrl(light_info);
        SocketCanFrame socket_can_frame;
        socket_can_frame.id = CPU_41;
        socket_can_frame.data_len = 0x08;
        if (light_info.ConvertToCanFrame(socket_can_frame.data, socket_can_frame.data_len) == -1) {
            AINFO << "ConvertToCanFrame Failed";
        }
        socket_can_transmitter_ptr->WriteCanFrame(socket_can_frame);
    }

    void SendLonCtrlData() {
        // auto& lon_ctrl_info = AionCanbusDataInteract::Instance()->GetLonCtrlRef();
        CPU_30_LonCtrl lon_ctrl_info;
        AionCanbusDataInteract::Instance()->GetLonCtrl(lon_ctrl_info);
        SocketCanFrame socket_can_frame;
        socket_can_frame.id = CPU_30;
        socket_can_frame.data_len = 0x08;
        if (lon_ctrl_info.ConvertToCanFrame(socket_can_frame.data, socket_can_frame.data_len) == -1) {
            AINFO << "lon_ctrl_info ConvertToCanFrame Failed";
        }
        socket_can_transmitter_ptr->WriteCanFrame(socket_can_frame);
    }

    void SendLatCtrlData() {
        // auto& lat_ctrl_info = AionCanbusDataInteract::Instance()->GetLatCtrlRef();
        CPU_31_LatCtrl lat_ctrl_info;
        AionCanbusDataInteract::Instance()->GetLatCtrl(lat_ctrl_info);
        SocketCanFrame socket_can_frame;
        socket_can_frame.id = CPU_31;
        socket_can_frame.data_len = 0x08;
        if (lat_ctrl_info.ConvertToCanFrame(socket_can_frame.data, socket_can_frame.data_len) == -1) {
            AINFO << "lat_ctrl_info ConvertToCanFrame Failed";
        }
        socket_can_transmitter_ptr->WriteCanFrame(socket_can_frame);
    }

public:
    std::atomic_bool                      _enabled;
    SocketCanConfig                       config;
    std::shared_ptr<SocketcanTransmitter> socket_can_transmitter_ptr;
};

#endif
