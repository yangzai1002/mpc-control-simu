#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*

*/

typedef struct BusinessTaskPoint{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//default_description
	std::string	name;	//default_description
	double	high;	//default_description
	double	pitch;	//default_description
	double	yaw;	//default_description
	int	zoom;	//default_description
	double	lat;	//default_description
	double	lon;	//default_description
	double	dir;	//default_description
	int	task_time;	//default_description
	int	type;	//default_description
	bool	valid;	//default_description

	BusinessTaskPoint(){
		key_hash_list.resize(16);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(615898035));
		key_hash_list.push_back(msghandle::type::fix_int32(1813554167));
		key_hash_list.push_back(msghandle::type::fix_int32(2057097079));
		key_hash_list.push_back(msghandle::type::fix_int32(1008798207));
		key_hash_list.push_back(msghandle::type::fix_int32(12888906));
		key_hash_list.push_back(msghandle::type::fix_int32(1490150812));
		key_hash_list.push_back(msghandle::type::fix_int32(502176466));
		key_hash_list.push_back(msghandle::type::fix_int32(221598070));
		key_hash_list.push_back(msghandle::type::fix_int32(578101984));
		key_hash_list.push_back(msghandle::type::fix_int32(742987902));
		key_hash_list.push_back(msghandle::type::fix_int32(855192431));
		key_hash_list.push_back(msghandle::type::fix_int32(1436138044));
		Clear();
	}

void Clear(){
	header={};
	name={};
	high={};
	pitch={};
	yaw={};
	zoom={};
	lat={};
	lon={};
	dir={};
	task_time={};
	type={};
	valid={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,name,high,pitch,yaw,zoom,lat,lon,dir,task_time,type,valid);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","name","high","pitch","yaw","zoom","lat","lon","dir","task_time","type","valid");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,name,high,pitch,yaw,zoom,lat,lon,dir,task_time,type,valid);
} BusinessTaskPoint;
REGISTER(BusinessTaskPoint,BusinessTaskPoint_Func);
