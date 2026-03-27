#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionLidarVisionObject.h"
#include "PerceptionCloudLabel.h"
/*
点云聚类模块检测结果
*/

typedef struct PerceptionLidarVisionObjectData{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//default_description
	std::vector<PerceptionLidarVisionObject>	trackobjs;	//跟踪上的聚类目标
	std::vector<PerceptionLidarVisionObject>	clusterobjs;	//原始单帧聚类目标
	std::vector<PerceptionLidarVisionObject>	vision_object_2d;	//转发视觉跟踪结果
	std::vector<PerceptionCloudLabel>	labels;	//点云匹配语义分割结果

	PerceptionLidarVisionObjectData(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(615898035));
		key_hash_list.push_back(msghandle::type::fix_int32(723469435));
		key_hash_list.push_back(msghandle::type::fix_int32(1224647875));
		key_hash_list.push_back(msghandle::type::fix_int32(834691345));
		key_hash_list.push_back(msghandle::type::fix_int32(1846265509));
		Clear();
	}

void Clear(){
	header={};
	trackobjs={};
	clusterobjs={};
	vision_object_2d={};
	labels={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list,header,trackobjs,clusterobjs,vision_object_2d,labels);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","trackobjs","clusterobjs","vision_object_2d","labels");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,trackobjs,clusterobjs,vision_object_2d,labels);
} PerceptionLidarVisionObjectData;
REGISTER(PerceptionLidarVisionObjectData,PerceptionLidarVisionObjectData_Func);
