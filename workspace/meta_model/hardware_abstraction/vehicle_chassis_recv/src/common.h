#ifndef _COMMON_H_
#define _COMMON_H_
// added by XieMin to avoid multi citing !


namespace protocol_common{

const char RecvIP[] = "26.28.19.1";
const int RecvPORT = 30001;
const char SenderIP[] = "26.28.19.2";
const int SenderPORT = 30002;

//
// typedef enum Result_Code { OK, CRC_ERROR, UNKNOW };
enum class Result_Code { OK, CRC_ERROR, UNKNOW };

unsigned short calculate_crc16_x25(const char *puchMsg, int usDataLen);

void SetUdpOutGoing(unsigned char *s, int length, bool b_close = false);

// 变量定义
#define WGS_AXIS_A 6378137.0
 #define PIT 3.1415926535897932
#define USERRAD2DEG (180.0 / PIT)
#define USERDEG2RAD (PIT / 180.0)
#define WGS84_E 0.0818191908
#define UTM_E2 (WGS84_E * WGS84_E)
#define UTM_K0 0.9996
#define BOOL S32
#define TRUE 1
#define FALSE 0
#define RECV_ZJZT 0xE0
#define RECV_DHXX 0xE5
/*INS系统状态的定义*/
#define INS_Calibration_State 0x8000      // 标定状态
#define INS_OD_ACTIVE 0x4000              // 惯导/里程计组合有效
#define Fault_State 0x1000                // 故障状态
#define Navigation_State 0x0401           // 导航状态
#define OD_Assist_Alignment_State 0x0100  // 里程计辅助对准状态
#define INS_Waiting_State 0x0040          // 待机状态
#define INS_Zero_Speed_Alignment 0x0021   // 零速对准状态
#define INS_Start_State 0x0010            // 启动状态
#define INS_Selfcheck_Success 0x00001     // 自检成功
#define INS_Selfcheck_Fail 0x0004         // 自检失败
#define INS_Mask 0xD575  // 防止其他位变化 影响对INS系统状态的判断
/*
#define INS_Calibration_State 0x8000         // 标定状态
#define INS_OD_ACTIVE 0x4000                 // 惯导/里程计组合有效
#define Fault_State 0x1000                   // 故障状态
#define Navigation_State 0x0400              // 导航状态
#define OD_Assist_Alignment_State 0x0100     // 里程计辅助对准状态
#define INS_Waiting_State 0x0040             // 待机状态
#define INS_Zero_Speed_Alignment 0x0020      // 零速对准状态
#define INS_Start_State 0x0010               // 启动状态
#define INS_Selfcheck_Success   0x00001      // 自检成功
#define INS_Selfcheck_Fail 0x0004            // 自检失败
#define INS_Mask 0xD575 //防止其他位变化 影响对INS系统状态的判断
*/
//************change time************//
#define BEIJINGTIME 8
#define DAY (60 * 60 * 24)
#define YEARFIRST 2001
#define YEARSTART (365 * (YEARFIRST - 1970) + 8)
#define YEAR400 (365 * 4 * 100 + (4 * (100 / 4 - 1) + 1))
#define YEAR100 (365 * 100 + (100 / 4 - 1))
#define YEAR004 (365 * 4 + 1)
#define YEAR001 365

//****坐标变换****//
char UTMLetterDesignator(double Lat);


// void LLtoUTM(Inertia2Auto *inp, int& ZoneNumber);

// void UTMtoLL(Inertia2Auto *inp, const int ZoneNumber);

};  // namespace protocol_common

#endif  // _COMMON_H_
