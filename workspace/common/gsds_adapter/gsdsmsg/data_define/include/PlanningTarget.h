#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
跟随规划内部目标数据结构
*/

typedef struct PlanningTarget{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头，包含时间戳和坐标系ID
	int	follow_id;	//目标的跟踪ID，用于在连续帧中识别同一目标
	int	type;	//目标类型：0=人, 1=车
	bool	invalid;	//目标是否无效的标志位: true=无效, false=有效
	double	length;	//目标长度 (m)
	double	width;	//目标宽度 (m)
	double	height;	//目标高度 (m)
	double	x;	//自车坐标系下x轴位置 (m)
	double	y;	//自车坐标系下y轴位置 (m)
	double	z;	//自车坐标系下z轴位置 (m)
	double	vx;	//自车坐标系下x轴方向速度 (m/s)
	double	vy;	//自车坐标系下y轴方向速度 (m/s)
	double	angle;	//自车坐标系下航向角
	double	xg;	//绝对坐标系(UTM)下x轴位置 (m)
	double	yg;	//绝对坐标系(UTM)下y轴位置 (m)
	double	utm_z;	//绝对坐标系(UTM)下z轴位置 (m)
	double	utm_vx;	//绝对坐标系(UTM)下x轴方向速度 (m/s)
	double	utm_vy;	//绝对坐标系(UTM)下y轴方向速度 (m/s)
	double	globalangle;	//绝对坐标系(UTM)下航向角
	double	speed;	//绝对速度大小 (m/s)
	double	lat;	//纬度 (degree)
	double	lon;	//经度 (degree)

	PlanningTarget(){
		Clear();
	}

void Clear(){
	key_hash_list.clear(); 
    header = {};
	follow_id={};
	type={};
	invalid={};
	length={};
	width={};
	height={};
	x={};
	y={};
	z={};
	vx={};
	vy={};
	angle={};
	xg={};
	yg={};
	utm_z={};
	utm_vx={};
	utm_vy={};
	globalangle={};
	speed={};
	lat={};
	lon={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,follow_id,type,invalid,length,width,height,x,y,z,vx,vy,angle,xg,yg,utm_z,utm_vx,utm_vy,globalangle,speed,lat,lon);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","follow_id","type","invalid","length","width","height","x","y","z","vx","vy","angle","xg","yg","utm_z","utm_vx","utm_vy","globalangle","speed","lat","lon");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,follow_id,type,invalid,length,width,height,x,y,z,vx,vy,angle,xg,yg,utm_z,utm_vx,utm_vy,globalangle,speed,lat,lon);
} PlanningTarget;
REGISTER(PlanningTarget,PlanningTarget_Func);
