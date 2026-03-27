#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct VideoRTPParameter{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	ssrc;	//同步信源标识
	std::string	peer_host;	//RTP推流对端主机名称
	int	peer_port;	//RTP推流对端端口
	int	push_enabled;	//RTP推流启用状态（0-禁用；1-启用）

	VideoRTPParameter(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(641580243));
		key_hash_list.push_back(msghandle::type::fix_int32(1885689848));
		key_hash_list.push_back(msghandle::type::fix_int32(2007368985));
		key_hash_list.push_back(msghandle::type::fix_int32(566532112));
		Clear();
	}

void Clear(){
	ssrc={};
	peer_host={};
	peer_port={};
	push_enabled={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,ssrc,peer_host,peer_port,push_enabled);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","ssrc","peer_host","peer_port","push_enabled");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,ssrc,peer_host,peer_port,push_enabled);
} VideoRTPParameter;
REGISTER(VideoRTPParameter,VideoRTPParameter_Func);
