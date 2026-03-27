#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
原始topic数据
*/

typedef struct OriginTopicInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	topic_name;	//topic name
	std::string	message_type;	//message type
	std::string	content;	//message content

	OriginTopicInfo(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(239261415));
		key_hash_list.push_back(msghandle::type::fix_int32(2136948377));
		key_hash_list.push_back(msghandle::type::fix_int32(692989891));
		Clear();
	}

void Clear(){
	topic_name={};
	message_type={};
	content={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,topic_name,message_type,content);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","topic_name","message_type","content");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,topic_name,message_type,content);
} OriginTopicInfo;
REGISTER(OriginTopicInfo,OriginTopicInfo_Func);
