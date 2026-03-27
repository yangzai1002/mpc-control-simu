#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionLaneline.h"
/*
感知车道线，综合输出结果
*/

typedef struct PerceptionLanelines{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//标准头
	double	trigger_stamp;	//数据触发的基准时间，用于目标的时间同步
	std::vector<PerceptionLaneline>	all_lines;	//车道线数据

	PerceptionLanelines(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(419781243));
		key_hash_list.push_back(msghandle::type::fix_int32(1976690153));
		key_hash_list.push_back(msghandle::type::fix_int32(1982978666));
		Clear();
	}

void Clear(){
	header={};
	trigger_stamp={};
	all_lines={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,trigger_stamp,all_lines);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","trigger_stamp","all_lines");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,trigger_stamp,all_lines);
} PerceptionLanelines;
REGISTER(PerceptionLanelines,PerceptionLanelines_Func);
