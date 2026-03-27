#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
BAG包相关状态
*/

typedef struct BagStatus{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	int	status;	//record_status

	BagStatus(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1895535689));
		Clear();
	}

void Clear(){
	header={};
	status={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,status);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","status");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,status);
} BagStatus;
REGISTER(BagStatus,BagStatus_Func);
