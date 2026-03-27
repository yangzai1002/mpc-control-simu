#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CurvePoint.h"
/*
曲线
*/

typedef struct Curve{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<CurvePoint>	curve;	//曲线

	Curve(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(919293297));
		Clear();
	}

void Clear(){
	curve={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,curve);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","curve");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,curve);
} Curve;
REGISTER(Curve,Curve_Func);
