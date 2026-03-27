#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "SensorCompressedImage.h"
/*
压缩图像数据数
*/

typedef struct SensorCompressedImageList{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	std::vector<SensorCompressedImage>	images;	//压缩图像内容

	SensorCompressedImageList(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1722555337));
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
} SensorCompressedImageList;
REGISTER(SensorCompressedImageList,SensorCompressedImageList_Func);
