#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionInstanceMask{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	enable;	//默认值false
	unsigned int	mask_w;	//默认值0
	unsigned int	mask_h;	//默认值0
	std::vector<unsigned char>	instance_mask;	//default_description

	PerceptionInstanceMask(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(7539101));
		key_hash_list.push_back(msghandle::type::fix_int32(197505625));
		key_hash_list.push_back(msghandle::type::fix_int32(1803915668));
		key_hash_list.push_back(msghandle::type::fix_int32(326317535));
		Clear();
	}

void Clear(){
	enable={};
	mask_w={};
	mask_h={};
	instance_mask={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,enable,mask_w,mask_h,instance_mask);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","enable","mask_w","mask_h","instance_mask");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,enable,mask_w,mask_h,instance_mask);
} PerceptionInstanceMask;
REGISTER(PerceptionInstanceMask,PerceptionInstanceMask_Func);
