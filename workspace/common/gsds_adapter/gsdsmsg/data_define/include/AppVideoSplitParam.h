#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "AppVideoSplitPoint.h"
/*
视频拼接参数:名称+裁剪区域+显示区域
*/

typedef struct AppVideoSplitParam{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	channel_name;	//相机名称
	AppVideoSplitPoint	view_area_points;	//显示区域点信息
	AppVideoSplitPoint	crop_area_points;	//裁剪区域点信息

	AppVideoSplitParam(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1305170989));
		key_hash_list.push_back(msghandle::type::fix_int32(1276210274));
		key_hash_list.push_back(msghandle::type::fix_int32(896517872));
		Clear();
	}

void Clear(){
	channel_name={};
	view_area_points={};
	crop_area_points={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,channel_name,view_area_points,crop_area_points);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","channel_name","view_area_points","crop_area_points");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,channel_name,view_area_points,crop_area_points);
} AppVideoSplitParam;
REGISTER(AppVideoSplitParam,AppVideoSplitParam_Func);
