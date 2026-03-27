#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "string"
#include "vector"
#include "CommonFaultInfo.h"
#include "CommonTimeChain.h"
/*
数据头信息
*/

typedef struct CommonHeader{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	long long	seq;	//数据序列号
	double	time;	//数据时间，以s为单位
	std::string	module_name;	//模块信息
	std::string	data_name;	//数据信息
	std::string	frame_id;	//数据帧id
	std::vector<CommonFaultInfo>	fault_vec;	//故障诊断相关信息
	std::vector<CommonTimeChain>	time_chain_vec;	//时间链

CommonHeader(){
	seq = 0;
	time = 0.0;
	module_name = "";
	data_name = "";
	frame_id = "";
	fault_vec.clear();
	time_chain_vec.clear();
	key_hash_list.reserve(12);
	key_hash_list.push_back(msghandle::type::fix_int32(323903232));
	key_hash_list.push_back(msghandle::type::fix_int32(951833551));
	key_hash_list.push_back(msghandle::type::fix_int32(665894195));
	key_hash_list.push_back(msghandle::type::fix_int32(1040391881));
	key_hash_list.push_back(msghandle::type::fix_int32(86853304));
	key_hash_list.push_back(msghandle::type::fix_int32(1569688387));
	key_hash_list.push_back(msghandle::type::fix_int32(214472424));
}

void Clear(){
	seq = 0;
	time = 0.0;
	module_name = "";
	data_name = "";
	frame_id = "";
	fault_vec.clear();
	time_chain_vec.clear();
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, seq,time,module_name,data_name,frame_id,fault_vec,time_chain_vec);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","seq","time","module_name","data_name","frame_id","fault_vec","time_chain_vec");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, seq,time,module_name,data_name,frame_id,fault_vec,time_chain_vec);
} CommonHeader;
REGISTER(CommonHeader,CommonHeader_Func);
