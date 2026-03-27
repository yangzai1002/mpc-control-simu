#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionCameraResult.h"
/*
一个检测节点的检测结果结构体
*/

typedef struct PerceptionVPResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	receive_stamp;	//接收时间戳
	double	send_stamp;	//发出时间戳
	std::vector<PerceptionCameraResult>	camera_vpresults;	//一个检测节点会检测1个或多个相机

	PerceptionVPResult(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1879357055));
		key_hash_list.push_back(msghandle::type::fix_int32(1903037816));
		key_hash_list.push_back(msghandle::type::fix_int32(1227878833));
		Clear();
	}

void Clear(){
	header={};
	receive_stamp={};
	send_stamp={};
	camera_vpresults={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,receive_stamp,send_stamp,camera_vpresults);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","receive_stamp","send_stamp","camera_vpresults");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,receive_stamp,send_stamp,camera_vpresults);
} PerceptionVPResult;
REGISTER(PerceptionVPResult,PerceptionVPResult_Func);
