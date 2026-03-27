#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonPoint3f.h"
#include "CommonPoint2f.h"
/*

*/

typedef struct PerceptionCloudLabel{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonPoint3f	point_vcs;	//vcs坐标
	CommonPoint2f	point_img;	//图像坐标
	int	label;	//类别
	int	cam_idx;	//相机序号

	PerceptionCloudLabel(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(266565586));
		key_hash_list.push_back(msghandle::type::fix_int32(1813172779));
		key_hash_list.push_back(msghandle::type::fix_int32(1004481956));
		key_hash_list.push_back(msghandle::type::fix_int32(1384662523));
		Clear();
	}

void Clear(){
	point_vcs={};
	point_img={};
	label={};
	cam_idx={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list,point_vcs,point_img,label,cam_idx);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","point_vcs","point_img","label","cam_idx");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,point_vcs,point_img,label,cam_idx);
} PerceptionCloudLabel;
REGISTER(PerceptionCloudLabel,PerceptionCloudLabel_Func);
