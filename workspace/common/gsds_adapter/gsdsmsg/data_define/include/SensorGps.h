#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
标准GPS信号
*/

typedef struct SensorGps{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//消息头
	double	lon;	//经度[-180,180]
	double	lat;	//纬度[-180,180]
	double	height;	//水平高度(m)
	double	heading;	//航向角
	double	velocity;	//GPS速度（m/s）
	double	up_velocity;	//北向速度（m/s）
	double	track_angle;	//接收机当前航迹角[-180, 180]
	int	week;	//星期数
	double	utc_time;	//utc时间
	double	hdop;	//水平精度因子（误差放大倍数）[0, 5]
	double	diff_age;	//差分期龄[0, 300]
	double	base_length;	//基线长度（m）
	double	xg;	//平移后的x坐标
	double	yg;	//平移后的y坐标
	double	zg;	//平移后的z坐标
	int	status;	//GPS状态位 0:无定位;  1:单点定位定  2:伪距差分  4:固定解  5:浮点解
	int	satenum;	//卫星数

	SensorGps(){
		key_hash_list.resize(22);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1462902908));
		key_hash_list.push_back(msghandle::type::fix_int32(1726242845));
		key_hash_list.push_back(msghandle::type::fix_int32(1578307515));
		key_hash_list.push_back(msghandle::type::fix_int32(426123230));
		key_hash_list.push_back(msghandle::type::fix_int32(1703433656));
		key_hash_list.push_back(msghandle::type::fix_int32(1832308861));
		key_hash_list.push_back(msghandle::type::fix_int32(164684583));
		key_hash_list.push_back(msghandle::type::fix_int32(1736717425));
		key_hash_list.push_back(msghandle::type::fix_int32(1725452674));
		key_hash_list.push_back(msghandle::type::fix_int32(1197279706));
		key_hash_list.push_back(msghandle::type::fix_int32(11201884));
		key_hash_list.push_back(msghandle::type::fix_int32(1917823804));
		key_hash_list.push_back(msghandle::type::fix_int32(382814413));
		key_hash_list.push_back(msghandle::type::fix_int32(1924730367));
		key_hash_list.push_back(msghandle::type::fix_int32(418706733));
		key_hash_list.push_back(msghandle::type::fix_int32(1060166747));
		key_hash_list.push_back(msghandle::type::fix_int32(1158516454));
		key_hash_list.push_back(msghandle::type::fix_int32(1988999625));
		Clear();
	}

void Clear(){
	header={};
	lon={};
	lat={};
	height={};
	heading={};
	velocity={};
	up_velocity={};
	track_angle={};
	week={};
	utc_time={};
	hdop={};
	diff_age={};
	base_length={};
	xg={};
	yg={};
	zg={};
	status={};
	satenum={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,lon,lat,height,heading,velocity,up_velocity,track_angle,week,utc_time,hdop,diff_age,base_length,xg,yg,zg,status,satenum);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","lon","lat","height","heading","velocity","up_velocity","track_angle","week","utc_time","hdop","diff_age","base_length","xg","yg","zg","status","satenum");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,lon,lat,height,heading,velocity,up_velocity,track_angle,week,utc_time,hdop,diff_age,base_length,xg,yg,zg,status,satenum);
} SensorGps;
REGISTER(SensorGps,SensorGps_Func);
