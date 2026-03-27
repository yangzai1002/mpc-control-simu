#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
华为4D毫米波输出的目标
*/

typedef struct RadarPoint4D{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	id;	//目标点 ID
	int	motionStatus;	//点云动态属性
	float	rangeStdDev;	//目标点径向距离标准差
	float	rangeRateStdDev;	//目标点径向速度标准差
	float	azimuthStdDev;	//目标点方位角标准差
	float	elevationStdDev;	//目标点俯仰角标准差
	int	existStatus;	//存在可能性
	int	falseAlarmProb;	//虚警概率
	int	heightConfidence;	//高度置信度
	int	attribState;	//点云属性状态信息
	int	modeFlag;	//点云波形标识
	float	x;	//x轴距离，单位m
	float	y;	//y轴距离，单位m
	float	z;	//z轴距离，单位m
	float	vx;	//x轴速度，单位m/s
	float	vy;	//y轴速度，单位m/s
	float	vz;	//z轴速度，单位m/s

	RadarPoint4D(){
		key_hash_list.resize(21);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(537695414));
		key_hash_list.push_back(msghandle::type::fix_int32(1076720249));
		key_hash_list.push_back(msghandle::type::fix_int32(1060552862));
		key_hash_list.push_back(msghandle::type::fix_int32(854109645));
		key_hash_list.push_back(msghandle::type::fix_int32(1175583928));
		key_hash_list.push_back(msghandle::type::fix_int32(1909250503));
		key_hash_list.push_back(msghandle::type::fix_int32(991192579));
		key_hash_list.push_back(msghandle::type::fix_int32(821836336));
		key_hash_list.push_back(msghandle::type::fix_int32(1291519972));
		key_hash_list.push_back(msghandle::type::fix_int32(1919850926));
		key_hash_list.push_back(msghandle::type::fix_int32(764599126));
		key_hash_list.push_back(msghandle::type::fix_int32(416570502));
		key_hash_list.push_back(msghandle::type::fix_int32(722513210));
		key_hash_list.push_back(msghandle::type::fix_int32(1028455918));
		key_hash_list.push_back(msghandle::type::fix_int32(758168757));
		key_hash_list.push_back(msghandle::type::fix_int32(443461385));
		key_hash_list.push_back(msghandle::type::fix_int32(128754013));
		Clear();
	}

void Clear(){
	id={};
	motionStatus={};
	rangeStdDev={};
	rangeRateStdDev={};
	azimuthStdDev={};
	elevationStdDev={};
	existStatus={};
	falseAlarmProb={};
	heightConfidence={};
	attribState={};
	modeFlag={};
	x={};
	y={};
	z={};
	vx={};
	vy={};
	vz={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,motionStatus,rangeStdDev,rangeRateStdDev,azimuthStdDev,elevationStdDev,existStatus,falseAlarmProb,heightConfidence,attribState,modeFlag,x,y,z,vx,vy,vz);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","motionStatus","rangeStdDev","rangeRateStdDev","azimuthStdDev","elevationStdDev","existStatus","falseAlarmProb","heightConfidence","attribState","modeFlag","x","y","z","vx","vy","vz");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,motionStatus,rangeStdDev,rangeRateStdDev,azimuthStdDev,elevationStdDev,existStatus,falseAlarmProb,heightConfidence,attribState,modeFlag,x,y,z,vx,vy,vz);
} RadarPoint4D;
REGISTER(RadarPoint4D,RadarPoint4D_Func);
