#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct NetDetectionResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	estimated_bandwidth;	//预估带宽（单位：kbps），-1-网络不可达；0-未知

	NetDetectionResult(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1390802863));
		Clear();
	}

void Clear(){
	estimated_bandwidth={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,estimated_bandwidth);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","estimated_bandwidth");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,estimated_bandwidth);
} NetDetectionResult;
REGISTER(NetDetectionResult,NetDetectionResult_Func);
