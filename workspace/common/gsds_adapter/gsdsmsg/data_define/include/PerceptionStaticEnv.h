#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionUtraData.h"
#include "PerceptionSemanticObj.h"
#include "PerceptionTerrainPatch.h"
#include "PerceptionTrafficLight.h"
#include "PerceptionLaneline.h"
/*
感知静态环境信息
*/

typedef struct PerceptionStaticEnv{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	send_stamp;	//节点数据发送时间
	double	start_stamp;	//节点开始数据计算时的时间
	float	cell_size;	//栅格地图中最小栅格的基础尺寸
	std::vector<PerceptionUtraData>	ultra_results;	//超声波检测数据
	std::vector<PerceptionSemanticObj>	semantic_objs;	//语义栅格小目标
	float	terrain_patch_size;	//地势图栅格尺寸，单位米
	std::vector<PerceptionTerrainPatch>	terrain_patches;	//地势地图数据
	std::vector<PerceptionTrafficLight>	traffic_lights;	//红绿灯
	std::vector<PerceptionLaneline>	lane_lines;	//车道线数据
	std::vector<unsigned char>	reserved;	//预留，供客户使用

	PerceptionStaticEnv(){
		key_hash_list.resize(15);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1471036805));
		key_hash_list.push_back(msghandle::type::fix_int32(1299431279));
		key_hash_list.push_back(msghandle::type::fix_int32(1988473390));
		key_hash_list.push_back(msghandle::type::fix_int32(812228571));
		key_hash_list.push_back(msghandle::type::fix_int32(1972913403));
		key_hash_list.push_back(msghandle::type::fix_int32(575068452));
		key_hash_list.push_back(msghandle::type::fix_int32(1944652288));
		key_hash_list.push_back(msghandle::type::fix_int32(1294505246));
		key_hash_list.push_back(msghandle::type::fix_int32(521782959));
		key_hash_list.push_back(msghandle::type::fix_int32(1052623088));
		Clear();
	}

void Clear(){
	header={};
	send_stamp={};
	start_stamp={};
	cell_size={};
	ultra_results={};
	semantic_objs={};
	terrain_patch_size={};
	terrain_patches={};
	traffic_lights={};
	lane_lines={};
	reserved={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,send_stamp,start_stamp,cell_size,ultra_results,semantic_objs,terrain_patch_size,terrain_patches,traffic_lights,lane_lines,reserved);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","send_stamp","start_stamp","cell_size","ultra_results","semantic_objs","terrain_patch_size","terrain_patches","traffic_lights","lane_lines","reserved");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,send_stamp,start_stamp,cell_size,ultra_results,semantic_objs,terrain_patch_size,terrain_patches,traffic_lights,lane_lines,reserved);
} PerceptionStaticEnv;
REGISTER(PerceptionStaticEnv,PerceptionStaticEnv_Func);
