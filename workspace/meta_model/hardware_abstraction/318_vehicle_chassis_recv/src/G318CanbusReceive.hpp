#ifndef __G318_CANBUS_RECEIVE_HPP__
#define __G318_CANBUS_RECEIVE_HPP__
#include <chrono>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "G318_can_recv_protocol.h"
#include "SocketCan.hpp"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "config_loader/custom_stack.h"
#include "log/log.h"
#include "monitor/faulthandle_api.hpp"
#include "ymdhms2GpsTs.hpp"

class G318CanbusRecvDataInteract {
private:
    G318CanbusRecvDataInteract() {
    }

public:
    // string last_fault_code = "5015";
    void GetVehicleInformation(ara::adsfi::VehicleInformation& vehicle_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        if (EPS_LatCtrlActive || TCU_APAShiftGearRequestEnable == 1) {
            current_vehicle_info.vehicle_act_state.drive_mode = 1;
        }
        if (exit_auto_con1 || exit_auto_con2) {
            current_vehicle_info.vehicle_act_state.drive_mode = 0;
            exit_auto_con1 = false;
            exit_auto_con2 = false;
        }
        AINFO << (int)current_vehicle_info.vehicle_act_state.drive_mode;
        vehicle_info = current_vehicle_info;
        // wheel_speed = current_wheel_speed;
    }

    void SetGW219StatusInfo(const GW_219_Status& gw_219_status_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        if ((int)gw_219_status_info.PCU_BrkPedlSts1 == 0 || (int)gw_219_status_info.PCU_BrkPedlSts2 == 1) {
            exit_auto_con1 = true;
        }
        AINFO << "制动踏板状态1：" << (int)gw_219_status_info.PCU_BrkPedlSts1;
        AINFO << "制动踏板状态2：" << (int)gw_219_status_info.PCU_BrkPedlSts2;
    }

    void SetGW220StatusInfo(const GW_220_Status& gw_220_status_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        if (abs((double)gw_220_status_info.EPS_MeasuredTorsionBarTorque) >= 10) {
            exit_auto_con2 = true;
        }
        EPS_LatCtrlActive = (int)gw_220_status_info.EPS_LatCtrlActive;
        AINFO << "EPS横向控制激活：" << (int)gw_220_status_info.EPS_LatCtrlActive;
        // AINFO << "驾驶员手力矩验证位：" << (int)gw_220_status_info.EPS_MeasuredTorsionBarTorqValid;
    }

    void SetGW222StatusInfo(const GW_222_Status& gw_222_status_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        TCU_APAShiftGearRequestEnable = (int)gw_222_status_info.TCU_APAShiftGearRequestEnable;
        AINFO << "APA换挡控制使能：" << (int)gw_222_status_info.TCU_APAShiftGearRequestEnable;
    }

    void SetGW226StatusInfo(const GW_226_Status& gw_226_status_info) {
        std::unique_lock<std::mutex> locker(mutex_);

        AINFO << "APA控制中断反馈：" << (int)gw_226_status_info.EPS_APA_Abortfeedback;
        AINFO << "APA请求转向控制反馈：" << (int)gw_226_status_info.EPS_APA_ControlFeedback;
        AINFO << "EPS临时故障：" << (int)gw_226_status_info.EPS_APA_EpasFAILED;
    }

    void SetGW217StatusInfo(const GW_217_Status& gw_217_status_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        current_vehicle_info.vehicle_act_state.speed = (double)gw_217_status_info.ESP_VehicleSpeed; //车速

        AINFO << "车速信号：" << (double)gw_217_status_info.ESP_VehicleSpeed;
    }

    void SetGW216StatusInfo(const GW_216_Status& gw_216_status_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        current_vehicle_info.vehicle_act_state.acceleration = (double)gw_216_status_info.ESP_LongAccel; // 纵向加速度

        AINFO << "横向加速度：" << (double)gw_216_status_info.ESP_LatAccel;
        AINFO << "纵向加速度：" << (double)gw_216_status_info.ESP_LongAccel;
    }

    void SetGW204StatusInfo(const GW_204_Status& gw_204_status_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        current_vehicle_info.vehicle_act_state.steer_angle = (double)gw_204_status_info.SAS_SteeringAngle; // 转向角度

        AINFO << "转向角度：" << (double)gw_204_status_info.SAS_SteeringAngle;
        AINFO << "转角速度：" << (double)gw_204_status_info.SAS_SteeringAngleSpeed;
    }

    void SetGW218StatusInfo(const GW_218_Status& gw_218_status_info) {
        std::unique_lock<std::mutex> locker(mutex_);

        AINFO << "发动机转速：" << (double)gw_218_status_info.EMS_EngSpd;
    }

    void SetGW224StatusInfo(const GW_224_Status& gw_224_status_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        // current_vehicle_info.wheel_speed.left_front_speed = (double)gw_224_status_info.Wheel_Speed_FL_Data; //
        // 左前轮速 current_vehicle_info.wheel_speed.right_front_speed = (double)gw_224_status_info.Wheel_Speed_FR_Data;
        // // 右前轮速 current_vehicle_info.wheel_speed.left_rear_speed =
        // (double)gw_224_status_info.Wheel_Speed_RL_Data; // 左后轮速 current_vehicle_info.wheel_speed.right_rear_speed
        // = (double)gw_224_status_info.Wheel_Speed_RR_Data; // 右后轮速

        AINFO << "左前轮速信号：" << (double)gw_224_status_info.Wheel_Speed_FL_Data;
        AINFO << "右前轮速信号：" << (double)gw_224_status_info.Wheel_Speed_FR_Data;
        AINFO << "左后轮速信号：" << (double)gw_224_status_info.Wheel_Speed_RL_Data;
        AINFO << "右后轮速信号：" << (double)gw_224_status_info.Wheel_Speed_RR_Data;

        GPSTime        gps_ts = {0, 0.0};
        TimeWithMillis cur_ms = {};
        cur_ms = getCurrentTimeWithMillis();
        gps_ts = utcToGps(cur_ms);
        //     current_wheel_speed.week = gps_ts.week;
        //     current_wheel_speed.utc_time = gps_ts.tow;
        //     current_wheel_speed.left_front_speed = (double)gw_224_status_info.Wheel_Speed_FL_Data;  // 左前轮速
        //     current_wheel_speed.right_front_speed = (double)gw_224_status_info.Wheel_Speed_FR_Data; // 右前轮速
        //     current_wheel_speed.left_rear_speed = (double)gw_224_status_info.Wheel_Speed_RL_Data;   // 左后轮速
        //     current_wheel_speed.right_rear_speed = (double)gw_224_status_info.Wheel_Speed_RR_Data;  // 右后轮速
    }

    void SetGW223StatusInfo(const GW_223_Status& gw_223_status_info) {
        std::unique_lock<std::mutex> locker(mutex_);
        if ((int)gw_223_status_info.TCU_ActualDrivingGear == 0) {
            current_vehicle_info.vehicle_act_state.shift_position = 2; //档位 0: P 1:R 2:N 3:D 4:T
            AINFO << "实际档位："
                  << "N";
        } else if ((int)gw_223_status_info.TCU_ActualDrivingGear == 9) {
            current_vehicle_info.vehicle_act_state.shift_position = 1; //档位 0: P 1:R 2:N 3:D 4:T
            AINFO << "实际档位："
                  << "R";
        } else if ((int)gw_223_status_info.TCU_ActualDrivingGear == 10) {
            current_vehicle_info.vehicle_act_state.shift_position = 0; //档位 0: P 1:R 2:N 3:D 4:T
            AINFO << "实际档位："
                  << "P";
        } else if ((int)gw_223_status_info.TCU_ActualDrivingGear >= 1 &&
                   (int)gw_223_status_info.TCU_ActualDrivingGear <= 8) {
            current_vehicle_info.vehicle_act_state.shift_position = 3; //档位 0: P 1:R 2:N 3:D 4:T
            AINFO << "实际档位："
                  << "D";
        }
    }

private:
    std::mutex                     mutex_;
    ara::adsfi::VehicleInformation current_vehicle_info;
    // ara::adsfi::SensorWheelSpeed   current_wheel_speed;
    int  TCU_APAShiftGearRequestEnable = 0;
    bool exit_auto_con1 = false;
    bool exit_auto_con2 = false;
    bool EPS_LatCtrlActive = false;

public:
    static G318CanbusRecvDataInteract* Instance() { // 静态公有方法，用于获取唯一实例
        static G318CanbusRecvDataInteract instance;
        return &instance;
    }
};

class G318CanbusRecvData {
public:
    G318CanbusRecvData() {
    }

    ~G318CanbusRecvData() {
    }
    void Init() {
        std::vector<struct can_filter> rfilter;
        struct can_filter              temp_filter;

        temp_filter.can_id = 0x219;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = 0x220;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = 0x222;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = 0x226;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = 0x217;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = 0x216;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = 0x204;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = 0x218;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = 0x224;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);
        temp_filter.can_id = 0x223;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);

        socket_can_transmitter_ptr = std::make_shared<SocketcanFdTransmitter>(config, rfilter);

        std::thread([&]() {
            SocketCanFdFrame can_frame_rev;
            can_frame_rev.fd_flag = 1;

            GW_219_Status gw_219_status_info;
            GW_220_Status gw_220_status_info;
            GW_222_Status gw_222_status_info;
            GW_226_Status gw_226_status_info;
            GW_217_Status gw_217_status_info;
            GW_216_Status gw_216_status_info;
            GW_204_Status gw_204_status_info;
            GW_218_Status gw_218_status_info;
            GW_224_Status gw_224_status_info;
            GW_223_Status gw_223_status_info;

            bool was_connected = true;
            bool was_read_ok = true;

            while (true) {
                if (!socket_can_transmitter_ptr->IsConnected()) {
                    if (was_connected) {
                        AINFO << "sock_can init error";
                        // FaultHandle::FaultApi::Instance()->SetFaultCode("1061171");
                        was_connected = false;
                    }
                    continue;
                } else {
                    if (!was_connected) {
                        AINFO << "sock_can recover ok";
                        // FaultHandle::FaultApi::Instance()->ResetFaultCode("1061171");
                        was_connected = true;
                    }
                }

                if (socket_can_transmitter_ptr->ReadCanFrame(can_frame_rev) == -1) {
                    if (was_read_ok) {
                        AINFO << "ReadCanFrame error";
                        // FaultHandle::FaultApi::Instance()->SetFaultCode("1061173");
                        was_read_ok = false;
                    }
                    bool reinit_success = socket_can_transmitter_ptr->ReInit(rfilter);
                    if (reinit_success) {
                        AINFO << "CAN 重连成功";
                    } else {
                        AINFO << "CAN 重连失败";
                    }
                    continue;
                } else {
                    if (!was_read_ok) {
                        AINFO << "ReadCanFrame ok";
                        // FaultHandle::FaultApi::Instance()->ResetFaultCode("1061173");
                        was_read_ok = true;
                    }
                }

                switch (can_frame_rev.id) {
                    case (0x219): {
                        if (gw_219_status_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "gw_219_status_info GetFromCanFrame error";
                        }
                        gw_219_status_info.rev_time = Tools::Second();
                        G318CanbusRecvDataInteract::Instance()->SetGW219StatusInfo(gw_219_status_info);
                    } break;
                    case (0x220): {
                        if (gw_220_status_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "gw_220_status_info GetFromCanFrame error";
                        }
                        gw_220_status_info.rev_time = Tools::Second();
                        G318CanbusRecvDataInteract::Instance()->SetGW220StatusInfo(gw_220_status_info);
                    } break;
                    case (0x222): {
                        if (gw_222_status_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "gw_222_status_info GetFromCanFrame error";
                        }
                        gw_222_status_info.rev_time = Tools::Second();
                        G318CanbusRecvDataInteract::Instance()->SetGW222StatusInfo(gw_222_status_info);
                    } break;
                    case (0x226): {
                        if (gw_226_status_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "gw_226_status_info GetFromCanFrame error";
                        }
                        gw_226_status_info.rev_time = Tools::Second();
                        G318CanbusRecvDataInteract::Instance()->SetGW226StatusInfo(gw_226_status_info);
                    } break;
                    case (0x217): {
                        if (gw_217_status_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "gw_217_status_info GetFromCanFrame error";
                        }
                        gw_217_status_info.rev_time = Tools::Second();
                        G318CanbusRecvDataInteract::Instance()->SetGW217StatusInfo(gw_217_status_info);
                    } break;
                    case (0x216): {
                        if (gw_216_status_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "gw_216_status_info GetFromCanFrame error";
                        }
                        gw_216_status_info.rev_time = Tools::Second();
                        G318CanbusRecvDataInteract::Instance()->SetGW216StatusInfo(gw_216_status_info);
                    } break;
                    case (0x204): {
                        if (gw_204_status_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "gw_204_status_info GetFromCanFrame error";
                        }
                        gw_204_status_info.rev_time = Tools::Second();
                        G318CanbusRecvDataInteract::Instance()->SetGW204StatusInfo(gw_204_status_info);
                    } break;
                    case (0x218): {
                        if (gw_218_status_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "gw_218_status_info GetFromCanFrame error";
                        }
                        gw_218_status_info.rev_time = Tools::Second();
                        G318CanbusRecvDataInteract::Instance()->SetGW218StatusInfo(gw_218_status_info);
                    } break;
                    case (0x224): {
                        if (gw_224_status_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "gw_224_status_info GetFromCanFrame error";
                        }
                        gw_224_status_info.rev_time = Tools::Second();
                        G318CanbusRecvDataInteract::Instance()->SetGW224StatusInfo(gw_224_status_info);
                    } break;
                    case (0x223): {
                        if (gw_223_status_info.GetFromCanFrame(can_frame_rev.data, can_frame_rev.data_len) == -1) {
                            AINFO << "gw_223_status_info GetFromCanFrame error";
                        }
                        gw_223_status_info.rev_time = Tools::Second();
                        G318CanbusRecvDataInteract::Instance()->SetGW223StatusInfo(gw_223_status_info);
                    } break;

                    default: {
                        AINFO << "receive other can id " << can_frame_rev.id ;
                    }
                }
            }
        }).detach();
    }

public:
    SocketCanFdConfig                       config;
    std::shared_ptr<SocketcanFdTransmitter> socket_can_transmitter_ptr;
};

#endif
