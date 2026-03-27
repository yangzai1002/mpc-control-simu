#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
标准压缩图像
*/

typedef struct SensorCompressedImage{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	std::string	encoding;	//0 rgb; 1 yuv ; 10:jpeg  11:png  12:h264
	std::vector<unsigned char>	data;	//像素内容

	SensorCompressedImage(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1865586042));
		key_hash_list.push_back(msghandle::type::fix_int32(1607944867));
		Clear();
	}

void Clear(){
	header={};
	encoding={};
	data={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,encoding,data);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","encoding","data");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,encoding,data);
} SensorCompressedImage;
REGISTER(SensorCompressedImage,SensorCompressedImage_Func);
