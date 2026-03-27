#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "OriginTopicInfo.h"
/*

*/

typedef struct OriginTopicInfoVec{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<OriginTopicInfo>	datas;	//default_description

	OriginTopicInfoVec(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1526481156));
		Clear();
	}

void Clear(){
	datas={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,datas);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","datas");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,datas);
} OriginTopicInfoVec;
REGISTER(OriginTopicInfoVec,OriginTopicInfoVec_Func);
