#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "AppVideoEditorParameter.h"
#include "AppVideoEncodingParameter.h"
/*
视频相关参数
*/

typedef struct AppVideoParameter{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	AppVideoEditorParameter	editor_parameter;	//视频编辑参数
	AppVideoEncodingParameter	encoding_parameter;	//视频编码参数

	AppVideoParameter(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(19595821));
		key_hash_list.push_back(msghandle::type::fix_int32(2073848416));
		Clear();
	}

void Clear(){
	editor_parameter={};
	encoding_parameter={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,editor_parameter,encoding_parameter);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","editor_parameter","encoding_parameter");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,editor_parameter,encoding_parameter);
} AppVideoParameter;
REGISTER(AppVideoParameter,AppVideoParameter_Func);
