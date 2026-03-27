#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionTLWomapPerCamera.h"
/*
无图红绿灯输出数据结构类型
*/

typedef struct PerceptionTLWomapResults{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	receive_stamp;	//接收时间戳
	double	send_stamp;	//发出时间戳
	std::vector<PerceptionTLWomapPerCamera>	trafficlight_womap_frames;	//所有相机lights

	PerceptionTLWomapResults(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1879357055));
		key_hash_list.push_back(msghandle::type::fix_int32(1903037816));
		key_hash_list.push_back(msghandle::type::fix_int32(1098731715));
		Clear();
	}

void Clear(){
	header={};
	receive_stamp={};
	send_stamp={};
	trafficlight_womap_frames={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,receive_stamp,send_stamp,trafficlight_womap_frames);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","receive_stamp","send_stamp","trafficlight_womap_frames");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,receive_stamp,send_stamp,trafficlight_womap_frames);
} PerceptionTLWomapResults;
REGISTER(PerceptionTLWomapResults,PerceptionTLWomapResults_Func);
