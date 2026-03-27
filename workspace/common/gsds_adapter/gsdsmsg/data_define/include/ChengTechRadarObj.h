#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
承泰科技radar传感器输出的目标数据
*/

typedef struct ChengTechRadarObj{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	id;	//目标id
	int	radar_type;	//雷达类型
	float	x;	//x方向位置，单位m
	float	y;	//y方向位置，单位m
	float	z;	//z方向位置，单位m
	float	vx;	//x方向的速度，单位m/s
	float	vy;	//y方向的速度，单位m/s
	float	ax;	//x方向的加速度，单位m/s^2
	float	ay;	//y方向的加速度，单位m/s^2
	int	DynamicProp;	//动态属性
	int	TrackStatus;	//航迹测量状态
	int	ExitProb;	//存在概率
	int	Class;	//类别
	float	length;	//长，单位m
	float	wide;	//宽，单位m
	float	hight;	//高，单位m
	bool	FarNearFlag;	//远近标志
	int	WarnFlag;	//报警标志
	float	RCS;	//RCS
	float	std_x;	//x位置标准差
	float	std_y;	//y位置标准差
	float	std_z;	//z位置标准差
	float	std_vx;	//x速度标准差
	float	std_vy;	//y速度标准差
	float	std_ax;	//x加速度标准差
	float	std_ay;	//y加速度标准差

	ChengTechRadarObj(){
		key_hash_list.resize(30);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(574509512));
		key_hash_list.push_back(msghandle::type::fix_int32(1775415006));
		key_hash_list.push_back(msghandle::type::fix_int32(1067068886));
		key_hash_list.push_back(msghandle::type::fix_int32(642592466));
		key_hash_list.push_back(msghandle::type::fix_int32(218116046));
		key_hash_list.push_back(msghandle::type::fix_int32(8410720));
		key_hash_list.push_back(msghandle::type::fix_int32(706976660));
		key_hash_list.push_back(msghandle::type::fix_int32(923170914));
		key_hash_list.push_back(msghandle::type::fix_int32(1802816190));
		key_hash_list.push_back(msghandle::type::fix_int32(87642918));
		key_hash_list.push_back(msghandle::type::fix_int32(61671877));
		key_hash_list.push_back(msghandle::type::fix_int32(1708999807));
		key_hash_list.push_back(msghandle::type::fix_int32(327929708));
		key_hash_list.push_back(msghandle::type::fix_int32(1299926750));
		key_hash_list.push_back(msghandle::type::fix_int32(666904950));
		key_hash_list.push_back(msghandle::type::fix_int32(2097403579));
		key_hash_list.push_back(msghandle::type::fix_int32(975579194));
		key_hash_list.push_back(msghandle::type::fix_int32(720521047));
		key_hash_list.push_back(msghandle::type::fix_int32(2121223614));
		key_hash_list.push_back(msghandle::type::fix_int32(1992072081));
		key_hash_list.push_back(msghandle::type::fix_int32(408293381));
		key_hash_list.push_back(msghandle::type::fix_int32(971998329));
		key_hash_list.push_back(msghandle::type::fix_int32(1689387640));
		key_hash_list.push_back(msghandle::type::fix_int32(105608940));
		key_hash_list.push_back(msghandle::type::fix_int32(1907771024));
		key_hash_list.push_back(msghandle::type::fix_int32(1762569420));
		Clear();
	}

void Clear(){
	id={};
	radar_type={};
	x={};
	y={};
	z={};
	vx={};
	vy={};
	ax={};
	ay={};
	DynamicProp={};
	TrackStatus={};
	ExitProb={};
	Class={};
	length={};
	wide={};
	hight={};
	FarNearFlag={};
	WarnFlag={};
	RCS={};
	std_x={};
	std_y={};
	std_z={};
	std_vx={};
	std_vy={};
	std_ax={};
	std_ay={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,radar_type,x,y,z,vx,vy,ax,ay,DynamicProp,TrackStatus,ExitProb,Class,length,wide,hight,FarNearFlag,WarnFlag,RCS,std_x,std_y,std_z,std_vx,std_vy,std_ax,std_ay);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","radar_type","x","y","z","vx","vy","ax","ay","DynamicProp","TrackStatus","ExitProb","Class","length","wide","hight","FarNearFlag","WarnFlag","RCS","std_x","std_y","std_z","std_vx","std_vy","std_ax","std_ay");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,radar_type,x,y,z,vx,vy,ax,ay,DynamicProp,TrackStatus,ExitProb,Class,length,wide,hight,FarNearFlag,WarnFlag,RCS,std_x,std_y,std_z,std_vx,std_vy,std_ax,std_ay);
} ChengTechRadarObj;
REGISTER(ChengTechRadarObj,ChengTechRadarObj_Func);
