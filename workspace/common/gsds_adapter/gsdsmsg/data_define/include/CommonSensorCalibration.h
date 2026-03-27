#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
常见传感器标定信息
*/

typedef struct CommonSensorCalibration{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	name;	//标定传感器名称
	int	prerotaxis_0;	//角度预处理轴0
	int	prerotaxis_1;	//角度预处理轴1
	int	prerotaxis_2;	//角度预处理轴2
	float	prerotdegree_0;	//角度预处理0
	float	prerotdegree_1;	//角度预处理1
	float	prerotdegree_2;	//角度预处理2
	float	xoffset;	//x轴偏移量
	float	yoffset;	//y轴偏移量
	float	zoffset;	//z轴偏移量
	float	roll;	//滚转角 度  [-90 90]
	float	pitch;	//俯仰角 度  [-90 90]
	float	yaw;	//偏航角 度  [-90 90]
	float	z_ground_offset;	//z轴地面偏移量

	CommonSensorCalibration(){
		key_hash_list.resize(18);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2049633320));
		key_hash_list.push_back(msghandle::type::fix_int32(1224860119));
		key_hash_list.push_back(msghandle::type::fix_int32(1673469755));
		key_hash_list.push_back(msghandle::type::fix_int32(2122079391));
		key_hash_list.push_back(msghandle::type::fix_int32(1222943882));
		key_hash_list.push_back(msghandle::type::fix_int32(640412224));
		key_hash_list.push_back(msghandle::type::fix_int32(57880566));
		key_hash_list.push_back(msghandle::type::fix_int32(711874287));
		key_hash_list.push_back(msghandle::type::fix_int32(1498940963));
		key_hash_list.push_back(msghandle::type::fix_int32(138523991));
		key_hash_list.push_back(msghandle::type::fix_int32(627663370));
		key_hash_list.push_back(msghandle::type::fix_int32(670757136));
		key_hash_list.push_back(msghandle::type::fix_int32(494093549));
		key_hash_list.push_back(msghandle::type::fix_int32(123836917));
		Clear();
	}

void Clear(){
	name={};
	prerotaxis_0={};
	prerotaxis_1={};
	prerotaxis_2={};
	prerotdegree_0={};
	prerotdegree_1={};
	prerotdegree_2={};
	xoffset={};
	yoffset={};
	zoffset={};
	roll={};
	pitch={};
	yaw={};
	z_ground_offset={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,name,prerotaxis_0,prerotaxis_1,prerotaxis_2,prerotdegree_0,prerotdegree_1,prerotdegree_2,xoffset,yoffset,zoffset,roll,pitch,yaw,z_ground_offset);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","name","prerotaxis_0","prerotaxis_1","prerotaxis_2","prerotdegree_0","prerotdegree_1","prerotdegree_2","xoffset","yoffset","zoffset","roll","pitch","yaw","z_ground_offset");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,name,prerotaxis_0,prerotaxis_1,prerotaxis_2,prerotdegree_0,prerotdegree_1,prerotdegree_2,xoffset,yoffset,zoffset,roll,pitch,yaw,z_ground_offset);
} CommonSensorCalibration;
REGISTER(CommonSensorCalibration,CommonSensorCalibration_Func);
