#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
目标轮廓点信息
*/

typedef struct PerceptionOccupyCell{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	idc;	//序号
	double	x;	//vcs坐标
	double	y;	//vcs坐标
	double	xg;	//utm坐标
	double	yg;	//utm坐标

	PerceptionOccupyCell(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(140151744));
		key_hash_list.push_back(msghandle::type::fix_int32(1176063322));
		key_hash_list.push_back(msghandle::type::fix_int32(416796115));
		key_hash_list.push_back(msghandle::type::fix_int32(318069447));
		key_hash_list.push_back(msghandle::type::fix_int32(1785796786));
		Clear();
	}

void Clear(){
	idc={};
	x={};
	y={};
	xg={};
	yg={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,idc,x,y,xg,yg);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","idc","x","y","xg","yg");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,idc,x,y,xg,yg);
} PerceptionOccupyCell;
REGISTER(PerceptionOccupyCell,PerceptionOccupyCell_Func);
