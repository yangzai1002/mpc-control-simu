#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionObject.h"
/*
感知融合最终输出的目标topic内容
*/

typedef struct PerceptionFinalObjects{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	send_stamp;	//节点数据发送时间
	double	start_stamp;	//节点开始数据计算时的时间
	std::vector<PerceptionObject>	objs;	//感知最终输出的障碍物
	std::vector<unsigned char>	reserved;	//预留，供客户使用

	PerceptionFinalObjects(){
		Clear();
	}

void Clear(){
	key_hash_list.clear();
	header={};
	send_stamp={};
	start_stamp={};
	objs={};
	reserved={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,send_stamp,start_stamp,objs,reserved);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","send_stamp","start_stamp","objs","reserved");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,send_stamp,start_stamp,objs,reserved);
} PerceptionFinalObjects;
REGISTER(PerceptionFinalObjects,PerceptionFinalObjects_Func);
