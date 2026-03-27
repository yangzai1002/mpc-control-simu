#ifndef __G318_CANBUS_SEND_H__
#define __G318_CANBUS_SEND_H__
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <thread>

#include "DatetimeUtil.hpp"
#include "G318_can_send_protocol.h"
#include "SocketCan.hpp"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehiclesensorcontrol.h"
#include "custom_stack.h"
#include "monitor/faulthandle_api.hpp"

class G318CanbusDataInteract {
private:
    G318CanbusDataInteract() {
    }

    ~G318CanbusDataInteract() {
        FreeControl();
    }

public:
    void SetVehicleSensorControl(const ara::adsfi::VehicleSensorControl& sensor_ctl) {
        std::unique_lock<std::mutex> locker(mutex_);
        sensor_ctl_ = sensor_ctl;
        //数据保护，mrct停的时候退出自主

        auto double_time_this = app_common::DatetimeUtil::double_time_from_sec_nsec(sensor_ctl.header.time);
        if (double_time_this == sensor_ctl_old_time && sensor_ctl_old_time != 0) {
            sensor_ctl_old_time_count = sensor_ctl_old_time_count + 1;
        } else {
            sensor_ctl_old_time = double_time_this;
            sensor_ctl_old_time_count = 0;
        }

        if (sensor_ctl_old_time_count > 10) {
            sensor_ctl_.chassis_enable = 0;
        }
    }

    void SetVehicleActControl(const ara::adsfi::VehicleActControl& act_ctl) {
        std::unique_lock<std::mutex> locker(mutex_);
        act_ctl_ = act_ctl;
        // 數據保護，傳入數據的值過於大時，將他們進行限制
        // if (act_ctl_.lon_control.target_accelation >= 0)
        //     act_ctl_.lon_control.target_accelation = 0;
        // if (act_ctl_.lon_control.target_accelation < -10)
        //     act_ctl_.lon_control.target_accelation = -10;
        if (act_ctl_.lon_control.target_torque >= 5000)
            act_ctl_.lon_control.target_torque = 5000;
        if (act_ctl_.lon_control.target_torque < 0)
            act_ctl_.lon_control.target_torque = 0;
        if (act_ctl_.lat_control.target_angle >= 480)
            act_ctl_.lon_control.target_torque = 480;
        if (act_ctl_.lat_control.target_angle <= -480)
            act_ctl_.lon_control.target_torque = -480;

        auto double_time_this = app_common::DatetimeUtil::double_time_from_sec_nsec(act_ctl.header.time);
        if (sensor_ctl_.chassis_enable == 1) {
            //数据保护，任务停止时车辆缓停
            if (double_time_this == act_ctl_old_time && act_ctl_old_time != 0) {
                act_ctl_old_time_count = act_ctl_old_time_count + 1;
            } else {
                act_ctl_old_time = double_time_this;
                act_ctl_old_time_count = 0;
            }

            if (act_ctl_old_time_count > 10) {
                act_ctl_.lon_control.actuator_mode = 0;
                // act_ctl_.lon_control.epb_mode = 1;
                // act_ctl_.lon_control.target_accelation = -7;
                act_ctl_.lon_control.target_torque = 0;
            }
            // AINFO << "act_ctl_.lon_control.epb_mode = " << (int)act_ctl_.lon_control.epb_mode;
            //  横向控制赋值
            ecu_106_ctrl_.ACC_MotorTorqueMaxLimitRequest = 20;
            ecu_106_ctrl_.ACC_MotorTorqueMinLimitRequest = -20;
            ecu_106_ctrl_.ACC_LatAngReq = act_ctl_.lat_control.target_angle;
            ecu_106_ctrl_.ACC_LatAngReqActive = 1;
            AINFO << "转向角度：" << ecu_106_ctrl_.ACC_LatAngReq;

            //  档位赋值
            switch ((int)act_ctl_.lon_control.shift_position) {
                case 0:
                    ecu_102_ctrl_.APA_TransPRNDShiftRequest = 1; // P
                    break;
                case 1:
                    ecu_102_ctrl_.APA_TransPRNDShiftRequest = 2; // R
                    break;
                case 2:
                    ecu_102_ctrl_.APA_TransPRNDShiftRequest = 3; // N
                    break;
                case 3:
                    ecu_102_ctrl_.APA_TransPRNDShiftRequest = 4; // D
                    break;
                case 4:
                    ecu_102_ctrl_.APA_TransPRNDShiftRequest = 0; // T
                    break;
                default:
                    ecu_102_ctrl_.APA_TransPRNDShiftRequest = 1; // P
                    break;
            }

            ecu_102_ctrl_.APA_LSCAction = 1;
            ecu_102_ctrl_.APA_TransPRNDShiftEnable = 1;
            ecu_102_ctrl_.APA_TargetAccelerationValid = 1;
            ecu_102_ctrl_.APA_TransPRNDShiftReqValid = 1;
            ecu_102_ctrl_.APA_EPBrequestValid = 1;

            if (((int)act_ctl_.lon_control.shift_position != last_shift_position) && (last_shift_position == 0)) {
                ecu_102_ctrl_.APA_EPBrequest = 2;
                last_epb_time = double_time_this;
            } else if (((int)act_ctl_.lon_control.shift_position != last_shift_position) &&
                       ((int)act_ctl_.lon_control.shift_position == 0)) {
                ecu_102_ctrl_.APA_EPBrequest = 1;
                last_epb_time = double_time_this;
            }

            if (last_epb_time == 0) {
                ecu_102_ctrl_.APA_LSCAction = 0;
                ecu_102_ctrl_.APA_TargetAccelerationValid = 0;
                ecu_102_ctrl_.APA_EPBrequest = 0;
                ecu_102_ctrl_.APA_EPBrequestValid = 0;
            }
            if ((double_time_this - last_epb_time) * 1e3 > 500) {
                ecu_102_ctrl_.APA_EPBrequest = 0;
                last_epb_time = 0;
            }

            last_shift_position = (int)act_ctl_.lon_control.shift_position;
            AINFO << "档位：" << (int)act_ctl_.lon_control.shift_position;

            // 纵向控制赋值
            ecu_101_ctrl_.ACC_Driveoff_Request = 1;
            if ((int)act_ctl_.lon_control.shift_position == 3 || (int)act_ctl_.lon_control.shift_position == 1) {
                if ((int)act_ctl_.lon_control.actuator_mode == 1) {
                    if (ecu_101_ctrl_.ACC_ACCTargetAcceleration < -5) {
                        ecu_101_ctrl_.ACC_ACCTargetAcceleration = -5;
                    } else {
                        ecu_101_ctrl_.ACC_ACCTargetAcceleration = 0;
                    }
                    ecu_101_ctrl_.ACC_DecToStop = 0;
                    ecu_101_ctrl_.ACC_CDDActive = 0;
                    ecu_101_ctrl_.ACC_AccTrqReq = act_ctl_.lon_control.target_torque;
                    ecu_101_ctrl_.ACC_AccTrqReqActive = 1;
                    ecu_101_ctrl_.ACC_ACCMode = 3;
                }
            }

            // if ((int)act_ctl_.lon_control.epb_mode == 1) {
            //     ecu_101_ctrl_.ACC_ACCTargetAcceleration = act_ctl_.lon_control.target_accelation; //-5~-10
            //     ecu_101_ctrl_.ACC_AccTrqReqActive = 0;
            //     ecu_101_ctrl_.ACC_AccTrqReq = 0;
            //     ecu_101_ctrl_.ACC_DecToStop = 1;
            //     ecu_101_ctrl_.ACC_CDDActive = 1;
            //     if (brake_flag <= 10) {
            //         ecu_101_ctrl_.ACC_ACCMode = 2;
            //         brake_flag += 1;
            //     } else {
            //         ecu_101_ctrl_.ACC_ACCMode = 3;
            //     }
            // } else {
            //     brake_flag = 0;
            // }

        } else {
            FreeControl();
        }
    }

    void FreeControl() {
        // 退出自驾
        ecu_101_ctrl_.ACC_ACCTargetAcceleration = -5;
        ecu_101_ctrl_.ACC_CDDActive = 0;
        ecu_101_ctrl_.ACC_ACCMode = 0;
        ecu_106_ctrl_.ACC_LatAngReqActive = 0;
        ecu_101_ctrl_.ACC_AccTrqReqActive = 0;
        ecu_102_ctrl_.APA_TargetAccelerationValid = 0;
        ecu_102_ctrl_.APA_TransPRNDShiftReqValid = 0;
        ecu_102_ctrl_.APA_EPBrequestValid = 0;
        ecu_102_ctrl_.APA_LSCAction = 0;
        ecu_102_ctrl_.APA_TransPRNDShiftEnable = 0;
        ecu_101_ctrl_.ACC_Driveoff_Request = 0;
        // 值置0
        ecu_101_ctrl_.ACC_AccTrqReq = 0;
        ecu_106_ctrl_.ACC_LatAngReq = 0;
        ecu_102_ctrl_.APA_TransPRNDShiftRequest = 0;
        // 档位置为当前档位
        last_shift_position = 0;
        act_ctl_old_time = 0;
        brake_flag = 0;
        last_epb_time = 0;
        act_ctl_old_time_count = 0;
        AINFO << "退出自主";
    }

    void GetECU106Ctrl(ECU_106_Ctrl& ecu_106_ctrl) {
        std::unique_lock<std::mutex> locker(mutex_);
        ecu_106_ctrl = ecu_106_ctrl_;
    }

    void GetECU105Ctrl(ECU_105_Ctrl& ecu_105_ctrl) {
        std::unique_lock<std::mutex> locker(mutex_);
        ecu_105_ctrl = ecu_105_ctrl_;
    }

    void GetECU102Ctrl(ECU_102_Ctrl& ecu_102_ctrl) {
        std::unique_lock<std::mutex> locker(mutex_);
        ecu_102_ctrl = ecu_102_ctrl_;
    }

    void GetECU101Ctrl(ECU_101_Ctrl& ecu_101_ctrl) {
        std::unique_lock<std::mutex> locker(mutex_);
        ecu_101_ctrl = ecu_101_ctrl_;
    }

private:
    std::mutex                       mutex_;
    ECU_106_Ctrl                     ecu_106_ctrl_;
    ECU_105_Ctrl                     ecu_105_ctrl_;
    ECU_102_Ctrl                     ecu_102_ctrl_;
    ECU_101_Ctrl                     ecu_101_ctrl_;
    int                              last_shift_position = 0;
    double                           last_epb_time = 0;
    int                              brake_flag = 0;
    ara::adsfi::VehicleActControl    act_ctl_;
    ara::adsfi::VehicleSensorControl sensor_ctl_;
    double                           act_ctl_old_time = 0;
    double                           act_ctl_old_time_count = 0;
    double                           sensor_ctl_old_time = 0;
    double                           sensor_ctl_old_time_count = 0;

public:
    static G318CanbusDataInteract* Instance() { // 静态公有方法，用于获取唯一实例
        static G318CanbusDataInteract instance;
        return &instance;
    }
};

class G318CanbusSendData {
public:
    G318CanbusSendData() : _enabled(true) {
    }
    void Init() {
        std::vector<struct can_filter> rfilter;
        struct can_filter              temp_filter;

        temp_filter.can_id = 0x106;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);

        temp_filter.can_id = 0x105;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);

        temp_filter.can_id = 0x102;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);

        temp_filter.can_id = 0x101;
        temp_filter.can_mask = CAN_SFF_MASK;
        rfilter.push_back(temp_filter);

        bool was_connected = true;
        bool was_read_ok = true;

        socket_can_transmitter_ptr = std::make_shared<SocketcanFdTransmitter>(config, rfilter);

        if (!socket_can_transmitter_ptr->IsConnected()) {
            if (was_connected) {
                AINFO << "sock_can init error";
                // FaultHandle::FaultApi::Instance()->SetFaultCode("1061171");
                was_connected = false;
            }
        } else {
            if (!was_connected) {
                AINFO << "sock_can recover ok";
                // FaultHandle::FaultApi::Instance()->ResetFaultCode("1061171");
                was_connected = true;
            }
        }

        std::thread([&]() {
            while (true) {
                if (_enabled) {
                    // SendECU105CtrlData(); // 10
                    // AINFO << "0x105发送数据";
                    SendECU106CtrlData(); // 10
                    // AINFO << "0x106发送数据";
                    usleep(10 * 1000);
                    SendECU106CtrlData();
                    // AINFO << "0x106发送数据";
                    SendECU102CtrlData();
                    // AINFO << "0x102发送数据";
                    SendECU101CtrlData();
                    // AINFO << "0x101发送数据";
                } else {
                    AINFO << "diabled send data";
                }
                usleep(10 * 1000);
            }
        }).detach();
    }

    void disable() {
        _enabled = false;
    }

    void enable() {
        _enabled = true;
    }

    ~G318CanbusSendData() {
    }

    void SendECU106CtrlData() {
        // auto& lon_ctrl_info = AionCanbusDataInteract::Instance()->GetLonCtrlRef();
        ECU_106_Ctrl ecu_106_ctrl_info;
        G318CanbusDataInteract::Instance()->GetECU106Ctrl(ecu_106_ctrl_info);
        SocketCanFdFrame socket_can_frame;
        socket_can_frame.id = 0x106;
        socket_can_frame.data_len = 0x40;
        socket_can_frame.fd_flag = 1;
        socket_can_frame.brs_flag = 1;
        if (ecu_106_ctrl_info.ConvertToCanFrame(socket_can_frame.data, socket_can_frame.data_len) == -1) {
            AINFO << "ecu_106_ctrl_info ConvertToCanFrame Failed";
        }
        // AINFO << "数据是：" << (int)socket_can_frame.data[0] << (int)socket_can_frame.data[1];
        socket_can_transmitter_ptr->WriteCanFrame(socket_can_frame);
    }

    void SendECU105CtrlData() {
        // auto& lon_ctrl_info = AionCanbusDataInteract::Instance()->GetLonCtrlRef();
        ECU_105_Ctrl ecu_105_ctrl_info;
        G318CanbusDataInteract::Instance()->GetECU105Ctrl(ecu_105_ctrl_info);
        SocketCanFdFrame socket_can_frame;
        socket_can_frame.id = 0x105;
        socket_can_frame.data_len = 0x40;
        socket_can_frame.fd_flag = 1;
        socket_can_frame.brs_flag = 1;
        if (ecu_105_ctrl_info.ConvertToCanFrame(socket_can_frame.data, socket_can_frame.data_len) == -1) {
            AINFO << "ecu_105_ctrl_info ConvertToCanFrame Failed";
        }
        // AINFO << "数据是：" << (int)socket_can_frame.data[0] << (int)socket_can_frame.data[1];
        socket_can_transmitter_ptr->WriteCanFrame(socket_can_frame);
    }

    void SendECU102CtrlData() {
        // auto& lon_ctrl_info = AionCanbusDataInteract::Instance()->GetLonCtrlRef();
        ECU_102_Ctrl ecu_102_ctrl_info;
        G318CanbusDataInteract::Instance()->GetECU102Ctrl(ecu_102_ctrl_info);
        SocketCanFdFrame socket_can_frame;
        socket_can_frame.id = 0x102;
        socket_can_frame.data_len = 0x40;
        socket_can_frame.fd_flag = 1;
        socket_can_frame.brs_flag = 1;
        if (ecu_102_ctrl_info.ConvertToCanFrame(socket_can_frame.data, socket_can_frame.data_len) == -1) {
            AINFO << "ecu_102_ctrl_info ConvertToCanFrame Failed";
        }
        // AINFO << "数据是：" << (int)socket_can_frame.data[32];
        // AINFO << "是否是CANFD" << socket_can_frame.fd_flag;
        socket_can_transmitter_ptr->WriteCanFrame(socket_can_frame);
    }

    void SendECU101CtrlData() {
        // auto& lon_ctrl_info = AionCanbusDataInteract::Instance()->GetLonCtrlRef();
        ECU_101_Ctrl ecu_101_ctrl_info;
        G318CanbusDataInteract::Instance()->GetECU101Ctrl(ecu_101_ctrl_info);
        SocketCanFdFrame socket_can_frame;
        socket_can_frame.id = 0x101;
        socket_can_frame.data_len = 0x40;
        socket_can_frame.fd_flag = 1;
        socket_can_frame.brs_flag = 1;
        if (ecu_101_ctrl_info.ConvertToCanFrame(socket_can_frame.data, socket_can_frame.data_len) == -1) {
            AINFO << "ecu_101_ctrl_info ConvertToCanFrame Failed";
        }
        socket_can_transmitter_ptr->WriteCanFrame(socket_can_frame);
    }

public:
    std::atomic<bool>                       _enabled;
    SocketCanFdConfig                       config;
    std::shared_ptr<SocketcanFdTransmitter> socket_can_transmitter_ptr;
};

#endif
