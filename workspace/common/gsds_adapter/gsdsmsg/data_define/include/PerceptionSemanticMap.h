#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionUtraData.h"
#include "PerceptionSemanticObj.h"
#include "PerceptionTerrainPatch.h"
/*
感知融合最终输出的目标topic内容
*/

typedef struct PerceptionSemanticMap{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	trigger_stamp;	//数据内容融合时的基准时间，用于目标的时间同步
	float	cell_size;	//栅格地图中最小栅格的基础尺寸
	std::vector<PerceptionUtraData>	ultra_results;	//超声波检测数据
	std::vector<PerceptionSemanticObj>	semantic_objs;	//语义栅格小目标
	std::vector<PerceptionTerrainPatch>	terrain_patches;	//地势地图数据
	std::vector<unsigned char>	reserved;	//预留，供客户使用

	PerceptionSemanticMap(){
		key_hash_list.resize(11);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1428331554));
		key_hash_list.push_back(msghandle::type::fix_int32(1988473390));
		key_hash_list.push_back(msghandle::type::fix_int32(812228571));
		key_hash_list.push_back(msghandle::type::fix_int32(1972913403));
		key_hash_list.push_back(msghandle::type::fix_int32(1944652288));
		key_hash_list.push_back(msghandle::type::fix_int32(1052623088));
		Clear();
	}

void Clear(){
	header={};
	trigger_stamp={};
	cell_size={};
	ultra_results={};
	semantic_objs={};
	terrain_patches={};
	reserved={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,trigger_stamp,cell_size,ultra_results,semantic_objs,terrain_patches,reserved);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","trigger_stamp","cell_size","ultra_results","semantic_objs","terrain_patches","reserved");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,trigger_stamp,cell_size,ultra_results,semantic_objs,terrain_patches,reserved);
} PerceptionSemanticMap;
REGISTER(PerceptionSemanticMap,PerceptionSemanticMap_Func);
