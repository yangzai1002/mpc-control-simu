#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionLidarVisionObject.h"
/*
目标跟踪数据
*/

typedef struct PerceptionTrackObjData{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//default_description
	std::vector<PerceptionLidarVisionObject>	objs;	//default_description

	PerceptionTrackObjData(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(615898035));
		key_hash_list.push_back(msghandle::type::fix_int32(2081898084));
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
} PerceptionTrackObjData;
REGISTER(PerceptionTrackObjData,PerceptionTrackObjData_Func);
