#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "string"
/*
时间链条信息
*/

typedef struct CommonTimeChain{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	double	t1;	//起始时间
	double	t2;	//终止时间
	std::string	from_module;	//起始模块
	std::string	to_module;	//终止模块

CommonTimeChain(){
	t1 = 0.0;
	t2 = 0.0;
	from_module = "";
	to_module = "";
	key_hash_list.reserve(9);
	key_hash_list.push_back(msghandle::type::fix_int32(1670264851));
	key_hash_list.push_back(msghandle::type::fix_int32(1670264852));
	key_hash_list.push_back(msghandle::type::fix_int32(1921946134));
	key_hash_list.push_back(msghandle::type::fix_int32(1706971965));
}

void Clear(){
	t1 = 0.0;
	t2 = 0.0;
	from_module = "";
	to_module = "";
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, t1,t2,from_module,to_module);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","t1","t2","from_module","to_module");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, t1,t2,from_module,to_module);
} CommonTimeChain;
REGISTER(CommonTimeChain,CommonTimeChain_Func);
