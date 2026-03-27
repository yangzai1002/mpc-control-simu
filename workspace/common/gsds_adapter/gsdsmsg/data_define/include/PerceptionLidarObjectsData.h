#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionLidarObject.h"
#include "PerceptionLidarSegmentMap.h"
/*
激光目标检测输出的目标topic内容
*/

typedef struct PerceptionLidarObjectsData{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头，header中的时间为发送时间
	double	start_stamp;	//节点开始数据计算时的时间
	double	send_stamp;	//节点数据发送时间
	std::vector<PerceptionLidarObject>	objects;	//激光目标结果
	PerceptionLidarSegmentMap	segment_map;	//可通行区域分割结果

	PerceptionLidarObjectsData(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1805923237));
		key_hash_list.push_back(msghandle::type::fix_int32(1299431279));
		key_hash_list.push_back(msghandle::type::fix_int32(1471036805));
		key_hash_list.push_back(msghandle::type::fix_int32(454540320));
		key_hash_list.push_back(msghandle::type::fix_int32(138983463));
		Clear();
	}

void Clear(){
	header={};
	start_stamp={};
	send_stamp={};
	objects={};
	segment_map={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,start_stamp,send_stamp,objects,segment_map);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","start_stamp","send_stamp","objects","segment_map");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,start_stamp,send_stamp,objects,segment_map);
} PerceptionLidarObjectsData;
REGISTER(PerceptionLidarObjectsData,PerceptionLidarObjectsData_Func);
