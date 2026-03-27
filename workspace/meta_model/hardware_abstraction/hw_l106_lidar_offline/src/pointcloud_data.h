#ifndef __HUAWEI_LIDAR_DATA_PROTOCOL_H__
#define __HUAWEI_LIDAR_DATA_PROTOCOL_H__

#include <string>
#include <sstream>


namespace avos {
namespace driver {
#pragma pack(push,1)

#define MAX_ANGLE 	36000
#define REVERSE_SIGNED_SHORT(value)  (short)(((value)&0x00ff) << 8 | ((value) & 0xff00)>>8)
#define REVERSE_SHORT(value)  (unsigned short)(((value)&0x00ff) << 8 | ((value) & 0xff00)>>8)
#define REVERSE_INT(value)  (unsigned int)(((value)&0x000000ff) << 24 | ((value) & 0x0000ff00)<<8 |  \
							 ((value) & 0x00ff0000)>>8 | ((value) & 0xff000000)>>24 )
// #define MAX_LIDAR_RING 192
#define MAX_LIDAR_RING 96

typedef struct DataTail {
	unsigned char reserved_field[4];
}DataTail;

typedef struct ChannelDataArray {
	unsigned short distance;	//标识目标距离,分辨率:1/256 m(0-255m)
	unsigned short intensity;	//Reserved
	unsigned char reflectivity;	//表示目标反射率。
	unsigned short Distance() {
		return REVERSE_SHORT(distance);
	}
}ChannelDataArray;

typedef struct DataBlock {
	unsigned short flag;//指示当前点云对应的通道集合:固定为0xFFDD,表示通道号为0~191;
	short azimuth;// 垂直channel点云数据对应的水平角度,分辨率:0.00390625° (1/256)
	ChannelDataArray channel_data[MAX_LIDAR_RING];
	short Azimuth() {
		// return REVERSE_SHORT(azimuth);
		return REVERSE_SIGNED_SHORT(azimuth);
	}
}DataBlock;

typedef struct HWUTCTime {
	unsigned int timestamp_sec;
	unsigned int timestamp_usec;

	double GetSecondValue() {
		double timestamp  = REVERSE_INT(timestamp_sec) + (double)(REVERSE_INT(timestamp_usec)) / 1000000.0;
		return timestamp;
	}
}HWUTCTime;

typedef struct DataHead {
	unsigned char reserved_filed[4];
	unsigned char version;
	unsigned short frame_id;
	unsigned short package_counter;
	unsigned short package_seq_num;
	unsigned short phase_lock_num;
	unsigned char data_block_num;
	HWUTCTime utc_time;
	unsigned char echo_mode;
	unsigned char lut_index;
	uint8_t 	   package_frame_flag;//0:头帧;1:中间帧;2:尾帧
	unsigned short azimuth_idx0;
	unsigned short azimuth_idx1;
	unsigned char reserved_field2[24];

	unsigned short AzimuthIDX0() {
		return REVERSE_SHORT(azimuth_idx0);
	}
	unsigned short PackageSeqNum() {
		return REVERSE_SHORT(package_seq_num);
	}
	unsigned short FrameID() {
		return REVERSE_SHORT(frame_id);
	}
	uint8_t PkgPosition(){
		return package_frame_flag;
	}

	uint8_t get_DataBlockNum(){
		// 每包数据 BLOCK 数量;单回波 02;双回波 01
		return data_block_num;
	}

	uint8_t get_Echomode(){
		/*
		0x30:单回波 strong(最强回波)模式
		0x31:单回波 First(第一回波)模式
		0x32:单回波 Last(最后回波)模式
		0x40:双回波:strongest + last 回波模式
		0x41:双回波:strongest + First 回波模式
		0x42:双回波:First +Last 回波模式
		*/
		return echo_mode;
	}

}DataHead;

typedef struct HWPointCloudProtocol
{
	DataHead data_header;	//53
	DataBlock data_block[2];	//964 Bytes
	DataTail data_tail;		//4 Bytes

	int size()
	{
		return sizeof(HWPointCloudProtocol);
	}
} HWPointCloudProtocol;
#pragma pack(pop)

}
}

#endif
