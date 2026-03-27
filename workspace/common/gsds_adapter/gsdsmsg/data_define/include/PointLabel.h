#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonPoint3f.h"
/*
点云和语义分割匹配结构体
*/

typedef struct PointLabel{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<CommonPoint3f>	point_vcs;	//vcs坐标
	std::vector<CommonPoint3f>	point_img;	//图像坐标
	int	label;	//类别
	int	cam_idx;	//相机id

	PointLabel(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(31045132));
		key_hash_list.push_back(msghandle::type::fix_int32(950741626));
		key_hash_list.push_back(msghandle::type::fix_int32(1004481956));
		key_hash_list.push_back(msghandle::type::fix_int32(162517595));
		Clear();
	}

void Clear(){
	point_vcs={};
	point_img={};
	label={};
	cam_idx={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,point_vcs,point_img,label,cam_idx);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","point_vcs","point_img","label","cam_idx");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,point_vcs,point_img,label,cam_idx);
} PointLabel;
REGISTER(PointLabel,PointLabel_Func);
