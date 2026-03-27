#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionSegmentMaskSplitInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	unsigned int	camera_id;	//default_description
	std::vector<unsigned int>	camera_roi;	//default_description
	std::vector<unsigned int>	mask_roi;	//default_description
	unsigned int	mask_type;	//default_description

	PerceptionSegmentMaskSplitInfo(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1425339427));
		key_hash_list.push_back(msghandle::type::fix_int32(981586438));
		key_hash_list.push_back(msghandle::type::fix_int32(493714469));
		key_hash_list.push_back(msghandle::type::fix_int32(1576260403));
		Clear();
	}

void Clear(){
	camera_id={};
	camera_roi={};
	mask_roi={};
	mask_type={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,camera_id,camera_roi,mask_roi,mask_type);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","camera_id","camera_roi","mask_roi","mask_type");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,camera_id,camera_roi,mask_roi,mask_type);
} PerceptionSegmentMaskSplitInfo;
REGISTER(PerceptionSegmentMaskSplitInfo,PerceptionSegmentMaskSplitInfo_Func);
