#ifndef __G318_CANRECV_PROTOCOL_H__
#define __G318_CANRECV_PROTOCOL_H__

#include <algorithm>
#include <vector>

struct GW_204_Status {
    double rev_time;
    double SAS_SteeringAngle;      // 转向角度
    double SAS_SteeringAngleSpeed; //转角速度

    GW_204_Status() {
        rev_time = 0;
        SAS_SteeringAngle = 0;
        SAS_SteeringAngleSpeed = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 64)
            return -1;
        if ((double)((data[9] & 0xFF) | ((data[8] & 0xFF) << 8)) > 32767) {
            SAS_SteeringAngle = ((double)((data[9] & 0xFF) | ((data[8] & 0xFF) << 8)) - 65536) * 0.1;
        } else {
            SAS_SteeringAngle = (double)((data[9] & 0xFF) | ((data[8] & 0xFF) << 8)) * 0.1;
        }
        SAS_SteeringAngleSpeed = (double)(data[10] & 0xFF) * 4;
        return 0;
    }
};

struct GW_216_Status {
    double rev_time;
    double ESP_LatAccel;  // 横向加速度
    double ESP_LongAccel; //  纵向加速度

    GW_216_Status() {
        rev_time = 0;
        ESP_LatAccel = 0;
        ESP_LongAccel = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 64)
            return -1;
        ESP_LatAccel = (double)(data[10] & 0xFF) * 0.1 - 12.7;
        ESP_LongAccel = (double)(((data[12] >> 6) & 0x03) | ((data[11] & 0xFF) << 2)) * 0.031 - 16;
        return 0;
    }
};

struct GW_217_Status {
    double rev_time;
    double ESP_VehicleSpeed; // 车速信号

    GW_217_Status() {
        rev_time = 0;
        ESP_VehicleSpeed = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 64)
            return -1;
        ESP_VehicleSpeed = (double)((data[13] & 0xFF) | ((data[12] & 0x1F) << 8)) * 0.05625 / 3.6;
        return 0;
    }
};

struct GW_218_Status {
    double rev_time;
    double EMS_EngSpd; // 发动机转速

    GW_218_Status() {
        rev_time = 0;
        EMS_EngSpd = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 64)
            return -1;
        EMS_EngSpd = (double)(((data[26] >> 7) & 0x01) | ((data[25] & 0xFF) << 1) | ((data[24] & 0x7F) << 9)) * 0.25;
        return 0;
    }
};

struct GW_219_Status {
    double       rev_time;
    unsigned int PCU_BrkPedlSts1; // 制动踏板状态1
    unsigned int PCU_BrkPedlSts2; // 制动踏板状态2

    GW_219_Status() {
        rev_time = 0;
        PCU_BrkPedlSts1 = 0;
        PCU_BrkPedlSts2 = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 64)
            return -1;
        PCU_BrkPedlSts1 = (unsigned int)((data[35] >> 2) & 0x01);
        PCU_BrkPedlSts2 = (unsigned int)((data[37] >> 2) & 0x01);
        return 0;
    }
};

struct GW_220_Status {
    double       rev_time;
    double       EPS_MeasuredTorsionBarTorque;    // 驾驶员手力矩
    unsigned int EPS_LatCtrlAvailabilityStatus;   // EPS横向控制可用状态
    unsigned int EPS_LatCtrlActive;               // EPS横向控制激活
    unsigned int EPS_MeasuredTorsionBarTorqValid; // 驾驶员手力矩验证位

    GW_220_Status() {
        rev_time = 0;
        EPS_LatCtrlAvailabilityStatus = 0;
        EPS_LatCtrlActive = 0;
        EPS_MeasuredTorsionBarTorque = 0;
        EPS_MeasuredTorsionBarTorqValid = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 64)
            return -1;
        EPS_MeasuredTorsionBarTorque = (double)(((data[8] & 0xFF) << 4) | ((data[9] & 0xF0) >> 4)) * 0.01 - 20.48;
        EPS_MeasuredTorsionBarTorqValid = (unsigned int)(data[13] & 0x01);
        EPS_LatCtrlAvailabilityStatus = (unsigned int)((data[14] >> 4) & 0x03);
        EPS_LatCtrlActive = (unsigned int)((data[14] >> 6) & 0x01);
        return 0;
    }
};

struct GW_222_Status {
    double       rev_time;
    unsigned int TCU_APAShiftGearRequestEnable; // APA换挡控制使能

    GW_222_Status() {
        rev_time = 0;
        TCU_APAShiftGearRequestEnable = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 64)
            return -1;
        TCU_APAShiftGearRequestEnable = (unsigned int)((data[8] >> 1) & 0x03);
        return 0;
    }
};

struct GW_223_Status {
    double       rev_time;
    unsigned int TCU_ActualDrivingGear; // 实际档位

    GW_223_Status() {
        rev_time = 0;
        TCU_ActualDrivingGear = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 64)
            return -1;

        TCU_ActualDrivingGear = (unsigned int)(data[9] & 0x0F);
        return 0;
    }
};

struct GW_224_Status {
    double rev_time;
    double Wheel_Speed_FL_Data; // 左前轮速信号
    double Wheel_Speed_FR_Data; // 右前轮速信号
    double Wheel_Speed_RL_Data; // 左后轮速信号
    double Wheel_Speed_RR_Data; // 右后轮速信号

    GW_224_Status() {
        rev_time = 0;
        Wheel_Speed_FL_Data = 0;
        Wheel_Speed_FR_Data = 0;
        Wheel_Speed_RL_Data = 0;
        Wheel_Speed_RR_Data = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 64)
            return -1;

        Wheel_Speed_FL_Data = (double)((data[59] & 0xFF) | ((data[58] & 0x1F) << 8)) * 0.056 / 3.6;
        Wheel_Speed_FR_Data = (double)((data[57] & 0xFF) | ((data[56] & 0x1F) << 8)) * 0.056 / 3.6;
        Wheel_Speed_RL_Data = (double)((data[51] & 0xFF) | ((data[50] & 0x1F) << 8)) * 0.056 / 3.6;
        Wheel_Speed_RR_Data = (double)((data[49] & 0xFF) | ((data[48] & 0x1F) << 8)) * 0.056 / 3.6;
        return 0;
    }
};

struct GW_226_Status {
    double       rev_time;
    unsigned int EPS_APA_ControlFeedback; // APA请求转向控制反馈
    unsigned int EPS_APA_EpasFAILED;      // EPS临时故障
    unsigned int EPS_APA_Abortfeedback;   // APA控制中断反馈

    GW_226_Status() {
        rev_time = 0;
        EPS_APA_ControlFeedback = 0;
        EPS_APA_EpasFAILED = 0;
        EPS_APA_Abortfeedback = 0;
    }

    int GetFromCanFrame(const char *data, int len) {
        if (len != 64)
            return -1;
        EPS_APA_ControlFeedback = (unsigned int)((data[11] >> 5) & 0x01);
        EPS_APA_EpasFAILED = (unsigned int)((data[9] >> 1) & 0x03);
        EPS_APA_Abortfeedback = (unsigned int)((data[9] >> 4) & 0x07);
        return 0;
    }
};

#endif
