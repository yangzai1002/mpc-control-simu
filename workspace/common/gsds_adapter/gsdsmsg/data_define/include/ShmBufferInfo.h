#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
共享内存buffer相关信息
*/

typedef struct ShmBufferInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	shm_key;	//共享内存的key值
	int	buffer_size;	//单个缓存大小
	int	queue_len;	//队列长度
	int	shm_id;	//共享内存ID
	int	current_index;	//当前数据的索引值

	ShmBufferInfo(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1584003491));
		key_hash_list.push_back(msghandle::type::fix_int32(1314040796));
		key_hash_list.push_back(msghandle::type::fix_int32(949476262));
		key_hash_list.push_back(msghandle::type::fix_int32(120589908));
		key_hash_list.push_back(msghandle::type::fix_int32(2012904600));
		Clear();
	}

void Clear(){
	shm_key={};
	buffer_size={};
	queue_len={};
	shm_id={};
	current_index={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,shm_key,buffer_size,queue_len,shm_id,current_index);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","shm_key","buffer_size","queue_len","shm_id","current_index");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,shm_key,buffer_size,queue_len,shm_id,current_index);
} ShmBufferInfo;
REGISTER(ShmBufferInfo,ShmBufferInfo_Func);
