#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
平板遥控类型
*/

typedef struct AppSoftControl{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	low_beam_ctrl;	//远光灯、近光灯
	float	max_speed;	//最大速度km/h
	int	other_ctrl;	//其他控制，比如音频设备、拾音器设备等，根据产品进行扩充
	int	record_return_path;	//0-不记录返航路径；1-记录返航路径

	AppSoftControl(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(747426090));
		key_hash_list.push_back(msghandle::type::fix_int32(1533455956));
		key_hash_list.push_back(msghandle::type::fix_int32(1680487682));
		key_hash_list.push_back(msghandle::type::fix_int32(1539778417));
		Clear();
	}

void Clear(){
	low_beam_ctrl={};
	max_speed={};
	other_ctrl={};
	record_return_path={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,low_beam_ctrl,max_speed,other_ctrl,record_return_path);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","low_beam_ctrl","max_speed","other_ctrl","record_return_path");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,low_beam_ctrl,max_speed,other_ctrl,record_return_path);
} AppSoftControl;
REGISTER(AppSoftControl,AppSoftControl_Func);
