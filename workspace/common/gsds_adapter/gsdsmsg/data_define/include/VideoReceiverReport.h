#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct VideoReceiverReport{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	long long	ts;	//报告时间
	double	fps;	//报告帧率
	double	bps;	//报告码率（单位：kbps）
	double	total_latency;	//报告总延时（单位：ms）

	VideoReceiverReport(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1809081445));
		key_hash_list.push_back(msghandle::type::fix_int32(1939691413));
		key_hash_list.push_back(msghandle::type::fix_int32(283673084));
		key_hash_list.push_back(msghandle::type::fix_int32(703162658));
		Clear();
	}

void Clear(){
	ts={};
	fps={};
	bps={};
	total_latency={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,ts,fps,bps,total_latency);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","ts","fps","bps","total_latency");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,ts,fps,bps,total_latency);
} VideoReceiverReport;
REGISTER(VideoReceiverReport,VideoReceiverReport_Func);
