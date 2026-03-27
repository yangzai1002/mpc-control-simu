#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "string"
/*

*/

typedef struct DebugSemanticInfo{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	int	pursuit_index;	//
	int	obs_id;	//
	int	obs_type;	//
	double	obs_x;	//
	double	obs_y;	//
	double	obs_z;	//
	double	obs_height;	//
	double	path_x;	//
	double	path_y;	//
	double	path_angle;	//
	double	path_index;	//
	double	path_length;	//
	std::string	collid_count;	//

DebugSemanticInfo(){
	pursuit_index = 0;
	obs_id = 0;
	obs_type = 0;
	obs_x = 0.0;
	obs_y = 0.0;
	obs_z = 0.0;
	obs_height = 0.0;
	path_x = 0.0;
	path_y = 0.0;
	path_angle = 0.0;
	path_index = 0.0;
	path_length = 0.0;
	collid_count = "";
	key_hash_list.reserve(18);
	key_hash_list.push_back(msghandle::type::fix_int32(1275652157));
	key_hash_list.push_back(msghandle::type::fix_int32(653618242));
	key_hash_list.push_back(msghandle::type::fix_int32(460661613));
	key_hash_list.push_back(msghandle::type::fix_int32(795493303));
	key_hash_list.push_back(msghandle::type::fix_int32(795493304));
	key_hash_list.push_back(msghandle::type::fix_int32(795493305));
	key_hash_list.push_back(msghandle::type::fix_int32(146817496));
	key_hash_list.push_back(msghandle::type::fix_int32(763091040));
	key_hash_list.push_back(msghandle::type::fix_int32(763091041));
	key_hash_list.push_back(msghandle::type::fix_int32(2013322747));
	key_hash_list.push_back(msghandle::type::fix_int32(74302438));
	key_hash_list.push_back(msghandle::type::fix_int32(437535918));
	key_hash_list.push_back(msghandle::type::fix_int32(2124168470));
}

void Clear(){
	pursuit_index = 0;
	obs_id = 0;
	obs_type = 0;
	obs_x = 0.0;
	obs_y = 0.0;
	obs_z = 0.0;
	obs_height = 0.0;
	path_x = 0.0;
	path_y = 0.0;
	path_angle = 0.0;
	path_index = 0.0;
	path_length = 0.0;
	collid_count = "";
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, pursuit_index,obs_id,obs_type,obs_x,obs_y,obs_z,obs_height,path_x,path_y,path_angle,path_index,path_length,collid_count);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","pursuit_index","obs_id","obs_type","obs_x","obs_y","obs_z","obs_height","path_x","path_y","path_angle","path_index","path_length","collid_count");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, pursuit_index,obs_id,obs_type,obs_x,obs_y,obs_z,obs_height,path_x,path_y,path_angle,path_index,path_length,collid_count);
} DebugSemanticInfo;
REGISTER(DebugSemanticInfo,DebugSemanticInfo_Func);
