#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
路径点信息描述
*/

typedef struct BusinessPoint{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	index;	//绝对地图中的index
	double	x;	//x坐标
	double	y;	//y坐标
	double	xg;	//xg坐标
	double	yg;	//yg坐标
	double	angle;	//相对角度
	double	heading;	//绝对坐标
	int	direction;	//前进方向（0-前进；1-倒车）

	BusinessPoint(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(8686274));
		key_hash_list.push_back(msghandle::type::fix_int32(571140852));
		key_hash_list.push_back(msghandle::type::fix_int32(1511336176));
		key_hash_list.push_back(msghandle::type::fix_int32(20336554));
		key_hash_list.push_back(msghandle::type::fix_int32(1829115332));
		key_hash_list.push_back(msghandle::type::fix_int32(1845088859));
		key_hash_list.push_back(msghandle::type::fix_int32(503111906));
		key_hash_list.push_back(msghandle::type::fix_int32(859255609));
		Clear();
	}

void Clear(){
	index={};
	x={};
	y={};
	xg={};
	yg={};
	angle={};
	heading={};
	direction={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list,index,x,y,xg,yg,angle,heading,direction);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","index","x","y","xg","yg","angle","heading","direction");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,index,x,y,xg,yg,angle,heading,direction);
} BusinessPoint;
REGISTER(BusinessPoint,BusinessPoint_Func);
