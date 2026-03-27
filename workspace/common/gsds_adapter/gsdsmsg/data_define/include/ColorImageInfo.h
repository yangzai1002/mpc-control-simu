#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct ColorImageInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	width;	//default_description
	int	height;	//default_description
	int	color_width;	//default_description

	ColorImageInfo(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(431612477));
		key_hash_list.push_back(msghandle::type::fix_int32(1254593184));
		key_hash_list.push_back(msghandle::type::fix_int32(1085094653));
		Clear();
	}

void Clear(){
	width={};
	height={};
	color_width={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,width,height,color_width);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","width","height","color_width");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,width,height,color_width);
} ColorImageInfo;
REGISTER(ColorImageInfo,ColorImageInfo_Func);
