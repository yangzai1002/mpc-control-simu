#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonPoint2d.h"
/*
感知车库拟合线信息
*/

typedef struct PerceptionGarageWallLine{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<double>	line_coeffs;	//直线系数
	std::vector<CommonPoint2d>	line_points;	//直线上的点

	PerceptionGarageWallLine(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1783155636));
		key_hash_list.push_back(msghandle::type::fix_int32(1632069200));
		Clear();
	}

void Clear(){
	line_coeffs={};
	line_points={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,line_coeffs,line_points);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","line_coeffs","line_points");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,line_coeffs,line_points);
} PerceptionGarageWallLine;
REGISTER(PerceptionGarageWallLine,PerceptionGarageWallLine_Func);
