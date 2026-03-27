#ifndef __G318_CANSEND_PROTOCOL_H__
#define __G318_CANSEND_PROTOCOL_H__
#include "log/log.h"

struct ECU_106_Ctrl {
    double ACC_MotorTorqueMaxLimitRequest; // EPS电机扭矩最大限制值
    double ACC_MotorTorqueMinLimitRequest; // EPS电机扭矩最小限制值
    double ACC_LatAngReq;                  // 转向角请求
    int    ACC_LatAngReqActive;            // 转向角请求激活
    int    ACC_RollingCounter_1BA_0;       // ACC_1BA_0计数器
    int    ACC_CRCCheck_1BA_0;             // ACC_1BA_0校验和
    int    ADS_ESSActive;                  // 紧急转向辅助激活
    int    ACC_RollingCounter_1BA_1;       // ACC_1BA_1计数器
    int    ACC_CRCCheck_1BA_1;             // ACC_1BA_1校验和
    int    ACC_ADCReqType;                 // ADC请求类型
    int    ACC_LDWinhaitwarning;           // 车道偏离预警惩罚模式告警
    int    ACC_LKAinhaitWarning;           // LKA惩罚模式告警
    int    AD_EESStatus;                   // EES系统状态
    int    AD_ESSWarning;                  // ESS触发报警
    int    AD_ESSObjType;                  // ESS触发目标类型
    int    AD_EESActive;                   // EES触发信号
    int    ADS_MesssageCounter_1BA;        // 1BA_报文计数器
    int    ADS_ResetFlag_1BA;              // 1BA_复位标志
    double ADS_Authernticator_1BA;         // 1BA_授权值
    int    ACC_LAEBActive;                 // LAEB激活状态

    ECU_106_Ctrl() {
        ACC_MotorTorqueMaxLimitRequest = 0; // EPS电机扭矩最大限制值
        ACC_MotorTorqueMinLimitRequest = 0; // EPS电机扭矩最小限制值
        ACC_LatAngReq = 0;                  // 转向角请求
        ACC_LatAngReqActive = 0;            // 转向角请求激活
        ACC_RollingCounter_1BA_0 = 0;       // ACC_1BA_0计数器
        ACC_CRCCheck_1BA_0 = 0;             // ACC_1BA_0校验和
        ADS_ESSActive = 0;                  // 紧急转向辅助激活
        ACC_RollingCounter_1BA_1 = 0;       // ACC_1BA_1计数器
        ACC_CRCCheck_1BA_1 = 0;             // ACC_1BA_1校验和
        ACC_ADCReqType = 0;                 // ADC请求类型
        ACC_LDWinhaitwarning = 0;           // 车道偏离预警惩罚模式告警
        ACC_LKAinhaitWarning = 0;           // LKA惩罚模式告警
        AD_EESStatus = 0;                   // EES系统状态
        AD_ESSWarning = 0;                  // ESS触发报警
        AD_ESSObjType = 0;                  // ESS触发目标类型
        AD_EESActive = 0;                   // EES触发信号
        ADS_MesssageCounter_1BA = 0;        // 1BA_报文计数器
        ADS_ResetFlag_1BA = 0;              // 1BA_复位标志
        ADS_Authernticator_1BA = 0;         // 1BA_授权值
        ACC_LAEBActive = 0;                 // LAEB激活状态
    }

    int ConvertToCanFrame(char *data, int max_len) {
        if (max_len < 64) {
            AERROR << "invalid buffer size";
            return -1;
        }
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        data[8] = 0;
        data[9] = 0;
        data[10] = 0;
        data[11] = 0;
        data[12] = 0;
        data[13] = 0;
        data[14] = 0;
        data[15] = 0;
        data[16] = 0;
        data[17] = 0;
        data[18] = 0;
        data[19] = 0;
        data[20] = 0;
        data[21] = 0;
        data[22] = 0;
        data[23] = 0;
        data[24] = 0;
        data[25] = 0;
        data[26] = 0;
        data[27] = 0;
        data[28] = 0;
        data[29] = 0;
        data[30] = 0;
        data[31] = 0;
        data[32] = (((std::uint64_t)((ACC_MotorTorqueMaxLimitRequest + 20.48) * 50) >> 3) & 0xFF);
        data[33] = (((std::uint64_t)((ACC_MotorTorqueMaxLimitRequest + 20.48) * 50) & 0x07) << 5) |
                   (((std::uint64_t)((ACC_MotorTorqueMinLimitRequest + 20.48) * 50) >> 6) & 0x1F);
        data[34] = ((std::uint64_t)((ACC_MotorTorqueMinLimitRequest + 20.48) * 50) & 0x3F) |
                   (((std::uint64_t)((ACC_LatAngReq + 720) * 10) >> 12) & 0x03);
        data[35] = (((std::uint64_t)((ACC_LatAngReq + 720) * 10) >> 4) & 0xFF);
        data[36] = (((std::uint64_t)((ACC_LatAngReq + 720) * 10) & 0x0F) << 4) | ((ACC_LatAngReqActive & 0x01) << 3) |
                   ((ADS_ESSActive & 0x01) << 2);
        data[37] = 0;
        data[38] = (ACC_RollingCounter_1BA_0 & 0x0F) | ((ACC_LAEBActive & 0x01) << 4);
        data[39] = (ACC_CRCCheck_1BA_0 & 0x0FF);
        data[40] = 0;
        data[41] = 0;
        data[42] = ((ACC_ADCReqType & 0x03) << 3);
        data[43] = 0;
        data[44] = (ACC_LDWinhaitwarning & 0x03);
        data[45] = ((ACC_LKAinhaitWarning & 0x03) << 6);
        data[46] = (ACC_RollingCounter_1BA_1 & 0x0F);
        data[47] = (ACC_CRCCheck_1BA_1 & 0xFF);
        data[48] = ((AD_EESStatus & 0x07) << 5) | ((AD_ESSWarning & 0x07) << 2) | (AD_ESSObjType & 0x03);
        data[49] = ((AD_EESActive & 0x07) << 5);
        data[50] = 0;
        data[51] = 0;
        data[52] = 0;
        data[53] = 0;
        data[54] = 0;
        data[55] = 0;
        data[56] = 0;
        data[57] = (((std::uint64_t)ADS_Authernticator_1BA >> 48) & 0xFF);
        data[58] = (((std::uint64_t)ADS_Authernticator_1BA >> 40) & 0xFF);
        data[59] = (((std::uint64_t)ADS_Authernticator_1BA >> 32) & 0xFF);
        data[60] = (((std::uint64_t)ADS_Authernticator_1BA >> 24) & 0xFF);
        data[61] = (((std::uint64_t)ADS_Authernticator_1BA >> 16) & 0xFF);
        data[62] = (((std::uint64_t)ADS_Authernticator_1BA >> 8) & 0xFF);
        data[63] = ((std::uint64_t)ADS_Authernticator_1BA & 0xFF);
        return 64;
    }
};

struct ECU_105_Ctrl {
    int APA_HiddenHandleControlReq; // 隐藏门把手控制请求
    int APA_DoorLockRq;             // 车门锁控制
    int APA_MirrorAutoFoldReq;      // 后视镜自动折叠请求
    int APA_DriverWindowControl;    // 主驾车窗控制
    int APA_PassengerWindowControl; // 副驾车窗控制
    int APA_LeftRearWindowControl;  // 左后车窗控制
    int APA_RightRearWindowControl; // 右后车窗控制
    int APA_CRCCheck_550;           // APA_550较验和
    int APA_RollingCounter_550;     // APA_550计数器

    ECU_105_Ctrl() {
        APA_HiddenHandleControlReq = 0;  // 隐藏门把手控制请求
        APA_DoorLockRq = 0;              // 车门锁控制
        APA_MirrorAutoFoldReq = 0;       // 后视镜自动折叠请求
        APA_DriverWindowControl = 0;     // 主驾车窗控制
        APA_PassengerWindowControl = 50; // 副驾车窗控制
        APA_LeftRearWindowControl = 0;   // 左后车窗控制
        APA_RightRearWindowControl = 0;  // 右后车窗控制
        APA_CRCCheck_550 = 0;            // APA_550较验和
        APA_RollingCounter_550 = 0;      // APA_550计数器
    }

    int ConvertToCanFrame(char *data, int max_len) {
        if (max_len < 64) {
            AERROR << "invalid buffer size";
            return -1;
        }
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        data[8] = (APA_HiddenHandleControlReq & 0x03) | ((APA_DoorLockRq & 0x03) << 2) |
                  ((APA_MirrorAutoFoldReq & 0x03) << 6);
        data[9] = 0;
        data[10] = (((APA_DriverWindowControl + 1) & 0x7F) << 1) | (((APA_PassengerWindowControl + 1) >> 6) & 0x01);
        data[11] = (((APA_PassengerWindowControl + 1) & 0x3F) << 2) | (((APA_LeftRearWindowControl + 1) >> 5) & 0x03);
        data[12] = (((APA_LeftRearWindowControl + 1) & 0x1F) << 3) | (((APA_RightRearWindowControl + 1) >> 4) & 0x07);
        data[13] = (((APA_RightRearWindowControl + 1) & 0x0F) << 4);
        data[14] = APA_RollingCounter_550 & 0x0F;
        data[15] = APA_CRCCheck_550 & 0xFF;
        data[16] = 0;
        data[17] = 0;
        data[18] = 0;
        data[19] = 0;
        data[20] = 0;
        data[21] = 0;
        data[22] = 0;
        data[23] = 0;
        data[24] = 0;
        data[25] = 0;
        data[26] = 0;
        data[27] = 0;
        data[28] = 0;
        data[29] = 0;
        data[30] = 0;
        data[31] = 0;
        data[32] = 0;
        data[33] = 0;
        data[34] = 0;
        data[35] = 0;
        data[36] = 0;
        data[37] = 0;
        data[38] = 0;
        data[39] = 0;
        data[40] = 0;
        data[41] = 0;
        data[42] = 0;
        data[43] = 0;
        data[44] = 0;
        data[45] = 0;
        data[46] = 0;
        data[47] = 0;
        data[48] = 0;
        data[49] = 0;
        data[50] = 0;
        data[51] = 0;
        data[52] = 0;
        data[53] = 0;
        data[54] = 0;
        data[55] = 0;
        data[56] = 0;
        data[57] = 0;
        data[58] = 0;
        data[59] = 0;
        data[60] = 0;
        data[61] = 0;
        data[62] = 0;
        data[63] = 0;
        return 64;
    }
};

struct ECU_102_Ctrl {
    int    APA_RollingCounter_247;         // APA_247计数器
    int    APA_CRCCheck_247;               // APA_247校验和
    int    APA_PtTorqReq;                  // APA扭矩请求
    int    APA_EPBrequest;                 // 电子手刹请求
    int    APA_EPBrequestValid;            // 电子手刹请求有效
    int    APA_TargetAccelerationValid;    // APA目标加速度有效信号
    int    APA_TransPRNDShiftRequest;      // 目标档位请求
    int    APA_TransPRNDShiftReqValid;     // 目标档位请求有效
    double APA_TargetAcceleration;         // APA目标加速度
    int    APA_TransPRNDShiftEnable;       // 换档控制使能
    int    APA_LSCAction;                  // LSC功能开启状态
    int    APA_Activation_Status;          // APA激活状态
    int    APA_DynamicSlotWarning;         // APA动态车位提示
    int    APA_SlotNotice;                 // 自动泊车车位提示信号
    int    APA_AccPedShieldReq;            // 加速踏板屏蔽请求
    int    APA_RollingCounter_26C;         // APA_26C计数器
    int    APA_CRCCheck_26C;               // APA_26C校验和
    int    APA_HSAHDforbidden;             // AUtoHold功能、坡起辅助功能禁止请求
    int    APA_SteeringAngleReqProtection; // APA转向控制请求
    int    APA_ErrorStatus;                // 自动泊车故障信号
    int    APA_EmergenceBrake;             // 紧急制动请求
    double APA_SteeringAngleReq;           // APA转向角度请求
    int    APA_RemoteOnOff;                // 遥控钥匙方案 APA 发出信号给PEPS
    int    APA_IncreasePressureReq;        // APA请求主动建压请求
    int    APA_TurnLightsCommand;          // 转向灯控制命令
    int    APA_RollingCounter_264;         // APA_264计数器
    int    APA_ParkingPercentage;          // 泊车进度条显示
    int    APA_CRCCheck_264;               // APA_264校验和
    int    APA_APAOnOff;                   // APA功能开启状态
    int    APA_FunctionOnOffSts;           // APA功能开启状态
    int    APA_ESP_BrakeFunctionMode;      // APA功能控制状态
    int    APA_LatLongSC;                  // APA横纵向控制中信号
    int    APA_PtTrqReqValid;              // 扭矩请求有效标志位
    int    APA_CRC_Checksum_236;           // APA_校验和_236
    int    APA_RollingCounter_236;         // APA_计数器_236
    int    APA_ESP_StandstillRequest;      // APA静态保压请求
    int    APA_ESPDistToStop;              // APA目标停车距离
    int    APA_VCUReadyReq;                // APA请求可行驶

    ECU_102_Ctrl() {
        APA_RollingCounter_247 = 0;         // APA_247计数器
        APA_CRCCheck_247 = 0;               // APA_247校验和
        APA_PtTorqReq = 0;                  // APA扭矩请求
        APA_EPBrequest = 0;                 // 电子手刹请求
        APA_EPBrequestValid = 0;            // 电子手刹请求有效
        APA_TargetAccelerationValid = 0;    // APA目标加速度有效信号
        APA_TransPRNDShiftRequest = 0;      // 目标档位请求
        APA_TransPRNDShiftReqValid = 0;     // 目标档位请求有效
        APA_TargetAcceleration = 0;         // APA目标加速度
        APA_TransPRNDShiftEnable = 0;       // 换档控制使能
        APA_LSCAction = 0;                  // LSC功能开启状态
        APA_Activation_Status = 0;          // APA激活状态
        APA_DynamicSlotWarning = 0;         // APA动态车位提示
        APA_SlotNotice = 0;                 // 自动泊车车位提示信号
        APA_AccPedShieldReq = 0;            // 加速踏板屏蔽请求
        APA_RollingCounter_26C = 0;         // APA_26C计数器
        APA_CRCCheck_26C = 0;               // APA_26C校验和
        APA_HSAHDforbidden = 0;             // AUtoHold功能、坡起辅助功能禁止请求
        APA_SteeringAngleReqProtection = 0; // APA转向控制请求
        APA_ErrorStatus = 0;                // 自动泊车故障信号
        APA_EmergenceBrake = 0;             // 紧急制动请求
        APA_SteeringAngleReq = 0;           // APA转向角度请求
        APA_RemoteOnOff = 0;                // 遥控钥匙方案 APA 发出信号给PEPS
        APA_IncreasePressureReq = 0;        // APA请求主动建压请求
        APA_TurnLightsCommand = 0;          // 转向灯控制命令
        APA_RollingCounter_264 = 0;         // APA_264计数器
        APA_ParkingPercentage = 0;          // 泊车进度条显示
        APA_CRCCheck_264 = 0;               // APA_264校验和
        APA_APAOnOff = 0;                   // APA功能开启状态
        APA_FunctionOnOffSts = 0;           // APA功能开启状态
        APA_ESP_BrakeFunctionMode = 0;      // APA功能控制状态
        APA_LatLongSC = 0;                  // APA横纵向控制中信号
        APA_PtTrqReqValid = 0;              // 扭矩请求有效标志位
        APA_CRC_Checksum_236 = 0;           // APA_校验和_236
        APA_RollingCounter_236 = 0;         // APA_计数器_236
        APA_ESP_StandstillRequest = 0;      // APA静态保压请求
        APA_ESPDistToStop = 0;              // APA目标停车距离
        APA_VCUReadyReq = 0;                // APA请求可行驶
    }

    int ConvertToCanFrame(char *data, int max_len) {
        if (max_len < 64) {
            AERROR << "invalid buffer size";
            return -1;
        }
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        data[8] = 0;
        data[9] = 0;
        data[10] = 0;
        data[11] = 0;
        data[12] = 0;
        data[13] = 0;
        data[14] = 0;
        data[15] = 0;
        data[16] = 0;
        data[17] = 0;
        data[18] = 0;
        data[19] = 0;
        data[20] = 0;
        data[21] = 0;
        data[22] = 0;
        data[23] = 0;
        data[24] = 0;
        data[25] = 0;
        data[26] = 0;
        data[27] = 0;
        data[28] = 0;
        data[29] = 0;
        data[30] = 0;
        data[31] = 0;
        data[32] = (APA_EPBrequest & 0x03) | ((APA_EPBrequestValid & 0x01) << 2) |
                   ((APA_TargetAccelerationValid & 0x01) << 3) | ((APA_TransPRNDShiftRequest & 0x07) << 4) |
                   ((APA_TransPRNDShiftReqValid & 0x01) << 7);
        data[33] = (std::uint64_t)((APA_TargetAcceleration + 5) * 20) & 0xFF;
        data[34] = 0;
        data[35] = ((APA_Activation_Status >> 1) & 0x03) | ((APA_TransPRNDShiftEnable & 0x01) << 2) |
                   ((APA_LSCAction & 0x01) << 3) | ((APA_HSAHDforbidden & 0x01) << 4);
        data[36] = ((APA_Activation_Status & 0x01) << 7) | ((APA_AccPedShieldReq & 0x01) << 1);
        data[37] = ((APA_SlotNotice & 0x0F) << 2) | ((APA_DynamicSlotWarning & 0x01) << 1);
        data[38] = (APA_RollingCounter_26C & 0x0F);
        data[39] = (APA_CRCCheck_26C & 0x0FF);
        data[40] = (APA_SteeringAngleReqProtection & 0x03) | ((APA_ErrorStatus & 0x01) << 2) |
                   ((APA_APAOnOff & 0x01) << 6) | ((APA_EmergenceBrake & 0x01) << 7);
        if (APA_SteeringAngleReq < 0) {
            APA_SteeringAngleReq = APA_SteeringAngleReq * 10 + 65536;
        } else {
            APA_SteeringAngleReq = APA_SteeringAngleReq * 10;
        }
        data[41] = ((std::uint64_t)(APA_SteeringAngleReq) >> 8) & 0xFF;
        data[42] = (std::uint64_t)(APA_SteeringAngleReq)&0xFF;
        data[43] = ((APA_IncreasePressureReq & 0x01) << 3) | ((APA_TurnLightsCommand & 0x03) << 4) |
                   ((APA_LatLongSC & 0x03) << 6) | (APA_RemoteOnOff & 0x03);
        data[44] = ((APA_FunctionOnOffSts >> 3) & 0x03);
        data[45] = ((APA_FunctionOnOffSts & 0x07) << 5) | ((APA_VCUReadyReq & 0x01) << 3) |
                   ((APA_ParkingPercentage >> 4) & 0x07);
        data[46] = ((APA_ParkingPercentage & 0x0F) << 4) | (APA_RollingCounter_264 & 0x0F);
        data[47] = (APA_CRCCheck_264 & 0xFF);
        data[48] = (((APA_PtTorqReq + 32768) >> 8) & 0xFF);
        data[49] = ((APA_PtTorqReq + 32768) & 0xFF);
        data[50] = ((APA_PtTrqReqValid & 0x01) << 7) | ((APA_ESP_StandstillRequest & 0x01) << 6) |
                   ((APA_ESPDistToStop >> 6) & 0x3F);
        data[51] = ((APA_ESPDistToStop & 0x3F) << 2);
        data[52] = 0;
        data[53] = 0;
        data[54] = (APA_RollingCounter_236 & 0x0F);
        data[55] = (APA_CRC_Checksum_236 & 0xFF);
        data[56] = 0;
        data[57] = 0;
        data[58] = 0;
        data[59] = 0;
        data[60] = 0;
        data[61] = (APA_RollingCounter_247 & 0x0F) | ((APA_ESP_BrakeFunctionMode & 0x07) << 4);
        data[62] = ((APA_CRCCheck_247 >> 8) & 0xFF);
        data[63] = (APA_CRCCheck_247 & 0xFF);
        return 64;
    }
};

struct ECU_101_Ctrl {
    double ACC_ACCTargetAcceleration;  // ACC目标加速度值
    int    ACC_LDWStatus;              // LDW状态
    int    ACC_LDWVibrationWarningReq; // 振动报警请求
    int    ACC_LKAStatus;              // LKA状态
    int    ACC_DecToStop;              // ACC停车请求
    int    ACC_EngineStartStopRequest; // ACC怠速起停请求
    int    ACC_CDDActive;              // ACC减速激活
    int    ACC_ACCMode;                // ACC工作模式
    int    ACC_Driveoff_Request;       // ACC起步请求
    int    ACC_RollingCounter_24E;     // ACC_24E计数器
    int    ACC_CRCCheck_24E;           // ACC_24E校验和
    int    ACC_EmergencyDataRrdReq;    // ACC紧急数据记录请求
    double ACC_AEBTargetDeceleration;  // AEB目标减速度
    int    ACC_AEBActive;              // AEB减速激活
    int    ACC_ABAlevel;               // ABA等级
    int    ACC_ABAActive;              // ABA激活
    int    ACC_AWBlevel;               // AWB等级
    int    ACC_AWBActive;              // AWB激活
    int    ACC_PrefillActive;          // AEB预填充请求
    int    ACC_FCWActive;              // 前碰撞预警激活
    int    ACC_FCWLatentWarning;       // 安全距离提醒报警
    int    ACC_FCWPreWarning;          // 前碰撞预警提醒
    int    ACC_AEBVehilceHoldReq;      // AEB停车保压请求
    int    ACC_AccTrqReqActive;        // ACC扭矩请求激活
    int    ACC_AccTrqReq;              // ACC扭矩请求值
    int    ACC_AEBCtrlType;            // AEB请求类型
    int    ACC_RollingCounter_25E;     // ACC_25E计数器
    int    ACC_CRCCheck_25E;           // ACC_25E校验和
    int    ADS_CRCCheck_244;           // ADS_244校验和
    int    ADS_RollingCounter_244;     // ADS_244计数器
    int    ACC_TextInfoForDriver;      // ACC文字提示
    int    ACC_LDWShakeLevStatus;      // LDW震动强度状态
    int    ACC_LngTakeOverReq;         // 纵向接管请求
    int    ACC_LatTakeoverReq;         // 横向接管请求
    int    ACC_HandsOnReq;             // handsOn请求
    int    ADS_MesssageCounter_244;    // 244_报文计数器
    double ADS_Authernticator_244;     // 244_授权值
    int    ACC_ADCNotice;              // 智能驾驶信息
    int    ACC_SafetyNotice;           // 安全提示信息
    int    ADC_WorkMode;               // 智驾工作模式
    int    ACC_TakeOverReq;            // 通用接管请求
    int    ADS_ResetFlag_244;          // 244_复位标志
    int    ACC_AEBSpeedDropLevel;      // AEB速度降请求

    ECU_101_Ctrl() {
        ACC_ACCTargetAcceleration = 0;  // ACC目标加速度值***
        ACC_LDWStatus = 0;              // LDW状态
        ACC_LDWVibrationWarningReq = 0; // 振动报警请求
        ACC_LKAStatus = 0;              // LKA状态
        ACC_DecToStop = 0;              // ACC停车请求
        ACC_EngineStartStopRequest = 0; // ACC怠速起停请求
        ACC_CDDActive = 0;              // ACC减速激活
        ACC_ACCMode = 0;                // ACC工作模式
        ACC_Driveoff_Request = 0;       // ACC起步请求
        ACC_RollingCounter_24E = 0;     // ACC_24E计数器
        ACC_CRCCheck_24E = 0;           // ACC_24E校验和
        ACC_EmergencyDataRrdReq = 0;    // ACC紧急数据记录请求
        ACC_AEBTargetDeceleration = 0;  // AEB目标减速度***
        ACC_AEBActive = 0;              // AEB减速激活
        ACC_ABAlevel = 0;               // ABA等级
        ACC_ABAActive = 0;              // ABA激活
        ACC_AWBlevel = 0;               // AWB等级
        ACC_AWBActive = 0;              // AWB激活
        ACC_PrefillActive = 0;          // AEB预填充请求
        ACC_FCWActive = 0;              // 前碰撞预警激活
        ACC_FCWLatentWarning = 0;       // 安全距离提醒报警
        ACC_FCWPreWarning = 0;          // 前碰撞预警提醒
        ACC_AEBVehilceHoldReq = 0;      // AEB停车保压请求
        ACC_AccTrqReqActive = 0;        // ACC扭矩请求激活
        ACC_AccTrqReq = 0;              // ACC扭矩请求值***
        ACC_AEBCtrlType = 0;            // AEB请求类型
        ACC_RollingCounter_25E = 0;     // ACC_25E计数器
        ACC_CRCCheck_25E = 0;           // ACC_25E校验和
        ADS_CRCCheck_244 = 0;           // ADS_244校验和
        ADS_RollingCounter_244 = 0;     // ADS_244计数器
        ACC_TextInfoForDriver = 0;      // ACC文字提示
        ACC_LDWShakeLevStatus = 0;      // LDW震动强度状态
        ACC_LngTakeOverReq = 0;         // 纵向接管请求
        ACC_LatTakeoverReq = 0;         // 横向接管请求
        ACC_HandsOnReq = 0;             // handsOn请求
        ADS_MesssageCounter_244 = 0;    // 244_报文计数器
        ADS_Authernticator_244 = 0;     // 244_授权值
        ACC_ADCNotice = 0;              // 智能驾驶信息
        ACC_SafetyNotice = 0;           // 安全提示信息
        ADC_WorkMode = 0;               // 智驾工作模式
        ACC_TakeOverReq = 0;            // 通用接管请求
        ADS_ResetFlag_244 = 0;          // 244_复位标志
        ACC_AEBSpeedDropLevel = 0;      // AEB速度降请求
    }

    int ConvertToCanFrame(char *data, int max_len) {
        if (max_len < 64) {
            AERROR << "invalid buffer size";
            return -1;
        }
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        data[8] = (ADS_ResetFlag_244 & 0x03) | ((ADS_MesssageCounter_244 & 0x3F) << 2);
        data[9] = (((std::uint64_t)ADS_Authernticator_244 & 0xFFFFFFFFFFFF) >> 48) & 0xFF;
        data[10] = (((std::uint64_t)ADS_Authernticator_244 & 0xFFFFFFFFFF) >> 40) & 0xFF;
        data[11] = (((std::uint64_t)ADS_Authernticator_244 & 0xFFFFFFFF) >> 32) & 0xFF;
        data[12] = (((std::uint64_t)ADS_Authernticator_244 & 0xFFFFFF) >> 24) & 0xFF;
        data[13] = (((std::uint64_t)ADS_Authernticator_244 & 0xFFFF) >> 16) & 0xFF;
        data[14] = (((std::uint64_t)ADS_Authernticator_244 & 0xFF) >> 8) & 0xFF;
        data[15] = ((std::uint64_t)ADS_Authernticator_244 & 0xFF);
        data[16] = 0;
        data[17] = 0;
        data[18] = 0;
        data[19] = 0;
        data[20] = 0;
        data[21] = (ADS_RollingCounter_244 & 0x0F);
        data[22] = ((ADS_CRCCheck_244 & 0xFF) >> 8) & 0xFF;
        data[23] = (ADS_CRCCheck_244 & 0xFF);
        data[24] = (std::uint64_t)((ACC_ACCTargetAcceleration + 10) * 20) & 0xFF;
        data[25] = (ACC_LDWStatus & 0x03) | ((ACC_LDWVibrationWarningReq & 0x03) << 2) | ((ACC_LKAStatus & 0x03) << 4);
        data[26] = ((ACC_LDWShakeLevStatus & 0x03) << 4) | ((ACC_EmergencyDataRrdReq & 0x03) << 6);
        data[27] = ((ACC_TextInfoForDriver & 0x3F) << 1) | ((ACC_DecToStop & 0x01) << 7);
        data[28] = 0;
        data[29] = (ACC_EngineStartStopRequest & 0x03) | ((ACC_CDDActive & 0x01) << 4);
        data[30] = ((ACC_Driveoff_Request & 0x01) << 7) | ((ACC_ACCMode & 0x07) << 4) | (ACC_RollingCounter_24E & 0x0F);
        data[31] = (ACC_CRCCheck_24E & 0xFF);
        data[32] = ((std::uint64_t)((ACC_AEBTargetDeceleration + 16) * 2000) >> 8) & 0xFF;
        data[33] = (std::uint64_t)((ACC_AEBTargetDeceleration + 16) * 2000) & 0xFF;
        data[34] = ((ACC_ABAlevel & 0x03) << 5) | ((ACC_AEBActive & 0x01) << 7) | ((ACC_ABAActive & 0x01) << 4) |
                   (ACC_AWBlevel & 0x0F);
        data[35] = ((((ACC_AccTrqReq + 5000) / 5) >> 13) & 0x01) | ((ACC_AEBVehilceHoldReq & 0x01) << 2) |
                   ((ACC_FCWPreWarning & 0x01) << 3) | ((ACC_FCWLatentWarning & 0x01) << 4) |
                   ((ACC_FCWActive & 0x01) << 5) | ((ACC_PrefillActive & 0x01) << 6) | ((ACC_AWBActive & 0x01) << 7);
        data[36] = ((((ACC_AccTrqReq + 5000) / 5) >> 5) & 0xFF);
        data[37] = ((ACC_AEBCtrlType >> 3) & 0x01) | ((ACC_AccTrqReqActive & 0x01) << 2) |
                   ((((ACC_AccTrqReq + 5000) / 5) & 0x1F) << 3);
        data[38] = (ACC_RollingCounter_25E & 0x0F) | ((ACC_AEBCtrlType & 0x07) << 5);
        data[39] = (ACC_CRCCheck_25E & 0x0FF);
        data[40] = ((ACC_ADCNotice >> 1) & 0x0FF);
        data[41] = (((ACC_SafetyNotice >> 6) & 0x01)) | ((ACC_TakeOverReq & 0x03) << 5) |
                   ((ACC_LatTakeoverReq & 0x03) << 1) | ((ACC_LngTakeOverReq & 0x03) << 3) |
                   ((ACC_ADCNotice & 0x01) << 7);
        data[42] = ((ACC_SafetyNotice & 0x3F) << 2);
        data[43] = ((ACC_HandsOnReq & 0x07) << 4);
        data[44] = ((ADC_WorkMode & 0x03) << 3) | ((ACC_AEBSpeedDropLevel & 0x03) << 1);
        data[45] = 0;
        data[46] = 0;
        data[47] = 0;
        data[48] = 0;
        data[49] = 0;
        data[50] = 0;
        data[51] = 0;
        data[52] = 0;
        data[53] = 0;
        data[54] = 0;
        data[55] = 0;
        data[56] = 0;
        data[57] = 0;
        data[58] = 0;
        data[59] = 0;
        data[60] = 0;
        data[61] = 0;
        data[62] = 0;
        data[63] = 0;
        return 64;
    }
};

#endif