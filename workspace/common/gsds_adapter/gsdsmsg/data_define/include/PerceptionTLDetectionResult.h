#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionSingleTLDetection.h"
/*

*/

typedef struct PerceptionTLDetectionResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	receive_stamp;	//接收时间戳
	double	send_stamp;	//发送时间
	std::vector<PerceptionSingleTLDetection>	traffic_lights;	//红绿灯详细信息
	bool	contain_lights;	//是否包含红绿灯

	PerceptionTLDetectionResult(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1879357055));
		key_hash_list.push_back(msghandle::type::fix_int32(887907917));
		key_hash_list.push_back(msghandle::type::fix_int32(1968621445));
		key_hash_list.push_back(msghandle::type::fix_int32(513524733));
		Clear();
	}

void Clear(){
	header={};
	receive_stamp={};
	send_stamp={};
	traffic_lights={};
	contain_lights={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,receive_stamp,send_stamp,traffic_lights,contain_lights);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","receive_stamp","send_stamp","traffic_lights","contain_lights");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,receive_stamp,send_stamp,traffic_lights,contain_lights);
} PerceptionTLDetectionResult;
REGISTER(PerceptionTLDetectionResult,PerceptionTLDetectionResult_Func);
