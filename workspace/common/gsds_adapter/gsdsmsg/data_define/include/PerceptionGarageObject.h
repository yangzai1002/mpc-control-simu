#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionGarageWallLine.h"
/*
感知检测车库结果
*/

typedef struct PerceptionGarageObject{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	x;	//车库坐标
	double	y;	//车库坐标
	double	length;	//车库长度
	double	width;	//车库宽度
	double	heading;	//车库朝向
	double	x_dr;	//车库utm坐标
	double	y_dr;	//车库utm坐标
	double	heading_dr;	//车库utm朝向
	std::vector<PerceptionGarageWallLine>	vcs_line;	//车库拟合线vcs
	std::vector<PerceptionGarageWallLine>	dr_line;	//车库拟合线dr

	PerceptionGarageObject(){
		key_hash_list.resize(14);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1492453553));
		key_hash_list.push_back(msghandle::type::fix_int32(1325610740));
		key_hash_list.push_back(msghandle::type::fix_int32(1036390728));
		key_hash_list.push_back(msghandle::type::fix_int32(1137610955));
		key_hash_list.push_back(msghandle::type::fix_int32(831087225));
		key_hash_list.push_back(msghandle::type::fix_int32(1330862698));
		key_hash_list.push_back(msghandle::type::fix_int32(1039213109));
		key_hash_list.push_back(msghandle::type::fix_int32(1107533666));
		key_hash_list.push_back(msghandle::type::fix_int32(800364565));
		key_hash_list.push_back(msghandle::type::fix_int32(951926143));
		Clear();
	}

void Clear(){
	x={};
	y={};
	length={};
	width={};
	heading={};
	x_dr={};
	y_dr={};
	heading_dr={};
	vcs_line={};
	dr_line={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,length,width,heading,x_dr,y_dr,heading_dr,vcs_line,dr_line);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","length","width","heading","x_dr","y_dr","heading_dr","vcs_line","dr_line");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,length,width,heading,x_dr,y_dr,heading_dr,vcs_line,dr_line);
} PerceptionGarageObject;
REGISTER(PerceptionGarageObject,PerceptionGarageObject_Func);
