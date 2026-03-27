#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "string"
/*
故障描述
*/

typedef struct CommonFaultInfo{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	double	t;	//故障发生时间
	std::string	info;	//故障描述
	int	error_code;	//故障码说明

CommonFaultInfo(){
	t = 0.0;
	info = "";
	error_code = 0;
	key_hash_list.reserve(8);
	key_hash_list.push_back(msghandle::type::fix_int32(930758390));
	key_hash_list.push_back(msghandle::type::fix_int32(82282985));
	key_hash_list.push_back(msghandle::type::fix_int32(2092012920));
}

void Clear(){
	t = 0.0;
	info = "";
	error_code = 0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, t,info,error_code);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","t","info","error_code");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, t,info,error_code);
} CommonFaultInfo;
REGISTER(CommonFaultInfo,CommonFaultInfo_Func);
