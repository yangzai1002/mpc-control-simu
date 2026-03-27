#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonPoint2f.h"
/*
感知障碍物对应在图像中的位置信息
*/

typedef struct PerceptionObjImgInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	cam_id;	//相机编号
	std::vector<CommonPoint2f>	rect;	//二维目标框,4个点
	std::vector<CommonPoint2f>	box;	//三维目标框, 8个点

	PerceptionObjImgInfo(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1677573022));
		key_hash_list.push_back(msghandle::type::fix_int32(1628962887));
		key_hash_list.push_back(msghandle::type::fix_int32(559611551));
		Clear();
	}

void Clear(){
	cam_id={};
	rect={};
	box={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,cam_id,rect,box);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","cam_id","rect","box");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,cam_id,rect,box);
} PerceptionObjImgInfo;
REGISTER(PerceptionObjImgInfo,PerceptionObjImgInfo_Func);
