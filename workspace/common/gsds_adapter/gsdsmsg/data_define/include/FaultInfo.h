#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
故障信息
*/

typedef struct FaultInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	code;	//故障代码
	int	handle;	//故障处置
	std::string	desc;	//故障描述
	std::string	extra_desc;	//额外描述信息
	double	timestamp;	//故障发生时间
	std::string	from;	//故障模块

	FaultInfo(){
		key_hash_list.resize(10);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(453397344));
		key_hash_list.push_back(msghandle::type::fix_int32(233407165));
		key_hash_list.push_back(msghandle::type::fix_int32(1859786877));
		key_hash_list.push_back(msghandle::type::fix_int32(958044402));
		key_hash_list.push_back(msghandle::type::fix_int32(1993848264));
		key_hash_list.push_back(msghandle::type::fix_int32(746649047));
		Clear();
	}

void Clear(){
	code={};
	handle={};
	desc={};
	extra_desc={};
	timestamp={};
	from={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,code,handle,desc,extra_desc,timestamp,from);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","code","handle","desc","extra_desc","timestamp","from");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,code,handle,desc,extra_desc,timestamp,from);
} FaultInfo;
REGISTER(FaultInfo,FaultInfo_Func);
