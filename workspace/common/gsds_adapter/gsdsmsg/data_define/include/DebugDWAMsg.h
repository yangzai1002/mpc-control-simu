#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "string"
/*

*/

typedef struct DebugDWAMsg{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	std::string	DWA_info;	//
	std::string	lat_goals_info;	//
	std::string	lon_goals_info;	//
	float	search_path_size;	//
	float	link_path_size;	//
	float	search_model_halfwheel_left;	//
	float	search_model_halfwheel_right;	//
	bool	if_force_avoid;	//
	bool	if_dwa_success;	//
	bool	if_lateral_goal_reached;	//
	bool	if_longitudinal_goal_reached;	//
	bool	if_link_success;	//
	int	lateral_goals_num;	//
	int	longitudinal_goals_num;	//
	int	refpath_size;	//
	int	dwapath_size;	//
	int	linkpath_size;	//
	int	allpath_size;	//

DebugDWAMsg(){
	DWA_info = "";
	lat_goals_info = "";
	lon_goals_info = "";
	search_path_size = 0.0;
	link_path_size = 0.0;
	search_model_halfwheel_left = 0.0;
	search_model_halfwheel_right = 0.0;
	if_force_avoid = false;
	if_dwa_success = false;
	if_lateral_goal_reached = false;
	if_longitudinal_goal_reached = false;
	if_link_success = false;
	lateral_goals_num = 0;
	longitudinal_goals_num = 0;
	refpath_size = 0;
	dwapath_size = 0;
	linkpath_size = 0;
	allpath_size = 0;
	key_hash_list.reserve(23);
	key_hash_list.push_back(msghandle::type::fix_int32(890704134));
	key_hash_list.push_back(msghandle::type::fix_int32(1642033884));
	key_hash_list.push_back(msghandle::type::fix_int32(841667880));
	key_hash_list.push_back(msghandle::type::fix_int32(2095347963));
	key_hash_list.push_back(msghandle::type::fix_int32(1751264273));
	key_hash_list.push_back(msghandle::type::fix_int32(166574704));
	key_hash_list.push_back(msghandle::type::fix_int32(2122457339));
	key_hash_list.push_back(msghandle::type::fix_int32(738416024));
	key_hash_list.push_back(msghandle::type::fix_int32(352495669));
	key_hash_list.push_back(msghandle::type::fix_int32(938311699));
	key_hash_list.push_back(msghandle::type::fix_int32(308881406));
	key_hash_list.push_back(msghandle::type::fix_int32(2002965907));
	key_hash_list.push_back(msghandle::type::fix_int32(1286007775));
	key_hash_list.push_back(msghandle::type::fix_int32(1042087514));
	key_hash_list.push_back(msghandle::type::fix_int32(375738156));
	key_hash_list.push_back(msghandle::type::fix_int32(2133754213));
	key_hash_list.push_back(msghandle::type::fix_int32(240031293));
	key_hash_list.push_back(msghandle::type::fix_int32(1374372338));
}

void Clear(){
	DWA_info = "";
	lat_goals_info = "";
	lon_goals_info = "";
	search_path_size = 0.0;
	link_path_size = 0.0;
	search_model_halfwheel_left = 0.0;
	search_model_halfwheel_right = 0.0;
	if_force_avoid = false;
	if_dwa_success = false;
	if_lateral_goal_reached = false;
	if_longitudinal_goal_reached = false;
	if_link_success = false;
	lateral_goals_num = 0;
	longitudinal_goals_num = 0;
	refpath_size = 0;
	dwapath_size = 0;
	linkpath_size = 0;
	allpath_size = 0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, DWA_info,lat_goals_info,lon_goals_info,search_path_size,link_path_size,search_model_halfwheel_left,search_model_halfwheel_right,if_force_avoid,if_dwa_success,if_lateral_goal_reached,if_longitudinal_goal_reached,if_link_success,lateral_goals_num,longitudinal_goals_num,refpath_size,dwapath_size,linkpath_size,allpath_size);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","DWA_info","lat_goals_info","lon_goals_info","search_path_size","link_path_size","search_model_halfwheel_left","search_model_halfwheel_right","if_force_avoid","if_dwa_success","if_lateral_goal_reached","if_longitudinal_goal_reached","if_link_success","lateral_goals_num","longitudinal_goals_num","refpath_size","dwapath_size","linkpath_size","allpath_size");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, DWA_info,lat_goals_info,lon_goals_info,search_path_size,link_path_size,search_model_halfwheel_left,search_model_halfwheel_right,if_force_avoid,if_dwa_success,if_lateral_goal_reached,if_longitudinal_goal_reached,if_link_success,lateral_goals_num,longitudinal_goals_num,refpath_size,dwapath_size,linkpath_size,allpath_size);
} DebugDWAMsg;
REGISTER(DebugDWAMsg,DebugDWAMsg_Func);
