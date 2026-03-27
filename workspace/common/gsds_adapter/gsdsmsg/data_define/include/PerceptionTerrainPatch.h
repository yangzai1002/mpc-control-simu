#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonPoint3f.h"
/*
地势地图栅格
*/

typedef struct PerceptionTerrainPatch{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonPoint3f	patch_center;	//中心点坐标 xyz
	float	slopx;	//坡度值slopx
	float	slopy;	//坡度值slopy
	unsigned char	type;	//区块类型

	PerceptionTerrainPatch(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1381876890));
		key_hash_list.push_back(msghandle::type::fix_int32(1523032987));
		key_hash_list.push_back(msghandle::type::fix_int32(929222009));
		key_hash_list.push_back(msghandle::type::fix_int32(1043765220));
		Clear();
	}

void Clear(){
	patch_center={};
	slopx={};
	slopy={};
	type={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,patch_center,slopx,slopy,type);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","patch_center","slopx","slopy","type");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,patch_center,slopx,slopy,type);
} PerceptionTerrainPatch;
REGISTER(PerceptionTerrainPatch,PerceptionTerrainPatch_Func);
