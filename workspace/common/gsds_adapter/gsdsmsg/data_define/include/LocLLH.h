#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
定位经纬高信息
*/

typedef struct LocLLH{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	lat;	//纬度[-180,180]
	double	lon;	//经度[-180,180]
	double	height;	//水平高度(m)

	LocLLH(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1578307515));
		key_hash_list.push_back(msghandle::type::fix_int32(1726242845));
		key_hash_list.push_back(msghandle::type::fix_int32(426123230));
		Clear();
	}

void Clear(){
	lat={};
	lon={};
	height={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,lat,lon,height);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","lat","lon","height");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,lat,lon,height);
} LocLLH;
REGISTER(LocLLH,LocLLH_Func);
