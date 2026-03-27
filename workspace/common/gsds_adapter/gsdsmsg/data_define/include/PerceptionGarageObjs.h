#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionGarageObject.h"
/*
感知车库检测结果
*/

typedef struct PerceptionGarageObjs{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//default_description
	double	start_stamp;	//开始时间
	double	send_stamp;	//发送时间
	std::vector<PerceptionGarageObject>	objects;	//车库检测结果
	int	reserve;	//default_description

	PerceptionGarageObjs(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(615898035));
		key_hash_list.push_back(msghandle::type::fix_int32(371674192));
		key_hash_list.push_back(msghandle::type::fix_int32(887907917));
		key_hash_list.push_back(msghandle::type::fix_int32(1507057102));
		key_hash_list.push_back(msghandle::type::fix_int32(581429175));
		Clear();
	}

void Clear(){
	header={};
	start_stamp={};
	send_stamp={};
	objects={};
	reserve={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,start_stamp,send_stamp,objects,reserve);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","start_stamp","send_stamp","objects","reserve");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,start_stamp,send_stamp,objects,reserve);
} PerceptionGarageObjs;
REGISTER(PerceptionGarageObjs,PerceptionGarageObjs_Func);
