#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
云控专用故障数据结构
*/

typedef struct CloudFaultInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	fault_time;	//故障时间
	std::string	fault_code;	//故障码
	std::string	message;	//故障消息
	std::string	node_name;	//节点名称
	int	fault_level;	//故障等级

	CloudFaultInfo(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(782030618));
		key_hash_list.push_back(msghandle::type::fix_int32(979176573));
		key_hash_list.push_back(msghandle::type::fix_int32(911126930));
		key_hash_list.push_back(msghandle::type::fix_int32(335179354));
		key_hash_list.push_back(msghandle::type::fix_int32(664549950));
		Clear();
	}

void Clear(){
	fault_time={};
	fault_code={};
	message={};
	node_name={};
	fault_level={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,fault_time,fault_code,message,node_name,fault_level);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","fault_time","fault_code","message","node_name","fault_level");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,fault_time,fault_code,message,node_name,fault_level);
} CloudFaultInfo;
REGISTER(CloudFaultInfo,CloudFaultInfo_Func);
