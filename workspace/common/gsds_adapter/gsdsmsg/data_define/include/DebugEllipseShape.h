#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
椭圆参数
*/

typedef struct DebugEllipseShape{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	center_x;	//中心坐标x
	double	center_y;	//中心坐标y
	double	a;	//长轴
	double	b;	//短轴
	double	angle;	//旋转角度

	DebugEllipseShape(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(328730206));
		key_hash_list.push_back(msghandle::type::fix_int32(2094641962));
		key_hash_list.push_back(msghandle::type::fix_int32(1789453170));
		key_hash_list.push_back(msghandle::type::fix_int32(926966279));
		key_hash_list.push_back(msghandle::type::fix_int32(1763680817));
		Clear();
	}

void Clear(){
	center_x={};
	center_y={};
	a={};
	b={};
	angle={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,center_x,center_y,a,b,angle);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","center_x","center_y","a","b","angle");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,center_x,center_y,a,b,angle);
} DebugEllipseShape;
REGISTER(DebugEllipseShape,DebugEllipseShape_Func);
