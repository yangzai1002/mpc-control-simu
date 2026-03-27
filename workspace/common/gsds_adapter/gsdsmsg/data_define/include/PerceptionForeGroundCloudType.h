#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "SensorLidarPoint.h"
/*
感知点云地面分割后的数据
*/

typedef struct PerceptionForeGroundCloudType{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	start_stamp;	//节点开始数据计算时的时间
	double	send_stamp;	//节点开始数据计算时的时间
	std::vector<int>	foreground_idx;	//foreground idx
	std::vector<SensorLidarPoint>	cloud;	//cloud data

	PerceptionForeGroundCloudType(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1299431279));
		key_hash_list.push_back(msghandle::type::fix_int32(1754761403));
		key_hash_list.push_back(msghandle::type::fix_int32(1815637496));
		key_hash_list.push_back(msghandle::type::fix_int32(1221099454));
		Clear();
	}

void Clear(){
	header={};
	start_stamp={};
	send_stamp={};
	foreground_idx={};
	cloud={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,start_stamp,send_stamp,foreground_idx,cloud);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","start_stamp","send_stamp","foreground_idx","cloud");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,start_stamp,send_stamp,foreground_idx,cloud);
} PerceptionForeGroundCloudType;
REGISTER(PerceptionForeGroundCloudType,PerceptionForeGroundCloudType_Func);
