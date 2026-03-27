#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
云台控制结构
*/

typedef struct PTZControl{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	target;	//控制目标 
	int	type;	//值类型
	float	value;	//控制目标值
	float	speed;	//转角速度
	bool	isReset;	//是否复位
	float	rotate_speed;	//转圈速度
	int	wiper_target;	//0/雨刷 1/镜头加热
	bool	isopen;	//是否打开

	PTZControl(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(805786211));
		key_hash_list.push_back(msghandle::type::fix_int32(1207037631));
		key_hash_list.push_back(msghandle::type::fix_int32(2051841617));
		key_hash_list.push_back(msghandle::type::fix_int32(196886274));
		key_hash_list.push_back(msghandle::type::fix_int32(1789934361));
		key_hash_list.push_back(msghandle::type::fix_int32(1005542980));
		key_hash_list.push_back(msghandle::type::fix_int32(752458032));
		key_hash_list.push_back(msghandle::type::fix_int32(1435819995));
		Clear();
	}

void Clear(){
	target={};
	type={};
	value={};
	speed={};
	isReset={};
	rotate_speed={};
	wiper_target={};
	isopen={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,target,type,value,speed,isReset,rotate_speed,wiper_target,isopen);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","target","type","value","speed","isReset","rotate_speed","wiper_target","isopen");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,target,type,value,speed,isReset,rotate_speed,wiper_target,isopen);
} PTZControl;
REGISTER(PTZControl,PTZControl_Func);
