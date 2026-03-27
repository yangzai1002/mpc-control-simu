#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
抽象按键状态
*/

typedef struct VehicleKeyBrife{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	key_name;	//按键名称
	unsigned char	index;	//byte的索引值
	unsigned char	start_bit;	//bit的开始
	unsigned char	bit_num;	//bit位长度

	VehicleKeyBrife(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(98950561));
		key_hash_list.push_back(msghandle::type::fix_int32(979812080));
		key_hash_list.push_back(msghandle::type::fix_int32(1832817705));
		key_hash_list.push_back(msghandle::type::fix_int32(1637803456));
		Clear();
	}

void Clear(){
	key_name={};
	index={};
	start_bit={};
	bit_num={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,key_name,index,start_bit,bit_num);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","key_name","index","start_bit","bit_num");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,key_name,index,start_bit,bit_num);
} VehicleKeyBrife;
REGISTER(VehicleKeyBrife,VehicleKeyBrife_Func);
