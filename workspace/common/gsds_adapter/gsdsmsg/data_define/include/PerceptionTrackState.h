#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionTrackState{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	enable;	//默认值false
	unsigned int	track_id;	//默认值0
	unsigned int	fusion_id;	//默认值0
	unsigned int	track_age;	//默认值0
	unsigned int	track_time_since_update;	//默认值0
	float	track_score;	//默认值0

	PerceptionTrackState(){
		key_hash_list.resize(10);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(7539101));
		key_hash_list.push_back(msghandle::type::fix_int32(2007658362));
		key_hash_list.push_back(msghandle::type::fix_int32(1935051247));
		key_hash_list.push_back(msghandle::type::fix_int32(131359486));
		key_hash_list.push_back(msghandle::type::fix_int32(358851313));
		key_hash_list.push_back(msghandle::type::fix_int32(535916465));
		Clear();
	}

void Clear(){
	enable={};
	track_id={};
	fusion_id={};
	track_age={};
	track_time_since_update={};
	track_score={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,enable,track_id,fusion_id,track_age,track_time_since_update,track_score);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","enable","track_id","fusion_id","track_age","track_time_since_update","track_score");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,enable,track_id,fusion_id,track_age,track_time_since_update,track_score);
} PerceptionTrackState;
REGISTER(PerceptionTrackState,PerceptionTrackState_Func);
