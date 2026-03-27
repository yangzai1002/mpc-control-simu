#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
标准 imu 信息
*/

typedef struct SensorImu{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//消息头
	int	week;	//星期数
	double	utc_time;	//utc时间
	double	gyro_x;	//陀螺角速度（x） °/s  [-300, 300]
	double	gyro_y;	//陀螺角速度（y） °/s  [-300, 300]
	double	gyro_z;	//陀螺角速度（z） °/s  [-300, 300]
	double	acce_x;	//x向加速度  m/s^2 [-490, 490]
	double	acce_y;	//y向加速度  m/s^2 [-490, 490]
	double	acce_z;	//z向加速度  m/s^2 [-490, 490]
	double	temperature;	//温度  摄氏度   [-40, 60]

	SensorImu(){
		key_hash_list.resize(14);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1462902908));
		key_hash_list.push_back(msghandle::type::fix_int32(1725452674));
		key_hash_list.push_back(msghandle::type::fix_int32(1197279706));
		key_hash_list.push_back(msghandle::type::fix_int32(425893693));
		key_hash_list.push_back(msghandle::type::fix_int32(1230884393));
		key_hash_list.push_back(msghandle::type::fix_int32(2035875093));
		key_hash_list.push_back(msghandle::type::fix_int32(1602885718));
		key_hash_list.push_back(msghandle::type::fix_int32(822736330));
		key_hash_list.push_back(msghandle::type::fix_int32(42586942));
		key_hash_list.push_back(msghandle::type::fix_int32(1435479101));
		Clear();
	}

void Clear(){
	header={};
	week={};
	utc_time={};
	gyro_x={};
	gyro_y={};
	gyro_z={};
	acce_x={};
	acce_y={};
	acce_z={};
	temperature={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,week,utc_time,gyro_x,gyro_y,gyro_z,acce_x,acce_y,acce_z,temperature);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","week","utc_time","gyro_x","gyro_y","gyro_z","acce_x","acce_y","acce_z","temperature");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,week,utc_time,gyro_x,gyro_y,gyro_z,acce_x,acce_y,acce_z,temperature);
} SensorImu;
REGISTER(SensorImu,SensorImu_Func);
