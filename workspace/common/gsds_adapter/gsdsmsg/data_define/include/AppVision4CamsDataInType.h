#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "AppImage.h"
/*

*/

typedef struct AppVision4CamsDataInType{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	image_front_h30_valid;	//default_description
	AppImage	image_front_h30;	//default_description
	bool	image_front_h60_valid;	//default_description
	AppImage	image_front_h60;	//default_description
	bool	image_left_h90_valid;	//default_description
	AppImage	image_left_h90;	//default_description
	bool	image_right_h90_valid;	//default_description
	AppImage	image_right_h90;	//default_description
	bool	image_pano_left_valid;	//default_description
	AppImage	image_pano_left;	//default_description
	bool	image_pano_right_valid;	//default_description
	AppImage	image_pano_right;	//default_description
	bool	image_pano_back_valid;	//default_description
	AppImage	image_pano_back;	//default_description

	AppVision4CamsDataInType(){
		key_hash_list.resize(18);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(350468280));
		key_hash_list.push_back(msghandle::type::fix_int32(1168548887));
		key_hash_list.push_back(msghandle::type::fix_int32(596758969));
		key_hash_list.push_back(msghandle::type::fix_int32(1378425216));
		key_hash_list.push_back(msghandle::type::fix_int32(275631172));
		key_hash_list.push_back(msghandle::type::fix_int32(368848003));
		key_hash_list.push_back(msghandle::type::fix_int32(1704209201));
		key_hash_list.push_back(msghandle::type::fix_int32(1478241208));
		key_hash_list.push_back(msghandle::type::fix_int32(1514196213));
		key_hash_list.push_back(msghandle::type::fix_int32(1583240476));
		key_hash_list.push_back(msghandle::type::fix_int32(1667373156));
		key_hash_list.push_back(msghandle::type::fix_int32(2098173347));
		key_hash_list.push_back(msghandle::type::fix_int32(103087697));
		key_hash_list.push_back(msghandle::type::fix_int32(287654872));
		Clear();
	}

void Clear(){
	image_front_h30_valid={};
	image_front_h30={};
	image_front_h60_valid={};
	image_front_h60={};
	image_left_h90_valid={};
	image_left_h90={};
	image_right_h90_valid={};
	image_right_h90={};
	image_pano_left_valid={};
	image_pano_left={};
	image_pano_right_valid={};
	image_pano_right={};
	image_pano_back_valid={};
	image_pano_back={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,image_front_h30_valid,image_front_h30,image_front_h60_valid,image_front_h60,image_left_h90_valid,image_left_h90,image_right_h90_valid,image_right_h90,image_pano_left_valid,image_pano_left,image_pano_right_valid,image_pano_right,image_pano_back_valid,image_pano_back);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","image_front_h30_valid","image_front_h30","image_front_h60_valid","image_front_h60","image_left_h90_valid","image_left_h90","image_right_h90_valid","image_right_h90","image_pano_left_valid","image_pano_left","image_pano_right_valid","image_pano_right","image_pano_back_valid","image_pano_back");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,image_front_h30_valid,image_front_h30,image_front_h60_valid,image_front_h60,image_left_h90_valid,image_left_h90,image_right_h90_valid,image_right_h90,image_pano_left_valid,image_pano_left,image_pano_right_valid,image_pano_right,image_pano_back_valid,image_pano_back);
} AppVision4CamsDataInType;
REGISTER(AppVision4CamsDataInType,AppVision4CamsDataInType_Func);
