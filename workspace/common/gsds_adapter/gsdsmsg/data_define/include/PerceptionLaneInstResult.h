#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionLaneInstResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	valid;	//default_description
	std::vector<int>	camera_indexs;	//default_description
	std::vector<int>	point_counts;	//default_description
	std::vector<float>	points;	//default_description
	int	points_size;	//default_description

	PerceptionLaneInstResult(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1436138044));
		key_hash_list.push_back(msghandle::type::fix_int32(1242867030));
		key_hash_list.push_back(msghandle::type::fix_int32(438137764));
		key_hash_list.push_back(msghandle::type::fix_int32(707025805));
		key_hash_list.push_back(msghandle::type::fix_int32(185683236));
		Clear();
	}

void Clear(){
	valid={};
	camera_indexs={};
	point_counts={};
	points={};
	points_size={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,valid,camera_indexs,point_counts,points,points_size);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","valid","camera_indexs","point_counts","points","points_size");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,valid,camera_indexs,point_counts,points,points_size);
} PerceptionLaneInstResult;
REGISTER(PerceptionLaneInstResult,PerceptionLaneInstResult_Func);
