#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonPoint2f.h"
/*

*/

typedef struct PerceptionSingleLaneInst{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<CommonPoint2f>	single_lane_inst;	//单根车道线实例检测点

	PerceptionSingleLaneInst(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(434340268));
		Clear();
	}

void Clear(){
	single_lane_inst={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,single_lane_inst);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","single_lane_inst");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,single_lane_inst);
} PerceptionSingleLaneInst;
REGISTER(PerceptionSingleLaneInst,PerceptionSingleLaneInst_Func);
