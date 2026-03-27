#ifndef __AION_CANRECV_PROTOCOL_H__
#define __AION_CANRECV_PROTOCOL_H__

#include <algorithm>
#include <vector>

#define BVCU3A_VEHICLE_INFO_INVALID_TIME (10 * 0.02)
#define BVCU3A_VEHICLE_INFO_RESUME_TIME (3 * 0.02)

#define BVCU3B_VEHICLE_STATE_INVALID_TIME (10 * 0.05)
#define BVCU3B_VEHICLE_STATE_RESUME_TIME (3 * 0.05)

#define BVCU3C_VEHICLE_STATUS_INVALID_TIME (10 * 0.05)
#define BVCU3C_VEHICLE_STATUS_RESUME_TIME (3 * 0.05)

#define BVCU363_KEY_STATE_INVALID_TIME (10 * 0.05)
#define BVCU363_KEY_STATE_RESUME_TIME (3 * 0.05)

typedef struct BVCU_363_KeyState {
    double rev_time;
    bool   is_valid;
    int    left_change_road_key_state;
    int    right_change_road_key_state;
    int    emergency_brake_key_state;
    int    slow_brake_key_state;
    int    operation_mode_key_state;
    int    presentation_mode_key_state;
    int    drive_key_state;
    BVCU_363_KeyState() {
        rev_time = 0.0;
        is_valid = false;
        left_change_road_key_state = 0;
        right_change_road_key_state = 0;
        emergency_brake_key_state = 0;
        slow_brake_key_state = 0;
        operation_mode_key_state = 0;
        presentation_mode_key_state = 0;
        drive_key_state = 0;
    }

    bool CheckValid(double now_time) {
        if (fabs(rev_time) < 0.1) {
            rev_time = now_time;
            return true;
        }
        if (now_time - rev_time > BVCU363_KEY_STATE_INVALID_TIME) {
            is_valid = false;
            return is_valid;
        }
        if (now_time - rev_time < BVCU363_KEY_STATE_RESUME_TIME) {
            is_valid = true;
            return is_valid;
        }
        return is_valid;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 8)
            return -1;
        left_change_road_key_state = (int)(data[0] & 0x01);
        right_change_road_key_state = (int)((data[0] >> 1) & 0x01);
        emergency_brake_key_state = (int)((data[0] >> 2) & 0x01);
        slow_brake_key_state = (int)((data[0] >> 3) & 0x01);
        operation_mode_key_state = (int)((data[0] >> 4) & 0x01);
        presentation_mode_key_state = (int)((data[0] >> 5) & 0x01);

        if ((operation_mode_key_state == 1) && (presentation_mode_key_state == 1)) {
            presentation_mode_key_state = 0;
        }
        drive_key_state = (int)((data[0] >> 6) & 0x01);
        return 0;
    }
} BVCU_363_KeyState;

typedef struct BVCU_4A_FaultInfo {
    double rev_time;
    bool   is_valid;
    int    EPS_fault;
    int    Brake_fault;
    int    Drive_fault;
    int    EPB_fault;
    int    Power12_Over_fault;
    int    Power12_Low_fault;
    int    PowerIGN_Over_fault;
    int    PowerIGN_Low_fault;
    int    EPS_Com_fault;
    int    BCM_Com_fault;
    int    EPB_Com_fault;
    int    CPU_Com_fault;
    int    VCU_Com_fault;
    BVCU_4A_FaultInfo() {
        rev_time = 0.0;
        is_valid = false;
        EPS_fault = 0;
        Brake_fault = 0;
        Drive_fault = 0;
        EPB_fault = 0;
        Power12_Over_fault = 0;
        Power12_Low_fault = 0;
        PowerIGN_Over_fault = 0;
        PowerIGN_Low_fault = 0;
        EPS_Com_fault = 0;
        BCM_Com_fault = 0;
        EPB_Com_fault = 0;
        CPU_Com_fault = 0;
        VCU_Com_fault = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        rev_time = 0.0;
        if (len != 8)
            return -1;
        EPS_fault = (int)((data[0]) & 0x01);
        Brake_fault = (int)((data[0] >> 1) & 0x01);
        Drive_fault = (int)((data[0] >> 2) & 0x01);
        EPB_fault = (int)((data[0] >> 3) & 0x01);
        Power12_Over_fault = (int)((data[2]) & 0x01);
        Power12_Low_fault = (int)((data[2] >> 1) & 0x01);
        PowerIGN_Over_fault = (int)((data[2] >> 2) & 0x01);
        PowerIGN_Low_fault = (int)((data[2] >> 3) & 0x01);
        EPS_Com_fault = (int)((data[4]) & 0x01);
        BCM_Com_fault = (int)((data[4] >> 4) & 0x01);
        EPB_Com_fault = (int)((data[4] >> 5) & 0x01);
        CPU_Com_fault = (int)((data[4] >> 6) & 0x01);
        VCU_Com_fault = (int)((data[4] >> 7) & 0x01);
        return 0;
    }
} BVCU_4A_FaultInfo;

typedef struct BVCU_3A_VehicleInfo {
    double rev_time;
    bool   is_valid;
    double steerangle;
    double speed;
    int    shift_position;
    int    epb_status;
    int    current_drive_mode;
    int    control_switch_state;
    double vehcle_accel;
    double break_pressure;
    BVCU_3A_VehicleInfo() {
        is_valid = false;
        rev_time = 0.0;
        steerangle = 0.0;
        speed = 0.0;
        shift_position = 0;
        epb_status = 0;
        current_drive_mode = 0;
        control_switch_state = 0;
        vehcle_accel = 0.0;
        break_pressure = 0.0;
    }

    bool CheckValid(double now_time) {
        if (fabs(rev_time) < 0.1) {
            rev_time = now_time;
            return true;
        }
        if (now_time - rev_time > BVCU3A_VEHICLE_INFO_INVALID_TIME) {
            is_valid = false;
            return is_valid;
        }
        if (now_time - rev_time < BVCU3A_VEHICLE_INFO_RESUME_TIME) {
            is_valid = true;
            return is_valid;
        }
        return is_valid;
    }
    int GetFromCanFrame(const char *data, int len) {
        if (len != 8)
            return -1;
        steerangle = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) * 0.1 - 1024;
        speed = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) * 0.1 / 3.6;
        shift_position = (int)((data[4]) & 0x03);
        epb_status = (int)((data[4] >> 2) & 0x03);
        current_drive_mode = (int)((data[4] >> 4) & 0x03);
        if (current_drive_mode == 0) { //自主
            current_drive_mode = 1;
        } else if (current_drive_mode == 2) { // 急停
                                              // keep
        } else {                              //手动
            current_drive_mode = 0;
        }
        control_switch_state = (int)((data[4] >> 6) & 0x03);
        vehcle_accel = (((unsigned short)data[5] << 8) + (unsigned char)data[6]) * 0.1 - 16;
        break_pressure = (double)(data[7]) * 0.1;
        return 0;
    }
} BVCU_3A_VehicleInfo;

typedef struct BVCU_3B_VehicleState {
    double rev_time;
    bool   is_valid;
    int    brake_light_state;
    int    doubleflash_light_state;
    int    lowbeam_state;
    int    highbeam_state;
    int    leftturn_light_state;
    int    rightturn_light_state;
    int    frontfog_light_state;
    int    rearfog_light_state;
    int    tail_light_state;
    int    horn_state;
    int    wiper_state;
    int    cruise_switch_state;
    int    driver_door_state;
    int    passager_door_state;
    int    driver_door_lock_state;
    int    passager_door_lock_state;
    int    driver_seatbeltrst;
    int    passager_seatbeltrst;

    double accpedalposi;
    double bms_battery_soc;
    // double drive_range;
    double fuel_left_over;
    double mileage;

    BVCU_3B_VehicleState() {
        rev_time = 0.0;
        is_valid = false;
        brake_light_state = 0;
        doubleflash_light_state = 0;
        lowbeam_state = 0;
        highbeam_state = 0;
        leftturn_light_state = 0;
        rightturn_light_state = 0;
        frontfog_light_state = 0;
        rearfog_light_state = 0;
        tail_light_state = 0;
        horn_state = 0;
        wiper_state = 0;
        cruise_switch_state = 0;
        driver_door_state = 0;
        passager_door_state = 0;
        driver_door_lock_state = 0;
        passager_door_lock_state = 0;
        driver_seatbeltrst = 0;
        passager_seatbeltrst = 0;
        accpedalposi = 0.0;
        bms_battery_soc = 0.0;
        // drive_range = 0.0;
        fuel_left_over = 0.0;
        mileage = 0.0;
    }

    bool CheckValid(double now_time) {
        if (fabs(rev_time) < 0.1) {
            rev_time = now_time;
            return true;
        }
        if (now_time - rev_time > BVCU3B_VEHICLE_STATE_INVALID_TIME) {
            is_valid = false;
            return is_valid;
        }
        if (now_time - rev_time < BVCU3B_VEHICLE_STATE_RESUME_TIME) {
            is_valid = true;
            return is_valid;
        }
        return is_valid;
    }
    int GetFromCanFrame(const char *data, int len) {
        if (len != 8)
            return -1;
        brake_light_state = (int)((data[0]) & 0x01);
        doubleflash_light_state = (int)((data[0] >> 1) & 0x01);
        lowbeam_state = (int)((data[0] >> 2) & 0x01);
        highbeam_state = (int)((data[0] >> 3) & 0x01);
        leftturn_light_state = (int)((data[0] >> 4) & 0x01);
        rightturn_light_state = (int)((data[0] >> 5) & 0x01);
        frontfog_light_state = (int)((data[0] >> 6) & 0x01);
        rearfog_light_state = (int)((data[0] >> 7) & 0x01);
        tail_light_state = (int)((data[1]) & 0x01);
        horn_state = (int)((data[1] >> 1) & 0x01);
        wiper_state = (int)((data[1] >> 2) & 0x01);
        cruise_switch_state = (int)((data[1] >> 3) & 0x01);
        driver_door_state = (int)((data[1] >> 4) & 0x01);
        passager_door_state = (int)((data[1] >> 5) & 0x01);
        driver_door_lock_state = (int)((data[1] >> 6) & 0x01);
        passager_door_lock_state = (int)((data[1] >> 7) & 0x01);
        driver_seatbeltrst = (int)((data[2]) & 0x01);
        passager_seatbeltrst = (int)((data[2] >> 1) & 0x01);
        accpedalposi = ((unsigned char)(data[3])) * 0.4;
        bms_battery_soc = ((unsigned char)(data[4])) * 0.4;
        mileage = (((unsigned short)data[5] << 8) + (unsigned char)data[6]) * 0.1;
        fuel_left_over = (double)(data[7]);
        return 0;
    }
} BVCU_3B_VehicleState;

typedef struct BVCU_3C_VehicleStatus {
    /* data */
    double rev_time;
    bool   is_valid;
    int    remote_en_state;

    BVCU_3C_VehicleStatus() {
        rev_time = 0.0;
        is_valid = false;
        remote_en_state = 0;
    }

    bool CheckValid(double now_time) {
        if (fabs(rev_time) < 0.1) {
            rev_time = now_time;
            return true;
        }
        if (now_time - rev_time > BVCU3C_VEHICLE_STATUS_INVALID_TIME) {
            is_valid = false;
            return is_valid;
        }
        if (now_time - rev_time < BVCU3C_VEHICLE_STATUS_RESUME_TIME) {
            is_valid = true;
            return is_valid;
        }
        return is_valid;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 8)
            return -1;
        remote_en_state = (int)((data[6] >> 5) & 0x01);
        return 0;
    }
} BVCU_3C_VehicleStatus;

#endif