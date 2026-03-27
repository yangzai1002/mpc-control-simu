#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
视频分割时的区域点:左上点和右下点
*/

typedef struct AppVideoSplitPoint{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	left_top_point_x;	//左上点x
	int	left_top_point_y;	//左上点y
	int	right_bottom_point_x;	//右下点x
	int	right_bottom_point_y;	//右下点y

	AppVideoSplitPoint(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1242735530));
		key_hash_list.push_back(msghandle::type::fix_int32(562979352));
		key_hash_list.push_back(msghandle::type::fix_int32(426674725));
		key_hash_list.push_back(msghandle::type::fix_int32(1894402195));
		Clear();
	}

void Clear(){
	left_top_point_x={};
	left_top_point_y={};
	right_bottom_point_x={};
	right_bottom_point_y={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,left_top_point_x,left_top_point_y,right_bottom_point_x,right_bottom_point_y);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","left_top_point_x","left_top_point_y","right_bottom_point_x","right_bottom_point_y");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,left_top_point_x,left_top_point_y,right_bottom_point_x,right_bottom_point_y);
} AppVideoSplitPoint;
REGISTER(AppVideoSplitPoint,AppVideoSplitPoint_Func);
