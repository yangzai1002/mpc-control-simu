#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionPairObject2D{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	unsigned int	obj_id;	//默认值0
	float	prob;	//默认值0
	float	x;	//默认值0
	float	y;	//默认值0
	float	w;	//默认值0
	float	h;	//默认值0
	float	offset_x;	//默认值0
	float	offset_y;	//默认值0

	PerceptionPairObject2D(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(540718394));
		key_hash_list.push_back(msghandle::type::fix_int32(110254378));
		key_hash_list.push_back(msghandle::type::fix_int32(122083315));
		key_hash_list.push_back(msghandle::type::fix_int32(1589810654));
		key_hash_list.push_back(msghandle::type::fix_int32(801839624));
		key_hash_list.push_back(msghandle::type::fix_int32(260766019));
		key_hash_list.push_back(msghandle::type::fix_int32(1810986847));
		key_hash_list.push_back(msghandle::type::fix_int32(1131230538));
		Clear();
	}

void Clear(){
	obj_id={};
	prob={};
	x={};
	y={};
	w={};
	h={};
	offset_x={};
	offset_y={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,obj_id,prob,x,y,w,h,offset_x,offset_y);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","obj_id","prob","x","y","w","h","offset_x","offset_y");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,obj_id,prob,x,y,w,h,offset_x,offset_y);
} PerceptionPairObject2D;
REGISTER(PerceptionPairObject2D,PerceptionPairObject2D_Func);
