#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct AppPatrolControlParameter{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	command;	//START,PAUSE,RESUME,QUIT
	std::string	file_track;	//default_description
	std::string	file_map;	//default_description
	std::string	file_task;	//default_description
	std::string	dest;	//default_description
	int	type;	//default_description
	bool	is_loop;	//是否循环任务

	AppPatrolControlParameter(){
		key_hash_list.resize(11);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1914199887));
		key_hash_list.push_back(msghandle::type::fix_int32(27632568));
		key_hash_list.push_back(msghandle::type::fix_int32(1214535921));
		key_hash_list.push_back(msghandle::type::fix_int32(1497624680));
		key_hash_list.push_back(msghandle::type::fix_int32(47505230));
		key_hash_list.push_back(msghandle::type::fix_int32(855192431));
		key_hash_list.push_back(msghandle::type::fix_int32(1467475512));
		Clear();
	}

void Clear(){
	command={};
	file_track={};
	file_map={};
	file_task={};
	dest={};
	type={};
	is_loop={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,command,file_track,file_map,file_task,dest,type,is_loop);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","command","file_track","file_map","file_task","dest","type","is_loop");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,command,file_track,file_map,file_task,dest,type,is_loop);
} AppPatrolControlParameter;
REGISTER(AppPatrolControlParameter,AppPatrolControlParameter_Func);
