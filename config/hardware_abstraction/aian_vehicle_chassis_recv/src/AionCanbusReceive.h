#ifndef __AIAN_CANBUS_RECIVE_H__
#define __AIAN_CANBUS_RECIVE_H__

#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "SocketCan.hpp"
#include "aion_can_recv_protocol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "log/log.h"
#include "tools.h"

#define BVCU_3C 0x3C // TBD
#define BVCU_3B 0x3B // TBD
#define BVCU_3A 0x3A // TBD
#define CPU_40 0x40  // TBD
#define BVCU_4A 0x4A // TBD

class CanbusRecvDataInteract {
private:
    CanbusRecvDataInteract() {
    }

public:
    void SetKeyState(const BVCU_363_KeyState& key_state) {
        std::unique_lock<std::mutex> locker(mutex_);
        // bvcu_key_state_ = key_state;
    }

    void SetVehicleInfo(const BVCU_3A_VehicleInfo& vehicle_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        current_vehicle_info.vehicle_act_state.steer_angle = vehicle_info.steerangle;
        current_vehicle_info.vehicle_act_state.speed = vehicle_info.speed;
        current_vehicle_info.vehicle_act_state.shift_position = vehicle_info.shift_position;
        current_vehicle_info.vehicle_act_state.epb_status = vehicle_info.epb_status;
        current_vehicle_info.vehicle_act_state.drive_mode = vehicle_info.current_drive_mode;
        // current_vehicle_info.vehicle_act_state.accpedal_position = vehicle_info.vehcle_accel;
        // current_vehicle_info.vehicle_act_state.brkpedal_position = vehicle_info.break_pressure;
    }

    void SetVehicleState(const BVCU_3B_VehicleState& vehicle_state) {
        std::unique_lock<std::mutex> locker(mutex_);
        // current_vehicle_info.vehicle_sensor_state.brake_light_state = vehicle_state.brake_light_state;
        // current_vehicle_info.vehicle_sensor_state.doubleflash_light_state = vehicle_state.doubleflash_light_state;
        // current_vehicle_info.vehicle_sensor_state.lowbeam_state = vehicle_state.lowbeam_state;
        // current_vehicle_info.vehicle_sensor_state.highbeam_state = vehicle_state.highbeam_state;
        // current_vehicle_info.vehicle_sensor_state.leftturn_light_state = vehicle_state.leftturn_light_state;
        // current_vehicle_info.vehicle_sensor_state.rightturn_light_state = vehicle_state.rightturn_light_state;
        // current_vehicle_info.vehicle_sensor_state.frontfog_light_state = vehicle_state.frontfog_light_state;
        // current_vehicle_info.vehicle_sensor_state.rearfog_light_state = vehicle_state.rearfog_light_state;
        // current_vehicle_info.vehicle_sensor_state.tail_light_state = vehicle_state.tail_light_state;
        // current_vehicle_info.vehicle_sensor_state.horn_state = vehicle_state.horn_state;
        // current_vehicle_info.vehicle_sensor_state.wiper_state = vehicle_state.wiper_state;
        // current_vehicle_info.vehicle_sensor_state.cruise_switch_state = vehicle_state.cruise_switch_state;
        // current_vehicle_info.vehicle_sensor_state.driver_door_lock_state = vehicle_state.driver_door_state;
        // current_vehicle_info.vehicle_sensor_state.passager_door_lock_state = vehicle_state.passager_door_state;
        // current_vehicle_info.vehicle_sensor_state.driver_door_lock_state = vehicle_state.driver_door_lock_state;
        // current_vehicle_info.vehicle_sensor_state.passager_door_lock_state = vehicle_state.passager_door_lock_state;
        // current_vehicle_info.vehicle_sensor_state.driver_seatbeltrst = vehicle_state.driver_seatbeltrst;
        current_vehicle_info.vehicle_sensor_state.bms_battery_soc = vehicle_state.bms_battery_soc;
        // current_vehicle_info.vehicle_sensor_state.fuel_left_over = vehicle_state.fuel_left_over;
        current_vehicle_info.vehicle_sensor_state.vehicle_total_mile = vehicle_state.mileage;
    }

    void SetVehicleStatus(const BVCU_3C_VehicleStatus& vehicle_status) {
        std::unique_lock<std::mutex> locker(mutex_);
        // current_vehicle_info.vehicle_sensor_state.remote_en_state = vehicle_status.remote_en_state;
    }

    void GetVehicleInformation(ara::adsfi::VehicleInformation& vehicle_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        vehicle_info = current_vehicle_info;
    }

private:
    std::mutex                     mutex_;
    ara::adsfi::VehicleInformation current_vehicle_info;

public:
    static CanbusRecvDataInteract* Instance() {            // 静态公有方法，用于获取唯一实例
        static CanbusRecvDataInteract* instance = nullptr; // 静态成员变量，用于存储唯一实例
        if (instance == nullptr) {
            instance = new CanbusRecvDataInteract();
        }
        return instance;
    }
};

class AionCanbusRecvData {
public:
    AionCanbusRecvData() {
    }

    ~AionCanbusRecvData() {
    }

    void Init() {
        std::vector<struct can_filter> rfilter;
        struct can_filter              temp_filter;

        temp_filter.can_id = BVCU_3C;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = BVCU_3B;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = BVCU_3A;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = CPU_40;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = BVCU_4A;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);

        socket_can_transmitter_ptr = std::make_shared<SocketcanTransmitter>(config, rfilter);

        std::thread([&]() {
            SocketCanFrame can_frame_rev;

            BVCU_363_KeyState     bvcu_key_state;
            BVCU_4A_FaultInfo     bvcu_fault_info;
            BVCU_3A_VehicleInfo   bvcu_vehicle_info;
            BVCU_3B_VehicleState  bvcu_vehicle_state;
            BVCU_3C_VehicleStatus bvcu_vehicle_status;

            // FaultHandle::FaultApi::Instance()->SetFaultCode("5015");

            while (true) {
                // TIME_MONITOR_USE(cabus_main);
                if (socket_can_transmitter_ptr->ReadCanFrame(can_frame_rev) == -1) {
                    AINFO << "ReadCanFrame error";
                    continue;
                }
                // FaultHandle::FaultApi::Instance()->ResetFaultCode("5015");
                // if(can_frame_rev.id>=0x611&& can_frame_rev.id<=0x617){
                //     AINFO<<"can "<<std::hex<<can_frame_rev.id<<" len:"<<can_frame_rev.data_len;
                //     for(int i=0; i<8; ++i){
                //         AINFO<<" "<<(int)can_frame_rev.data[i];
                //     }
                //     AINFO;
                // }
                switch (can_frame_rev.id) {
                    case (BVCU_3A): {
                        if (bvcu_vehicle_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "bvcu_vehicle_info GetFromCanFrame error";
                        }
                        bvcu_vehicle_info.rev_time = Tools::Second();
                        CanbusRecvDataInteract::Instance()->SetVehicleInfo(bvcu_vehicle_info);
                    } break;
                    case (BVCU_3B): {
                        if (bvcu_vehicle_state.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "bvcu_vehicle_state GetFromCanFrame error";
                        }
                        bvcu_vehicle_state.rev_time = Tools::Second();
                        CanbusRecvDataInteract::Instance()->SetVehicleState(bvcu_vehicle_state);
                    } break;
                    case (BVCU_3C): {
                        if (bvcu_vehicle_status.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "bvcu_key_state GetFromCanFrame error";
                        }
                        bvcu_vehicle_status.rev_time = Tools::Second();
                        CanbusRecvDataInteract::Instance()->SetVehicleStatus(bvcu_vehicle_status);
                    } break;
                    case (BVCU_4A): {
                        // if (bvcu_fault_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                        // 	AINFO << "bvcu_fault_info GetFromCanFrame error";
                        // }
                        // bvcu_fault_info.rev_time = Tools::Second();
                        // CanbusRecvDataInteract::Instance()->SetFaultInfo(bvcu_fault_info);
                    } break;

                    default: {
                        AINFO << "receive other can id " << can_frame_rev.id;
                    }
                }
            }
        }).detach();
    }

public:
    SocketCanConfig                       config;
    std::shared_ptr<SocketcanTransmitter> socket_can_transmitter_ptr;
};

// void AionCanbusReceive ( VehicleInformation& vehicle_info ) {
// 	STATIC_DATA(bool, is_init, false);
// 	STATIC_DATA(std::shared_ptr<AionCanbusRecvData>, driver_ptr, nullptr);

// 	if (is_init == false) {

// 		auto ptr = CustomStack::Instance();
// 		is_init = true;
// 		driver_ptr = std::make_shared<AionCanbusRecvData>();

// 		ptr->GetConfig("can_device", driver_ptr->config.can_name);
// 		driver_ptr->Init();
// 	}

// 	CanbusRecvDataInteract::Instance()->GetVehicleInformation(vehicle_info);
// }

#endif
