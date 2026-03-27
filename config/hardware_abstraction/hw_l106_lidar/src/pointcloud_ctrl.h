#ifndef __HUAWEI_LIDAR_CTRL_PROTOCOL_H__
#define __HUAWEI_LIDAR_CTRL_PROTOCOL_H__

#include <string>
#include <sstream>


namespace avos {
namespace driver {
#pragma pack(push,1)
#define REVERSE_SIGNED_SHORT(value)  (short)(((value)&0x00ff) << 8 | ((value) & 0xff00)>>8)
#define REVERSE_SHORT(value)  (unsigned short)(((value)&0x00ff) << 8 | ((value) & 0xff00)>>8)
#define REVERSE_INT(value)  (unsigned int)(((value)&0x000000ff) << 24 | ((value) & 0x0000ff00)<<8 |  \
							 ((value) & 0x00ff0000)>>8 | ((value) & 0xff000000)>>24 )



typedef struct MsgHead {
	unsigned int message_id;
	unsigned short message_type;
	unsigned short length;
	MsgHead() {
		message_id = REVERSE_INT(0x00F0);
		message_type = 0;
		length = 0;
	}

	unsigned int GetID() {
		return REVERSE_INT(message_id);
	}
	unsigned short GetType() {
		return REVERSE_SHORT(message_type);
	}
	uint16_t size()
	{
		return sizeof(MsgHead);
	}
}MsgHead;

typedef struct E2ECheck {
	unsigned short e2e_length;
	unsigned short counter;
	unsigned int data_id;
	unsigned int checksum;
	E2ECheck() {
		e2e_length = 0;
		counter = 0;
		data_id = REVERSE_INT(0x01001004);
		checksum = REVERSE_INT(0xb9495b50);
	}
	void AddCounter() {
		counter++;
	}
	uint16_t size()
	{
		return sizeof(E2ECheck);
	}
}E2ECheck;

typedef struct CheckAlive
{
	MsgHead msg_head;
	E2ECheck e2e_check;
	CheckAlive() {
		msg_head.message_id = REVERSE_INT(0x00F0);

		e2e_check.data_id = REVERSE_INT(0x01001004);//0x01001004
		e2e_check.e2e_length = REVERSE_SHORT(sizeof(CheckAlive));
		e2e_check.checksum = REVERSE_INT(0xb9495b50);
	}
} CheckAlive;

typedef struct SuppleInfo
{
	MsgHead msg_head;
	unsigned short data_id;
	E2ECheck e2e_check;
	SuppleInfo() {
		msg_head.message_id = REVERSE_INT(0x0031);
		msg_head.length = REVERSE_SHORT(0x0002);
		e2e_check.data_id = REVERSE_INT(0x01001002);//0x01001002
		e2e_check.e2e_length = REVERSE_SHORT(sizeof(SuppleInfo));
		// e2e_check.checksum = REVERSE_INT(0xb9495b50);
	}
} SuppleInfo;

typedef struct VerticalAngle
{
	short vertical_angle[576];//1152, 0~192

	short Angle(int i) {
		if (i < 0 || i >= 576) {
			return -1;
		}
		return REVERSE_SIGNED_SHORT(vertical_angle[i]);
	}
} VerticalAngle;

typedef struct HorizontalAngleOffset
{
	short vertical_angle[576];//1152, 0~192
	short Angle(int i) {
		if (i < 0 || i >= 576) {
			return -1;
		}
		return REVERSE_SIGNED_SHORT(vertical_angle[i]);
	}
} HorizontalAngleOffset;

typedef struct VerticalAngleOffset
{
	short vertical_angle[1500];//3000, 0~480
	short Angle(int i) {
		if (i < 0 || i >= 1500) {
			return -1;
		}
		return REVERSE_SIGNED_SHORT(vertical_angle[i]);
	}
} VerticalAngleOffset;


typedef struct Payload
{
	unsigned short data_id;
	unsigned short length;
	union DataRecord
	{
		VerticalAngle vertical_angle;
		VerticalAngleOffset vertical_angle_offset;
		HorizontalAngleOffset horizontal_angle_offset;
	}DataRecord;

	// DataRecord data_record;
	unsigned int GetID() {
		return REVERSE_SHORT(data_id);
	}
} Payload;


typedef struct PointcloudCtrl //点云数据补充信息的响应消息
{
	MsgHead msg_head;
	Payload pay_load;
	E2ECheck e2e_check;
} PointcloudCtrl;


typedef struct LidarConfig {
	unsigned int message_id;
	short message_type;
	short length;
	unsigned char frame_mode;
	unsigned char echo_mode;
	unsigned char reflectivity_mode;
	unsigned char antiinterference_mode;
	unsigned char resolution_mode;
	unsigned char interfaceVer_mode;
	unsigned short phase_lock;
	// unsigned char frame_mode;
	E2ECheck e2e_check;
	LidarConfig() {
		message_id = REVERSE_INT(0x0111);
		message_type = 0x00;
		length = REVERSE_SHORT(0x08);
		frame_mode = 0x00;
		echo_mode = 0x30; //单回波 strong(最强回波)模式
		reflectivity_mode = 0;
		antiinterference_mode = 3;
		resolution_mode = 1;
		interfaceVer_mode = 1;
		phase_lock = REVERSE_SHORT(0x358);// 0x78: 120; 0x5A: 90;   0x3c: 60; 0x358: 600

		e2e_check.data_id = REVERSE_INT(0x01001006); //0x01001006
		e2e_check.e2e_length = REVERSE_SHORT(sizeof(LidarConfig));
	}

	void Checksum() {
		const uint32_t polynomial = 0xF4ACFB13;  // Polynomial
		const int width = 32;                    // Width of CRC


		// e2e_check.checksum =
		// REVERSE_INT(CheckCrc32(polynomial, 0xFFFFFFFF, true, true, 0xFFFFFFFF,
		// 	(const uint8_t *)&message_id, sizeof(LidarConfig) - 4));
	}
}LidarConfig;

typedef struct ErrCodeDetail
{
	uint8_t EthPhyErrorState;
	uint8_t SocCtrlErrorState;
	uint8_t ReceiveModuleErrorState;
	uint8_t SendModuleErrorState;
	uint8_t InnerModulePowerState;
	uint8_t ElectricMotorErrorState;
	uint8_t WindowsHeatErrorState;
	uint8_t EthCRCErrorState;
	uint8_t SQIInadequateState;
	uint8_t SoftwareErrorState;
	uint8_t HighTemperatureErrorState;
	uint8_t LowTemperatureErrorState;
	uint8_t HighVoltageErrorState;
	uint8_t LowVoltageErrorState;
	uint8_t CfgErrorState;
	uint8_t TimeSynLossState;
	uint8_t TimeSynJumpState;
	uint8_t DeviceCertiState;
	uint8_t BlockState;
	uint8_t CommunicationLossState;
	uint8_t CommunicationDataInvalidState;
	uint8_t RangingDegradationState;
	uint8_t RanginggenerallyDegradationState;
	uint8_t LogUploadState;
	uint16_t Reserved; //byte 25/26
	uint8_t  LidarHeatStatus;
	uint8_t BlindType;
	uint8_t BlindLevel1;
	uint8_t ConfidenceLevel1;
	uint8_t BlindLevel2;
	uint8_t ConfidenceLevel2;
	uint8_t BlindLevel3;
	uint8_t ConfidenceLevel3;
	uint8_t BlindLevel4;
	uint8_t ConfidenceLevel4;
	uint8_t BlindLevel5;
	uint8_t ConfidenceLevel5;
	uint8_t BlindLevel6;
	uint8_t ConfidenceLevel6;
	uint8_t BlindLevel7;
	uint8_t ConfidenceLevel7;
	uint8_t BlindLevel8;
	uint8_t ConfidenceLevel8;
	uint8_t BlindLevel9;
	uint8_t ConfidenceLevel9;
	uint8_t BlindLevel10;
	uint8_t ConfidenceLevel10;
	uint8_t BlindLevel11;
	uint8_t ConfidenceLevel11;
	uint8_t BlindLevel12;
	uint8_t ConfidenceLevel12;
	uint8_t BlindLevel13;
	uint8_t ConfidenceLevel13;
	uint8_t BlindLevel14;
	uint8_t ConfidenceLevel14;
	uint8_t BlindLevel15;
	uint8_t ConfidenceLevel15;
	uint8_t BlindLevel16;
	uint8_t ConfidenceLevel16;
	uint8_t BlindLevel17;
	uint8_t ConfidenceLevel17;
	uint8_t BlindLevel18;
	uint8_t ConfidenceLevel18;

	uint16_t size()
	{
		return sizeof(ErrCodeDetail);
	}
}ErrCodeDetail;


typedef struct LidarErrCode {
	MsgHead msg_head; //0x0380 0x02 64
	ErrCodeDetail pay_load;
	E2ECheck e2e_check;
	uint16_t size()
	{
		return sizeof(LidarErrCode);
	}
}LidarErrCode;


#pragma pack(pop)

}
}
#endif
