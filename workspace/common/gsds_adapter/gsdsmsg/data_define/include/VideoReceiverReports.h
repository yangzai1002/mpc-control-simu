#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "VideoReceiverReport.h"
/*

*/

typedef struct VideoReceiverReports{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<VideoReceiverReport>	reports;	//接收端报告

	VideoReceiverReports(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1348872976));
		Clear();
	}

void Clear(){
	reports={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,reports);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","reports");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,reports);
} VideoReceiverReports;
REGISTER(VideoReceiverReports,VideoReceiverReports_Func);
