#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
仿真用数据结构
*/

typedef struct SimLocData{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	x;	//
	double	y;	//
	double	z;	//
	double	h;	//
	double	he;	//
	double	r;	//
	double	v;	//
	double	acc;	//
	int	road_id;	//
	int	lane_id;	//
	double	s;	//
	double	heading_rate;	//

	SimLocData(){
		key_hash_list.resize(16);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1639929903));
		key_hash_list.push_back(msghandle::type::fix_int32(1564713890));
		key_hash_list.push_back(msghandle::type::fix_int32(1489497877));
		key_hash_list.push_back(msghandle::type::fix_int32(695902463));
		key_hash_list.push_back(msghandle::type::fix_int32(1711992100));
		key_hash_list.push_back(msghandle::type::fix_int32(2091225981));
		key_hash_list.push_back(msghandle::type::fix_int32(1790361929));
		key_hash_list.push_back(msghandle::type::fix_int32(1068225838));
		key_hash_list.push_back(msghandle::type::fix_int32(1402575633));
		key_hash_list.push_back(msghandle::type::fix_int32(1446863513));
		key_hash_list.push_back(msghandle::type::fix_int32(2016009968));
		key_hash_list.push_back(msghandle::type::fix_int32(669718696));
		Clear();
	}

void Clear(){
	x={};
	y={};
	z={};
	h={};
	he={};
	r={};
	v={};
	acc={};
	road_id={};
	lane_id={};
	s={};
	heading_rate={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list,x,y,z,h,he,r,v,acc,road_id,lane_id,s,heading_rate);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","z","h","he","r","v","acc","road_id","lane_id","s","heading_rate");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,z,h,he,r,v,acc,road_id,lane_id,s,heading_rate);
} SimLocData;
REGISTER(SimLocData,SimLocData_Func);
