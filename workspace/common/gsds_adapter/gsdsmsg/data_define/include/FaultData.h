#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "FaultInfo.h"
/*
自主系统故障信息
*/

typedef struct FaultData{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//通信数据
	std::vector<FaultInfo>	fault_info;	//故障信息
	int	fault_level;	//故障等级信息

	FaultData(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1038265994));
		key_hash_list.push_back(msghandle::type::fix_int32(695404162));
		key_hash_list.push_back(msghandle::type::fix_int32(192850296));
		Clear();
	}

void Clear(){
	header={};
	fault_info={};
	fault_level={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,fault_info,fault_level);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","fault_info","fault_level");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,fault_info,fault_level);
} FaultData;
REGISTER(FaultData,FaultData_Func);
