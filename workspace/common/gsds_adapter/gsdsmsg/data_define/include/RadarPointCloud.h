#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "RadarPoint4D.h"
/*
华为4D毫米波驱动输出目标
*/

typedef struct RadarPointCloud{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//header
	std::vector<RadarPoint4D>	objs;	//目标数据数组

	RadarPointCloud(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(226523786));
		key_hash_list.push_back(msghandle::type::fix_int32(776232384));
		Clear();
	}

void Clear(){
	header={};
	objs={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,objs);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","objs");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,objs);
} RadarPointCloud;
REGISTER(RadarPointCloud,RadarPointCloud_Func);
