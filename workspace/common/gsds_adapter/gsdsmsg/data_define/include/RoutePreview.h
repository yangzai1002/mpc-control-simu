#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
规划路线预览状态
*/

typedef struct RoutePreview{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	isPreviewing;	//规划路线预览状态 0:关闭 1:开启
	int	reserved1;	//预留1
	int	reserved2;	//预留2
	int	reserved3;	//预留3

	RoutePreview(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1042315203));
		key_hash_list.push_back(msghandle::type::fix_int32(883277201));
		key_hash_list.push_back(msghandle::type::fix_int32(1967799301));
		key_hash_list.push_back(msghandle::type::fix_int32(904837753));
		Clear();
	}

void Clear(){
	isPreviewing={};
	reserved1={};
	reserved2={};
	reserved3={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,isPreviewing,reserved1,reserved2,reserved3);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","isPreviewing","reserved1","reserved2","reserved3");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,isPreviewing,reserved1,reserved2,reserved3);
} RoutePreview;
REGISTER(RoutePreview,RoutePreview_Func);
