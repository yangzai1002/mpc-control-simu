#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "CommonHeader.h"
#include "vector"
#include "ara::adsfi::IdpBusinessPoint.h"
/*
全局路径规划
*/

typedef struct AppGlobalPlannedPath{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	CommonHeader	head;	//
	std::vector<ara::adsfi::IdpBusinessPoint>	path;	//规划得到的路径

AppGlobalPlannedPath(){
	path.clear();
	key_hash_list.reserve(7);
	key_hash_list.push_back(msghandle::type::fix_int32(1668940143));
	key_hash_list.push_back(msghandle::type::fix_int32(1495379140));
}

void Clear(){
	path.clear();
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, head,path);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","head","path");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, head,path);
} AppGlobalPlannedPath;
REGISTER(AppGlobalPlannedPath,AppGlobalPlannedPath_Func);
