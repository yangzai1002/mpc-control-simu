#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionCameraInstSegResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	width;	//default_description
	int	height;	//default_description
	int	inst_num;	//目标实例个数
	std::vector<int>	inst_class;	//每个实例的类别，一个元素对应一个实例
	std::vector<float>	inst_score;	//实例置信度，限定大于阈值的为正实例样本
	std::vector<unsigned char>	mask;	//当前帧所有实例mask图

	PerceptionCameraInstSegResult(){
		key_hash_list.resize(10);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(431612477));
		key_hash_list.push_back(msghandle::type::fix_int32(1254593184));
		key_hash_list.push_back(msghandle::type::fix_int32(159554538));
		key_hash_list.push_back(msghandle::type::fix_int32(1527280403));
		key_hash_list.push_back(msghandle::type::fix_int32(1961049146));
		key_hash_list.push_back(msghandle::type::fix_int32(1523501639));
		Clear();
	}

void Clear(){
	width={};
	height={};
	inst_num={};
	inst_class={};
	inst_score={};
	mask={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,width,height,inst_num,inst_class,inst_score,mask);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","width","height","inst_num","inst_class","inst_score","mask");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,width,height,inst_num,inst_class,inst_score,mask);
} PerceptionCameraInstSegResult;
REGISTER(PerceptionCameraInstSegResult,PerceptionCameraInstSegResult_Func);
