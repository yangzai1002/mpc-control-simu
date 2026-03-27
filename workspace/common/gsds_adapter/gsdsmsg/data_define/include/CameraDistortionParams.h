#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerCameraInfo.h"
/*
相机内外参数、畸变校正参数
*/

typedef struct CameraDistortionParams{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	receive_stamp;	//接收时间戳
	double	send_stamp;	//发出时间戳
	std::vector<std::string>	camera_names;	//注意:需要和相机驱动中的图片data_name保持一致，否则可能会出现无法和相机数据匹配的情况
	std::vector<PerCameraInfo>	cameras_info;	//具体的相机标定参数词条：注意，必须和上面的 camera_names 中的内容一一对应，否则会读取出错

	CameraDistortionParams(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1879357055));
		key_hash_list.push_back(msghandle::type::fix_int32(1903037816));
		key_hash_list.push_back(msghandle::type::fix_int32(621872530));
		key_hash_list.push_back(msghandle::type::fix_int32(526200274));
		Clear();
	}

void Clear(){
	header={};
	receive_stamp={};
	send_stamp={};
	camera_names={};
	cameras_info={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,receive_stamp,send_stamp,camera_names,cameras_info);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","receive_stamp","send_stamp","camera_names","cameras_info");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,receive_stamp,send_stamp,camera_names,cameras_info);
} CameraDistortionParams;
REGISTER(CameraDistortionParams,CameraDistortionParams_Func);
