#ifndef __AION_CANSEND_PROTOCOL_H__
#define __AION_CANSEND_PROTOCOL_H__

struct CPU_30_LonCtrl {
    int target_torque;
    int target_pressure;
    int actuator_mode;
    int shift_position;
    int automode_en;
    int enlon;
    CPU_30_LonCtrl() {
        target_torque = 0;
        target_pressure = 0;
        actuator_mode = 0;
        shift_position = 0;
        automode_en = 0;
        enlon = 1;
    }
    int ConvertToCanFrame(char *data, int max_len) {
        if (max_len < 8)
            return -1;
        data[0] = (unsigned short)target_torque / 256;
        data[1] = (unsigned short)target_torque % 256;
        data[2] = (unsigned short)target_pressure / 256;
        data[3] = (unsigned short)target_pressure % 256;
        data[4] =
            ((unsigned short)actuator_mode) | ((unsigned short)enlon << 2) | ((unsigned short)shift_position << 6);
        data[5] = (unsigned short)target_pressure & 0x01;
        data[6] = 0;
        data[7] = data[0] ^ data[1] ^ data[2] ^ data[3] ^ data[4] ^ data[5] ^ data[6];
        return 8;
    }
};

struct CPU_31_LatCtrl {
    int target_angle;
    int target_torque;
    int Auto_Readysts;
    int enlat;
    CPU_31_LatCtrl() {
        target_angle = 0;
        target_torque = 0;
        Auto_Readysts = 1;
        enlat = 1;
    }
    int ConvertToCanFrame(char *data, int max_len) {
        if (max_len < 8)
            return -1;
        data[0] = (unsigned short)((target_angle + 1024) / 0.1) / 256;
        data[1] = (unsigned short)((target_angle + 1024) / 0.1) % 256;
        data[2] = (unsigned short)target_torque / 256;
        data[3] = (unsigned short)target_torque % 256;
        data[4] = ((unsigned short)enlat) & 0x03 | (unsigned char)((Auto_Readysts & 0x03) << 2);
        data[5] = 0;
        data[6] = 0;
        data[7] = data[0] ^ data[1] ^ data[2] ^ data[3] ^ data[4] ^ data[5] ^ data[6];
        return 8;
    }
};

struct CPU_41_LightCtrl {
    int brake_light;
    int double_flash_light;
    int low_beam;
    int high_beam;
    int turn_left_light;
    int turn_right_light;
    int auto_horn;
    int front_fog_light;
    int rear_fog_light;
    int tail_light;
    int wiper;
    int door_lock;
    CPU_41_LightCtrl() {
        brake_light = 0;
        double_flash_light = 0;
        low_beam = 0;
        high_beam = 0;
        turn_left_light = 0;
        turn_right_light = 0;
        auto_horn = 0;
        front_fog_light = 0;
        rear_fog_light = 0;
        tail_light = 0;
        wiper = 0;
        door_lock = 0;
    }
    int ConvertToCanFrame(char *data, int max_len) {
        if (max_len < 8)
            return -1;
        data[0] = (brake_light)&0x01;
        data[0] |= (double_flash_light & 0x01) << 1;
        data[0] |= (low_beam & 0x01) << 2;
        data[0] |= (high_beam & 0x01) << 3;
        data[0] |= (turn_left_light & 0x01) << 4;
        data[0] |= (turn_right_light & 0x01) << 5;
        data[0] |= (auto_horn & 0x01) << 6;
        data[0] |= (front_fog_light & 0x01) << 7;
        data[1] = (rear_fog_light & 0x01);
        data[1] |= (tail_light & 0x01) << 1;
        data[1] |= (wiper & 0x01) << 2;
        data[1] |= (door_lock & 0x01) << 3;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = data[0] ^ data[1] ^ data[2] ^ data[3] ^ data[4] ^ data[5] ^ data[6];
        return 8;
    }
};

#endif
