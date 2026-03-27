#ifndef __HUAWEI_4D_RADAR_DATA_H__
#define __HUAWEI_4D_RADAR_DATA_H__


#include <string>
#include <sstream>
#include <stdint.h>
#include "data_pool.h"

#define REVERSE_SIGNED_SHORT(value)  (short)(((value)&0x00ff) << 8 | ((value) & 0xff00)>>8)
#define REVERSE_SHORT(value)  (unsigned short)(((value)&0x00ff) << 8 | ((value) & 0xff00)>>8)
#define REVERSE_INT(value)  (unsigned int)(((value)&0x000000ff) << 24 | ((value) & 0x0000ff00)<<8 |  \
							 ((value) & 0x00ff0000)>>8 | ((value) & 0xff000000)>>24 )
#define CULCULATE_TABLE_SIZE 360*100
#define  A_PI  3.14159265358979323846

namespace avos {
namespace driver {
#pragma pack(push,1)

typedef struct MsgHeader {
	// unsigned char udp_header[42];
	unsigned int message_id;
	unsigned short message_type;
	unsigned short length;

	MsgHeader() {

	}

	void Print(const std::string &prefix = "") {
		std::cout << prefix << " message_id:" << GetMessageId() << std::endl;
		std::cout << prefix << " message_type:" << GetMessageType() << std::endl;
		std::cout << prefix << " length:" << GetMessageLength() << std::endl;
	}

	unsigned int GetMessageId() {
		return (REVERSE_INT(message_id));
	}

	unsigned short GetMessageType() {
		return (REVERSE_SHORT(message_type));
	}

	unsigned short GetMessageLength() {
		return (REVERSE_SHORT(length));
	}


	bool isHeader() {
		if (GetMessageId() == 0x0001) {
			return true;
		}

		return false;
	}

	bool isContent() {
		if (GetMessageId() == 0x0002) {
			return true;
		}

		return false;
	}


} MsgHeader;

typedef struct E2ECheck {
	unsigned short e2e_length;
	unsigned short counter;
	unsigned int data_id;
	unsigned int checksum;
	E2ECheck() {
		e2e_length = 0;
		counter = 0;
		data_id = 0;
		checksum = 0;//REVERSE_INT(0xb9495b50);
	}

	unsigned short GetE2eLength() {
		return REVERSE_SHORT(e2e_length);
	}

	void SetWakeId() {
		data_id = REVERSE_INT(0x0100200d);
	}

	void SetVehicleId() {
		data_id = REVERSE_INT(0x01002009);
	}

	void SetConfigId() {
		data_id = REVERSE_INT(0x01002005);
	}
	// void AddCounter() {
	// 	counter = (++counter) % 65535;
	// }

	void FillCheckSum(const uint8_t *buffer, uint32_t length) {
		checksum = REVERSE_INT(CheckCrc32(buffer, length));
	}



	uint32_t ReflectedData(uint32_t data)
	{
		data = ((data & 0xffff0000) >> 16) | ((data & 0x0000ffff) << 16);
		data = ((data & 0xff00ff00) >> 8) | ((data & 0x00ff00ff) << 8);
		data = ((data & 0xf0f0f0f0) >> 4) | ((data & 0x0f0f0f0f) << 4);
		data = ((data & 0xcccccccc) >> 2) | ((data & 0x33333333) << 2);
		data = ((data & 0xaaaaaaaa) >> 1) | ((data & 0x55555555) << 1);

		return (data >> 24) & 0xff;


	}


	uint32_t ReflectedData32(uint32_t data)
	{
		data = ((data & 0xffff0000) >> 16) | ((data & 0x0000ffff) << 16);
		data = ((data & 0xff00ff00) >> 8) | ((data & 0x00ff00ff) << 8);
		data = ((data & 0xf0f0f0f0) >> 4) | ((data & 0x0f0f0f0f) << 4);
		data = ((data & 0xcccccccc) >> 2) | ((data & 0x33333333) << 2);
		data = ((data & 0xaaaaaaaa) >> 1) | ((data & 0x55555555) << 1);

		return data;

	}



	uint32_t CheckCrc32(const uint8_t *buffer, uint32_t length,
	                    uint32_t poly = 0xF4ACFB13,
	                    uint32_t init = 0xFFFFFFFF,
	                    bool refIn = true,
	                    bool refOut = true,
	                    uint32_t xorOut = 0xFFFFFFFF )
	{
		uint32_t i = 0;
		uint32_t crc = init;

		while (length--)
		{
			if (refIn == true)
			{
				crc ^= ReflectedData(*buffer++) << 24;
			}
			else
			{
				crc ^= (*buffer++) << 24;
			}

			for (i = 0; i < 8; i++)
			{
				if (crc & 0x80000000)
				{
					crc <<= 1;
					crc ^= poly;
				}
				else
				{
					crc <<= 1;
				}
			}
		}

		if (refOut == true)
		{
			crc = ReflectedData32(crc);
		}

		return crc ^ xorOut;
	}


} E2ECheck;




typedef struct RadarPointHeader
{
	MsgHeader header;  					// 请求消息 ID:0x0001  type:0x02
	unsigned char interface_version;    // 接口版本号,当前固定填 3
	unsigned char packageNum;			// 点云消息 UDP数据包数量,每个包 50 个点云
	unsigned short clusterNum;			// 原始点云集群数量
	unsigned short clusterReportCnt;		// 从雷达启动开始计数,每次上报加 1,大于65535 回到 0 继续计数
	unsigned short uamb_range_rate;		// 速度无模糊范围

	unsigned int timestamp_sec;
	unsigned short timestamp_msec;

	unsigned char reserved0;
	unsigned char reserved1;
	unsigned char scaling;				// 缩放系数:为解决泊车模式下点云径向距离精度更高,检测距离更短的需求
	unsigned char block_flag;			// 遮挡告警标志 0:无遮挡  1:遮挡
	unsigned char reserved2;
	unsigned char block_level;			//遮挡告警程度  0:轻微遮挡  1:一般遮挡  2:严重遮挡  3:超强遮挡
	unsigned char reserved3[4];

	E2ECheck e2e;

	RadarPointHeader() {

	}

	long long frame_ts_ms()
	{
		return (REVERSE_INT(timestamp_sec)*1000 + REVERSE_SHORT(timestamp_msec));
	}

	unsigned short pkg_num()
	{
		return packageNum;
	}

	unsigned short obj_num()
	{
		return REVERSE_SHORT(clusterNum);
	}

	unsigned short frame_id()
	{
		return REVERSE_SHORT(clusterReportCnt);
	}

	void FillCheckSum( ) {
		e2e.FillCheckSum((uint8_t *)&header, sizeof(RadarPointHeader) - 4);
	}

} RadarPointHeader;


typedef struct RadarClusterInfo {
	unsigned short cluster_id;   // 目标点 ID
	unsigned char motion_state;  // 点云动态属性  0x0: stationary  0x1: moving  0x3: unknown
	short range_rate;   // 目标点径向速度
	short azimuth;		 // 目标点方位角
	short elevation;	 // 目标点俯仰角
	unsigned char range_std_dev; // 目标点径向距离标准差
	unsigned char range_rate_std_dev;  // 目标点径向速度标准差
	unsigned char azimuth_std_dev;     // 目标点方位角标准差
	unsigned char elevation_std_dev;   // 目标点俯仰角标准差
	unsigned char reserved0;
	unsigned char snr;				   // 点云信噪比
	unsigned char exist_status;        // 存在可能性  0:高存在概率点   1:低存在概率点
	unsigned char false_alarm_prob;    // 虚警概率
	unsigned char height_confidence;   // 高度置信度  0:高度高置信度点  1: 高度低置信度点
	unsigned char attrib_state;		   // 点云属性状态信息
	unsigned short range;			   // 目标点径向距离
	unsigned char ambg_state;		   // 点云多普勒状态
	unsigned char mode_flag;		   // 点云波形标识  0:行车模式,径向距离不需要进行缩放  1:泊车模式,径向距离需要按缩放系数进行缩放
	unsigned char reserved1;


unsigned short GetClusterId() {
		return REVERSE_SHORT(cluster_id);
	}

	float GetRangeRate() {
		return (REVERSE_SHORT(range_rate) * 0.02  - 128 );
	}

	float GetAzimuth() {
		return (REVERSE_SHORT(azimuth) * 0.025  - 90 );
	}

	float GetElevation() {
		return ((REVERSE_SHORT(elevation) * 0.025) - 90);
	}

	float GetRange() {
		return (float)(REVERSE_SHORT(range) * 0.05);
	}


	void GetObjectData(float &x, float &y, float &z,
	                   float &vx, float &vy, float &vz) {
		float t_range_rate = GetRangeRate();
		float t_range = GetRange();
		float t_azimuth = GetAzimuth() * A_PI / 180.0;
		float t_elevation = GetElevation() * A_PI / 180.0;

		x = t_range * cos(t_elevation) * cos(t_azimuth);
		y = t_range * cos(t_elevation) * sin(t_azimuth);
		z = t_range * sin(t_elevation);

		vx = t_range_rate * cos(t_elevation) * cos(t_azimuth);
		vy = t_range_rate * cos(t_elevation) * sin(t_azimuth);
		vz = t_range_rate * sin(t_elevation);

	}


	// unsigned short GetClusterId() {
	// 	return REVERSE_SHORT(cluster_id);
	// }

	// float GetRangeRate() {
	// 	return REVERSE_SIGNED_SHORT(range_rate) * 0.02;
	// }

	// int GetAzimuth() {//目标点方位角
	// 	if(REVERSE_SIGNED_SHORT(azimuth) < 0)
	// 	{
	// 		return ((int)((REVERSE_SIGNED_SHORT(azimuth)+360) * 0.025 * 100))%CULCULATE_TABLE_SIZE;
	// 	}
	// 	else
	// 	{
	// 		return ((int)(REVERSE_SIGNED_SHORT(azimuth) * 0.025 * 100))%CULCULATE_TABLE_SIZE;
	// 	}
	// }

	// int GetElevation() {//目标点俯仰角
	// 	if(REVERSE_SIGNED_SHORT(elevation)<0)
	// 	{
	// 		return ((int)((REVERSE_SIGNED_SHORT(elevation)+360) * 0.025 * 100))%CULCULATE_TABLE_SIZE;
	// 	}
	// 	else
	// 	{
	// 		return ((int)(REVERSE_SIGNED_SHORT(elevation) * 0.025 * 100))%CULCULATE_TABLE_SIZE;
	// 	}
	// }

	// float GetRange() {
	// 	return (float)(REVERSE_SHORT(range) * 0.05);
	// }

	void Print(const std::string &prefix = "") {
		// std::cout << "size:" << sizeof(RadarClusterInfo) << std::endl;

		// unsigned char *out = (unsigned char*)&cluster_id;
		// for (int i = 0; i < sizeof(RadarClusterInfo); i++)
		// {
		// 	printf(" %02x", out[i]);
		// }
		// printf("\n");

		float x, y, z, vx, vy, vz;
		GetObjectData(x, y, z, vx, vy, vz);
		std::cout << prefix << "cluster_id:" << GetClusterId() << std::endl;
		std::cout << prefix << "motion_state:" << (int)motion_state << std::endl;
		std::cout << prefix << "range_rate:" << GetRangeRate() << std::endl;
		std::cout << prefix << "azimuth:" << GetAzimuth() << std::endl;
		std::cout << prefix << "elevation:" << GetElevation() << std::endl;
		std::cout << prefix << "range:" << GetRange() << std::endl;
		std::cout << prefix << "----------------------" << std::endl;
		std::cout << prefix << "x:" << x << std::endl;
		std::cout << prefix << "y:" << y << std::endl;
		std::cout << prefix << "z:" << z << std::endl;
		std::cout << prefix << "vx:" << vx << std::endl;
		std::cout << prefix << "vy:" << vy << std::endl;
		std::cout << prefix << "vz:" << vz << std::endl;

	}

} RadarClusterInfo;


typedef struct RadarPointContent
{
	MsgHeader header;					// id:0x0002  type:0x02
	unsigned short package_id;			// 数据包索引号,不超过点云头消息中packageNum
	unsigned short package_cluster_num;  // 数据包中有效的点云数量,不超过 50 个
	RadarClusterInfo cluster_info[50];  // 点云 N 的有效信息,当packageClusterNum>N 时有效

	E2ECheck e2e;

	RadarPointContent() {
	}


	unsigned short GetPackageId() {
		return REVERSE_SHORT(package_id);
	}

	unsigned short GetPackageClusterNum() {
		return REVERSE_SHORT(package_cluster_num);
	}



	void Print(const std::string &prefix = "") {
		header.Print(prefix + "header\t:");
		std::cout << prefix << " package_id:" << GetPackageId() << std::endl;
		std::cout << prefix << " package_cluster_num:" << GetPackageClusterNum() << std::endl;

		for (int i = 0; i < GetPackageClusterNum(); i++) {
			std::cout << "object_cloud:" << i << std::endl;
			cluster_info[i].Print("\t");
		}

		std::cout << "e2e length:" << e2e.GetE2eLength() << std::endl;
	}


	void FillCheckSum( ) {
		e2e.FillCheckSum((uint8_t *)&header, sizeof(RadarPointHeader) - 4);
	}

} RadarPointContent;


typedef struct ConfigRadarMsg {
	MsgHeader header;
	unsigned char reserved0;
	unsigned char conn_outlet_dir_valid;     // 外置连接器侧出口方向配置是否有效  0:无效  1:有效
	unsigned char reserved1[2];
	unsigned char pfm_mode_valid; 			 // Radar 性能模式设置是否有效   0:无效   1:有效
	unsigned char radar_pos_valid; 			 // Radar 位置配置是否有效   0:无效   1:有效
	unsigned char reserved2[3];
	unsigned char cfg_conn_out_dir;			 // 外置连接器侧出口方向 0:左侧 1:右侧 默认值:0
	unsigned char reserved3[2];
	unsigned char cfg_pfm_mode;				 // 0:行车模式, 1:泊车模式  默认值:0
	unsigned char cfg_radar_pos;			 // 0:前向雷达  1:左前雷达  2:左后角雷达  3:后向雷达 4:右后角雷达  5:右前雷达  默认值:0(MRR)
	unsigned char reserved4[10];
	E2ECheck e2e;

	ConfigRadarMsg() {

	}

	void FillCheckSum( ) {
		e2e.FillCheckSum((uint8_t *)&header, sizeof(RadarPointHeader) - 4);
	}

} ConfigRadarMsg;


typedef struct VehicleInfoMsg {
	MsgHeader header;
	unsigned char veh_spd_dir;				 // 车辆运动方向  0:静止 1:向前运动  2:向后运动
	short yawrate;					 // 横摆角速度(逆时针为正 顺时针为负) 单位:deg/s
	unsigned short veh_spd;					 // 车速 单位:m/s

	unsigned char longitude_acc_valid;		 // 纵向加速度有效性 0x0-invalid  0x1-valid
	short longitude_acc;		 	 // 纵向加速度(前正后负) 单位:m/s2

	unsigned char lateral_acc_valid;		 // 纵向加速度有效性 0x0-invalid  0x1-valid
	short lateral_acc;		 	 // 纵向加速度(前正后负) 单位:m/s2

	unsigned char wheel_speed_valid;		 // 轮速信息有效性 0x0-invalid  0x1-valid
	short wheel_speed_fl;			 // 前左轮速(前正后负) 单位:m/s
	short wheel_speed_fr;			 // 前右轮速(前正后负) 单位:m/s
	short wheel_speed_rr;			 // 后右轮速(前正后负) 单位:m/s
	short wheel_speed_rl;			 // 后左轮速(前正后负) 单位:m/s

	unsigned char steer_angle_valid;		 // 方向盘转角有效性 0x0-invalid  0x1-valid
	short steer_angle;				// 方向盘转角(逆时针为正,顺时针为负)

	unsigned char steer_velocity_valid;		 // 方向盘转角速度 0x0-invalid  0x1-valid
	short steer_velocity;			 // 方向盘转角速度(逆时针为正,顺时针为负)

	unsigned char gear_status;		 // 实际档位信息 0x0-fail 0x1-P 0x2-N 0x3-R  0x4-Gear 1   0x5-Gear 2 0x6-Gear 3  0xF-not defined

	unsigned char wheel_scale_factor_valid;  // 尺度因子有效性
	unsigned char wheel_scale_factor;		 // 尺度因子

	unsigned char rainfall_amount ;          // 雨量的表示,0:NA;(未知,标识无法获取雨量)  1:large;  2:medium; 3:small; 4:none;
	unsigned char reserved1[6];

	E2ECheck e2e;

	VehicleInfoMsg() {

		header.message_id = REVERSE_INT(0x0104);
		header.message_type = REVERSE_SHORT(0x01);
		header.length = REVERSE_SHORT(36);
		veh_spd_dir = 0x00;
		yawrate = REVERSE_SHORT(0x00);
		veh_spd = REVERSE_SHORT(0x00);

		longitude_acc_valid = 0x01;
		longitude_acc = REVERSE_SHORT(0x00);

		lateral_acc_valid = 0x01;
		lateral_acc = REVERSE_SHORT(0x00);

		// wheel_speed_valid = 0x00;
		wheel_speed_valid = 0x01;

		wheel_speed_fl = REVERSE_SHORT(0x00);
		wheel_speed_fr = REVERSE_SHORT(0x00);
		wheel_speed_rr = REVERSE_SHORT(0x00);
		wheel_speed_rl = REVERSE_SHORT(0x00);

		steer_angle_valid = 0x01;
		steer_angle = REVERSE_SHORT(0x00);

		steer_velocity_valid = 0x01;
		steer_velocity = REVERSE_SHORT(0x00);

		gear_status = 0x0f;

		wheel_scale_factor_valid = 0x00;

		wheel_scale_factor = 0x00;
		rainfall_amount = 0x00;
		memset(reserved1, 0, 6);


		e2e.e2e_length = REVERSE_SHORT(sizeof(VehicleInfoMsg));
		e2e.data_id = REVERSE_INT(0x01002009);
	}

	void SetData(const VehicleInfo& veh_info)
	{
		veh_spd_dir = veh_info.veh_spd_dir;
		yawrate = REVERSE_SIGNED_SHORT((short)(veh_info.yawrate * 100));
		veh_spd = REVERSE_SHORT((unsigned short)(veh_info.vehSpd * 100));
		gear_status = veh_info.gear;
		// 车辆运动方向  0:静止 1:向前运动  2:向后运动
		if(1 == veh_spd_dir)
		{
			wheel_speed_fl = REVERSE_SHORT((unsigned short)(veh_info.vehSpd * 100));
		}
		if(2 == veh_spd_dir)
		{
			wheel_speed_fl = REVERSE_SIGNED_SHORT((short)(-(veh_info.vehSpd * 100)));
		}
		wheel_speed_fr = wheel_speed_fl;
		wheel_speed_rr = wheel_speed_fl;
		wheel_speed_rl = wheel_speed_fl;
		steer_angle = REVERSE_SHORT((short)(veh_info.steer_angle * 10));
		steer_velocity = REVERSE_SHORT((short)(veh_info.steeringVelocity * 10));
		longitude_acc = REVERSE_SHORT((short)(veh_info.longitudeAcc * 50));
		lateral_acc = REVERSE_SHORT((short)(veh_info.lateralAcc * 50));
	}


	void FillCheckSum( ) {
		static int counter = 0;
		counter ++;
		if (++counter > 65530) {
			counter = 0;
		}
		e2e.counter = REVERSE_SHORT(counter);


		e2e.FillCheckSum((uint8_t *)&header, sizeof(VehicleInfoMsg) - 4);
	}

} VehicleInfoMsg;


typedef struct ResponseMsg {
	MsgHeader header;
	unsigned char reserved[8];
	E2ECheck e2e;
	ResponseMsg() {

	}
} ResponseMsg;


typedef struct HeartBeatMsg {
	MsgHeader header;
	unsigned char reserved[8];
	E2ECheck e2e;
	HeartBeatMsg() {
		header.message_id = REVERSE_INT( 0x0107);
		header.message_type = REVERSE_SHORT(0x01);
		header.length = REVERSE_SHORT(0x08);
		memset(reserved, 0, 8);

		// e2e.SetWakeId();
		e2e.e2e_length = REVERSE_SHORT(sizeof(HeartBeatMsg));
		e2e.data_id = REVERSE_INT(0x0100200d);
		// e2e.data_id = 0x0100200d;
	}



	void FillCheckSum( ) {

		static int counter = 0;
		counter ++;
		if (++counter > 65530) {
			counter = 0;
		}

		e2e.counter = REVERSE_SHORT(counter);
		e2e.FillCheckSum((uint8_t *)&header, sizeof(HeartBeatMsg) - 4);
	}
} HeartBeatMsg;


typedef struct ErrCodeMsg {
	// Radar->Host 通知消息使用 52373 作为发送端口,默认接收报文的 UDP 端口是 52373
	MsgHeader header;
	unsigned char nvmErrorSt;		// 在启动时从非易失性存储器读取/写入配置参数的状态. 0:successful, 1:failed
	unsigned char fatalErrorSt;     // 检测到复位后可能不会消失的内部错误. 0:No Error, 1:Error
	unsigned char generalErrorSt;   // 检测到复位后很可能会消失的内部错误, 0:No Error, 1:Error
	unsigned char tempAlarmSt; 	    // 一般告警下 Radar 会继续工作,严重告警后,Radar 会停止工作, 0x0:Normal, 0x1:Minor abnormal, 0x2:Major abnormal
	unsigned char voltageAlarmSt; 	// 如果工作电压低于或高于定义范围,则错误将激活, 0x0:Normal, 0x1:Minor abnormal, 0x2:Major abnormal
	unsigned char interferenceSt;	// 已经检测受到其他雷达传感器的干扰。在干扰的情况下,雷达的任何接口功能都没有变化。在内部,背景噪声增加,这会根据干扰强度缓慢降
										//低性能,但只要检测到任何障碍物,雷达就会继续向接口发送集群或物体。0:没有干扰, 1:有干扰
	unsigned char blockAlarmSt;		// Blockage Detection 告警标志 0:no block, 1:partional block, 2:full block
	unsigned char velInfoSt;		// 连续 500ms 没有收到任何车辆速率信息的消息(收到的消息有误也认为没有收到)上报该故障, 0:No Error, 1:Error
	unsigned char eleAxisErrSt;		// 雷达安装位置由于震动等原因导致中心轴线偏离原定位置. 0:no misalignment, 1: minor misalignment, 2: major misalignment
	unsigned char cfgAbnormalSt;	// 安装参数错误或者车型未配置, 0:No Error, 1:Error
	unsigned char unCalAlarm;		// 未标定模块不能直接使用, 0:Normal, 1:No Calibrate
	unsigned char TsnSynSt;			// Host 与传感器之间的时钟同步信号异常, 0x0:Normal, 0x1:Invalid
	unsigned char TriggerLogUpload; // 触发日志上传的指示标志, 0:No trigger, 1:Trigger
	unsigned char Reserved; 		// (固定填 1)
	unsigned char Reserved1[9]; 		// 告警保留字段 9 字节,用于后续扩展

	E2ECheck e2e;

	ErrCodeMsg() {
		// header.message_id = REVERSE_INT( 0x0106);
		// header.message_type = REVERSE_SHORT(0x02);
		// header.length = REVERSE_SHORT(0x08);
	}

	void FillCheckSum( ) {
		e2e.FillCheckSum((uint8_t *)&header, sizeof(ErrCodeMsg) - 4);
	}

} ErrCodeMsg;

#pragma pack(pop)

}
}




#endif
