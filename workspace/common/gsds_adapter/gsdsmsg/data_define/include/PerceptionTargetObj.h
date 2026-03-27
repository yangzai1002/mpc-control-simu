#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
感知跟随目标数据
*/

typedef struct PerceptionTargetObj{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	x;	//车辆坐标系下x
	double	y;	//车辆坐标系下y
	double	z;	//车辆坐标系下y
	double	vx;	//自车坐标系下x轴方向速度
	double	vy;	//自车坐标系下y轴方向速度
	double	yaw;	//自车坐标系下yaw
	double	lat;	//纬度
	double	lon;	//经度
	double	speed;	//绝对坐标系速度
	bool	invalid;	//目标是否有效：true无效，false有效
	int	type;	//目标类型：0人1车
	double	utm_x;	//绝对坐标系x
	double	utm_y;	//绝对坐标系y
	double	utm_z;	//绝对坐标系z
	double	utm_vx;	//绝对坐标系下x轴方向速度
	double	utm_vy;	//绝对坐标系下y轴方向速度
	double	utm_yaw;	//绝对坐标系下目标yaw角
	int	trk_id;	//跟踪id
	double	width;	//目标宽
	double	length;	//目标长
	double	height;	//目标高

	PerceptionTargetObj(){
		key_hash_list.reserve(26);
		Clear();
	}

void Clear(){
	key_hash_list.clear();
	header={};
	x={};
	y={};
	z={};
	vx={};
	vy={};
	yaw={};
	lat={};
	lon={};
	speed={};
	invalid={};
	type={};
	utm_x={};
	utm_y={};
	utm_z={};
	utm_vx={};
	utm_vy={};
	utm_yaw={};
	trk_id={};
	width={};
	length={};
	height={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,x,y,z,vx,vy,yaw,lat,lon,speed,invalid,type,utm_x,utm_y,utm_z,utm_vx,utm_vy,utm_yaw,trk_id,width,length,height);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","x","y","z","vx","vy","yaw","lat","lon","speed","invalid","type","utm_x","utm_y","utm_z","utm_vx","utm_vy","utm_yaw","trk_id","width","length","height");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,x,y,z,vx,vy,yaw,lat,lon,speed,invalid,type,utm_x,utm_y,utm_z,utm_vx,utm_vy,utm_yaw,trk_id,width,length,height);
} PerceptionTargetObj;
REGISTER(PerceptionTargetObj,PerceptionTargetObj_Func);
