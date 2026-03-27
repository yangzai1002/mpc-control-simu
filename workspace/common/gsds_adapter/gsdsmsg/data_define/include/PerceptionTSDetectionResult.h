#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionTSDetectionPerFrame.h"
/*

*/

typedef struct PerceptionTSDetectionResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	receive_stamp;	//接收时间戳
	double	send_stamp;	//发出时间戳
	std::vector<PerceptionTSDetectionPerFrame>	trafficsign_frames;	//所有帧标识牌信息

	PerceptionTSDetectionResult(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1879357055));
		key_hash_list.push_back(msghandle::type::fix_int32(1903037816));
		key_hash_list.push_back(msghandle::type::fix_int32(1822718072));
		Clear();
	}

void Clear(){
	header={};
	receive_stamp={};
	send_stamp={};
	trafficsign_frames={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,receive_stamp,send_stamp,trafficsign_frames);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","receive_stamp","send_stamp","trafficsign_frames");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,receive_stamp,send_stamp,trafficsign_frames);
} PerceptionTSDetectionResult;
REGISTER(PerceptionTSDetectionResult,PerceptionTSDetectionResult_Func);
