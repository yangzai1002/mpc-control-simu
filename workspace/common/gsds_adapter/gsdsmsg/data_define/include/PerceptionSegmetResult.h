#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionSegmentMaskSplitInfo.h"
/*

*/

typedef struct PerceptionSegmetResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	valid;	//default_description
	bool	mask_compressed;	//default_description
	unsigned int	camera_num;	//default_description
	std::vector<std::string>	camera_names;	//default_description
	std::vector<unsigned int>	camera_heights;	//default_description
	std::vector<unsigned int>	camera_widths;	//default_description
	std::vector<PerceptionSegmentMaskSplitInfo>	mask_split_infos;	//default_description
	std::vector<unsigned char>	mask_data;	//default_description
	unsigned int	data_size;	//default_description

	PerceptionSegmetResult(){
		key_hash_list.resize(13);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1436138044));
		key_hash_list.push_back(msghandle::type::fix_int32(1029968780));
		key_hash_list.push_back(msghandle::type::fix_int32(100358810));
		key_hash_list.push_back(msghandle::type::fix_int32(1759900984));
		key_hash_list.push_back(msghandle::type::fix_int32(938073314));
		key_hash_list.push_back(msghandle::type::fix_int32(1128675015));
		key_hash_list.push_back(msghandle::type::fix_int32(213222727));
		key_hash_list.push_back(msghandle::type::fix_int32(661247258));
		key_hash_list.push_back(msghandle::type::fix_int32(1208167736));
		Clear();
	}

void Clear(){
	valid={};
	mask_compressed={};
	camera_num={};
	camera_names={};
	camera_heights={};
	camera_widths={};
	mask_split_infos={};
	mask_data={};
	data_size={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,valid,mask_compressed,camera_num,camera_names,camera_heights,camera_widths,mask_split_infos,mask_data,data_size);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","valid","mask_compressed","camera_num","camera_names","camera_heights","camera_widths","mask_split_infos","mask_data","data_size");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,valid,mask_compressed,camera_num,camera_names,camera_heights,camera_widths,mask_split_infos,mask_data,data_size);
} PerceptionSegmetResult;
REGISTER(PerceptionSegmetResult,PerceptionSegmetResult_Func);
