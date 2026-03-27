#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
感知车道线匹配点
*/

typedef struct PerceptionLaneLinePoint{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	x;	//相对坐标x
	float	y;	//相对坐标y
	double	xdr;	//绝对坐标xdr
	double	ydr;	//绝对坐标ydr
	float	u;	//图像坐标u
	float	v;	//图像坐标v
	int	point_type;	//点类型 1:UNKNOWN,2:DIVISION,3:MERGE？

	PerceptionLaneLinePoint(){
		key_hash_list.resize(11);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1262543554));
		key_hash_list.push_back(msghandle::type::fix_int32(880971662));
		key_hash_list.push_back(msghandle::type::fix_int32(347304116));
		key_hash_list.push_back(msghandle::type::fix_int32(1347299176));
		key_hash_list.push_back(msghandle::type::fix_int32(438128428));
		key_hash_list.push_back(msghandle::type::fix_int32(56556536));
		key_hash_list.push_back(msghandle::type::fix_int32(1206027284));
		Clear();
	}

void Clear(){
	x={};
	y={};
	xdr={};
	ydr={};
	u={};
	v={};
	point_type={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,xdr,ydr,u,v,point_type);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","xdr","ydr","u","v","point_type");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,xdr,ydr,u,v,point_type);
} PerceptionLaneLinePoint;
REGISTER(PerceptionLaneLinePoint,PerceptionLaneLinePoint_Func);
