#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct TrackPoint{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	lon;	//default_description
	double	lat;	//default_description
	double	heading;	//default_description
	int	index;	//default_description
	double	length;	//default_description
	char	direction;	//default_description

	TrackPoint(){
		key_hash_list.resize(10);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(221598070));
		key_hash_list.push_back(msghandle::type::fix_int32(502176466));
		key_hash_list.push_back(msghandle::type::fix_int32(932548613));
		key_hash_list.push_back(msghandle::type::fix_int32(1857286841));
		key_hash_list.push_back(msghandle::type::fix_int32(330064575));
		key_hash_list.push_back(msghandle::type::fix_int32(1656313563));
		Clear();
	}

void Clear(){
	lon={};
	lat={};
	heading={};
	index={};
	length={};
	direction={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,lon,lat,heading,index,length,direction);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","lon","lat","heading","index","length","direction");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,lon,lat,heading,index,length,direction);
} TrackPoint;
REGISTER(TrackPoint,TrackPoint_Func);
