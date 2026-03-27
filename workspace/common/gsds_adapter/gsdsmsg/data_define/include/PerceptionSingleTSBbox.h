#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionSingleTSBbox{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	id;	//检测id
	unsigned short	xmin;	//Bbox左上角X坐标
	unsigned short	ymin;	//Bbox左上角Y坐标
	unsigned short	xmax;	//Bbox右上角X坐标
	unsigned short	ymax;	//Bbox右上角Y坐标
	float	det_score;	//检测置信度
	float	cls_score;	//分类置信度
	bool	is_valid;	//是否有效
	bool	occlude;	//遮挡（未启动）
	unsigned char	signType;	//标识牌细分类类型
	unsigned char	attribute;	//标识牌（只有2个类别，unknown和标识牌）

	PerceptionSingleTSBbox(){
		key_hash_list.resize(15);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1585854299));
		key_hash_list.push_back(msghandle::type::fix_int32(1640891574));
		key_hash_list.push_back(msghandle::type::fix_int32(1363738098));
		key_hash_list.push_back(msghandle::type::fix_int32(30551609));
		key_hash_list.push_back(msghandle::type::fix_int32(1900881781));
		key_hash_list.push_back(msghandle::type::fix_int32(841689574));
		key_hash_list.push_back(msghandle::type::fix_int32(331105634));
		key_hash_list.push_back(msghandle::type::fix_int32(2110835811));
		key_hash_list.push_back(msghandle::type::fix_int32(1398674448));
		key_hash_list.push_back(msghandle::type::fix_int32(694131807));
		key_hash_list.push_back(msghandle::type::fix_int32(11686536));
		Clear();
	}

void Clear(){
	id={};
	xmin={};
	ymin={};
	xmax={};
	ymax={};
	det_score={};
	cls_score={};
	is_valid={};
	occlude={};
	signType={};
	attribute={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,xmin,ymin,xmax,ymax,det_score,cls_score,is_valid,occlude,signType,attribute);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","xmin","ymin","xmax","ymax","det_score","cls_score","is_valid","occlude","signType","attribute");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,xmin,ymin,xmax,ymax,det_score,cls_score,is_valid,occlude,signType,attribute);
} PerceptionSingleTSBbox;
REGISTER(PerceptionSingleTSBbox,PerceptionSingleTSBbox_Func);
