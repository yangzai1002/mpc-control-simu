#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionObjPointLabels{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	total_label;	//总有效点个数
	int	out_bounding_label;	//出了图像边界的点个数
	int	background;	//背景点个数
	int	road_curb;	//路沿点个数
	int	fence;	//篱笆点个数
	int	roadblock;	//路障点个数
	int	roaduser_car;	//机动车类别个数
	int	roaduser_other;	//非机动车类别个数

	PerceptionObjPointLabels(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(425574576));
		key_hash_list.push_back(msghandle::type::fix_int32(19328411));
		key_hash_list.push_back(msghandle::type::fix_int32(744038591));
		key_hash_list.push_back(msghandle::type::fix_int32(1231039092));
		key_hash_list.push_back(msghandle::type::fix_int32(2096527405));
		key_hash_list.push_back(msghandle::type::fix_int32(1875564784));
		key_hash_list.push_back(msghandle::type::fix_int32(1927787451));
		key_hash_list.push_back(msghandle::type::fix_int32(180187487));
		Clear();
	}

void Clear(){
	total_label={};
	out_bounding_label={};
	background={};
	road_curb={};
	fence={};
	roadblock={};
	roaduser_car={};
	roaduser_other={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,total_label,out_bounding_label,background,road_curb,fence,roadblock,roaduser_car,roaduser_other);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","total_label","out_bounding_label","background","road_curb","fence","roadblock","roaduser_car","roaduser_other");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,total_label,out_bounding_label,background,road_curb,fence,roadblock,roaduser_car,roaduser_other);
} PerceptionObjPointLabels;
REGISTER(PerceptionObjPointLabels,PerceptionObjPointLabels_Func);
