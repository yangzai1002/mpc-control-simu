#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
图像
*/

typedef struct AppImage{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	int	height;	//图像高
	int	width;	//图像宽
	std::string	encoding;	//0 rgb; 1 yuv ; 10:jpeg  11:png  12:h264
	int	step;	//数据单位步长
	std::vector<unsigned char>	data;	//像素内容

	AppImage(){
		key_hash_list.resize(10);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(108245337));
		key_hash_list.push_back(msghandle::type::fix_int32(1102647756));
		key_hash_list.push_back(msghandle::type::fix_int32(1865586042));
		key_hash_list.push_back(msghandle::type::fix_int32(1167492235));
		key_hash_list.push_back(msghandle::type::fix_int32(1607944867));
		Clear();
	}

void Clear(){
	header={};
	height={};
	width={};
	encoding={};
	step={};
	data={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,height,width,encoding,step,data);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","height","width","encoding","step","data");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,height,width,encoding,step,data);
} AppImage;
REGISTER(AppImage,AppImage_Func);
