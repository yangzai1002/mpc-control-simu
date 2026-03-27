#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "string"
#include "vector"
/*

*/

typedef struct DebugASTARMsg{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	std::string	goalinfo;	//
	std::string	startinfo;	//
	std::string	type;	//
	std::string	MTR_info;	//
	std::string	allowance_info;	//
	std::string	stayinfo;	//
	bool	goalvalid;	//
	bool	startvalid;	//
	std::vector<int>	loop;	//
	int	RS_size;	//
	int	SEHS_size;	//
	int	allpath_size;	//

DebugASTARMsg(){
	goalinfo = "";
	startinfo = "";
	type = "";
	MTR_info = "";
	allowance_info = "";
	stayinfo = "";
	goalvalid = false;
	startvalid = false;
	loop.clear();
	RS_size = 0;
	SEHS_size = 0;
	allpath_size = 0;
	key_hash_list.reserve(17);
	key_hash_list.push_back(msghandle::type::fix_int32(1299881820));
	key_hash_list.push_back(msghandle::type::fix_int32(1293072921));
	key_hash_list.push_back(msghandle::type::fix_int32(107202057));
	key_hash_list.push_back(msghandle::type::fix_int32(2103181441));
	key_hash_list.push_back(msghandle::type::fix_int32(355208804));
	key_hash_list.push_back(msghandle::type::fix_int32(1374612794));
	key_hash_list.push_back(msghandle::type::fix_int32(715149288));
	key_hash_list.push_back(msghandle::type::fix_int32(1874676667));
	key_hash_list.push_back(msghandle::type::fix_int32(1098930042));
	key_hash_list.push_back(msghandle::type::fix_int32(1580609507));
	key_hash_list.push_back(msghandle::type::fix_int32(853768883));
	key_hash_list.push_back(msghandle::type::fix_int32(1374372338));
}

void Clear(){
	goalinfo = "";
	startinfo = "";
	type = "";
	MTR_info = "";
	allowance_info = "";
	stayinfo = "";
	goalvalid = false;
	startvalid = false;
	loop.clear();
	RS_size = 0;
	SEHS_size = 0;
	allpath_size = 0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, goalinfo,startinfo,type,MTR_info,allowance_info,stayinfo,goalvalid,startvalid,loop,RS_size,SEHS_size,allpath_size);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","goalinfo","startinfo","type","MTR_info","allowance_info","stayinfo","goalvalid","startvalid","loop","RS_size","SEHS_size","allpath_size");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, goalinfo,startinfo,type,MTR_info,allowance_info,stayinfo,goalvalid,startvalid,loop,RS_size,SEHS_size,allpath_size);
} DebugASTARMsg;
REGISTER(DebugASTARMsg,DebugASTARMsg_Func);
