#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
视频编码参数
*/

typedef struct AppVideoEncodingParameter{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	adaptive_mode;	//自动调整码率和分辨率（0-关闭；1-打开）
	int	rate_control_mode;	//码率控制模式：0-码率；1-质量；
	int	quality;	//编码质量：1-51，1 最好，51 最差
	int	bps;	//输出码率（kbps）
	int	fps;	//输出帧率
	int	encoding_gop;	//编码器GOP
	double	width;	//输出宽度
	double	height;	//输出高度

	AppVideoEncodingParameter(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(764680212));
		key_hash_list.push_back(msghandle::type::fix_int32(1200385037));
		key_hash_list.push_back(msghandle::type::fix_int32(1286760969));
		key_hash_list.push_back(msghandle::type::fix_int32(218505552));
		key_hash_list.push_back(msghandle::type::fix_int32(1246404067));
		key_hash_list.push_back(msghandle::type::fix_int32(1752535426));
		key_hash_list.push_back(msghandle::type::fix_int32(1927245141));
		key_hash_list.push_back(msghandle::type::fix_int32(408176348));
		Clear();
	}

void Clear(){
	adaptive_mode={};
	rate_control_mode={};
	quality={};
	bps={};
	fps={};
	encoding_gop={};
	width={};
	height={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,adaptive_mode,rate_control_mode,quality,bps,fps,encoding_gop,width,height);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","adaptive_mode","rate_control_mode","quality","bps","fps","encoding_gop","width","height");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,adaptive_mode,rate_control_mode,quality,bps,fps,encoding_gop,width,height);
} AppVideoEncodingParameter;
REGISTER(AppVideoEncodingParameter,AppVideoEncodingParameter_Func);
