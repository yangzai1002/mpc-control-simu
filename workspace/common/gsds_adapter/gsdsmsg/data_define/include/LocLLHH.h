#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
接收到的关键点数据
*/

typedef struct LocLLHH{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	lat;	//纬度
	double	lon;	//经度
	double	height;	//高程
	double	heading;	//航向
	int	point_type;	//点类型

	LocLLHH(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1004532820));
		key_hash_list.push_back(msghandle::type::fix_int32(178387686));
		key_hash_list.push_back(msghandle::type::fix_int32(761546894));
		key_hash_list.push_back(msghandle::type::fix_int32(1513239165));
		key_hash_list.push_back(msghandle::type::fix_int32(1569573313));
		Clear();
	}

void Clear(){
	lat={};
	lon={};
	height={};
	heading={};
	point_type={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,lat,lon,height,heading,point_type);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","lat","lon","height","heading","point_type");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,lat,lon,height,heading,point_type);
} LocLLHH;
REGISTER(LocLLHH,LocLLHH_Func);
