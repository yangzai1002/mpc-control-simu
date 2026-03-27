#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CloudFaultInfo.h"
/*
云控当前故障向量
*/

typedef struct CloudFaultInfoVec{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<CloudFaultInfo>	fault_info_vec;	//当前故障

	CloudFaultInfoVec(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(134440644));
		Clear();
	}

void Clear(){
	fault_info_vec={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,fault_info_vec);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","fault_info_vec");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,fault_info_vec);
} CloudFaultInfoVec;
REGISTER(CloudFaultInfoVec,CloudFaultInfoVec_Func);
