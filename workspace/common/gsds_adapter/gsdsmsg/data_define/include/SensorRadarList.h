#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "SensorRadar.h"
/*
毫米波雷达数据列表
*/

typedef struct SensorRadarList{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//header
	std::vector<SensorRadar>	items;	//雷达数据列表

	SensorRadarList(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(226523786));
		key_hash_list.push_back(msghandle::type::fix_int32(1012651152));
		Clear();
	}

void Clear(){
	header={};
	items={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,items);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","items");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,items);
} SensorRadarList;
REGISTER(SensorRadarList,SensorRadarList_Func);
