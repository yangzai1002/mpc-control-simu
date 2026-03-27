#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
编码后的视频帧
*/

typedef struct VideoEncodedFrame{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//公用报文头
	int	encoding;	//编码方式：0-h264
	int	key_frame;	//是否关键帧：0-非关键帧；1-关键帧；
	int	frame_size;	//编码后的帧长度
	std::vector<unsigned char>	frame_data;	//编码后的帧数据
	double	fps;	//FPS
	double	pts;	//PTS

	VideoEncodedFrame(){
		key_hash_list.resize(11);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1458464433));
		key_hash_list.push_back(msghandle::type::fix_int32(1585748894));
		key_hash_list.push_back(msghandle::type::fix_int32(2146217084));
		key_hash_list.push_back(msghandle::type::fix_int32(500983313));
		key_hash_list.push_back(msghandle::type::fix_int32(1427043880));
		key_hash_list.push_back(msghandle::type::fix_int32(1770368741));
		key_hash_list.push_back(msghandle::type::fix_int32(1048977725));
		Clear();
	}

void Clear(){
	header={};
	encoding={};
	key_frame={};
	frame_size={};
	frame_data={};
	fps={};
	pts={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,encoding,key_frame,frame_size,frame_data,fps,pts);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","encoding","key_frame","frame_size","frame_data","fps","pts");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,encoding,key_frame,frame_size,frame_data,fps,pts);
} VideoEncodedFrame;
REGISTER(VideoEncodedFrame,VideoEncodedFrame_Func);
