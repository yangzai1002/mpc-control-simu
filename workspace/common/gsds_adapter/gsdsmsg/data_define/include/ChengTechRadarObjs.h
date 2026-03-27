#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "ChengTechRadarObj.h"
/*

*/

typedef struct ChengTechRadarObjs{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//header
	std::vector<ChengTechRadarObj>	objs;	//目标数据

	ChengTechRadarObjs(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(226523786));
		key_hash_list.push_back(msghandle::type::fix_int32(1049276451));
		Clear();
	}

void Clear(){
	header={};
	objs={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,objs);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","objs");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,objs);
} ChengTechRadarObjs;
REGISTER(ChengTechRadarObjs,ChengTechRadarObjs_Func);
