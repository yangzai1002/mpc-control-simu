#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "SensorImage.h"
/*
标准图像数据
*/

typedef struct SensorImageList{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	std::vector<SensorImage>	images;	//像素内容

	SensorImageList(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(316694853));
		Clear();
	}

void Clear(){
	header={};
	images={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,images);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","images");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,images);
} SensorImageList;
REGISTER(SensorImageList,SensorImageList_Func);
