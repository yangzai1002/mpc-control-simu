#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "BusinessPoint.h"
/*
APP点信息描述
*/

typedef struct BusinessPath{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<BusinessPoint>	points;	//路径点信息
	double	length;	//路径总长度

	BusinessPath(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(522001662));
		key_hash_list.push_back(msghandle::type::fix_int32(1114288885));
		Clear();
	}

void Clear(){
	points={};
	length={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,points,length);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","points","length");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,points,length);
} BusinessPath;
REGISTER(BusinessPath,BusinessPath_Func);
