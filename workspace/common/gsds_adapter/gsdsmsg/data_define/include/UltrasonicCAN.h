#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
超声波can协议说明
*/

typedef struct UltrasonicCAN{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	unsigned short	Lin0_USS4_ToF_Direct;	//Lin0通道_318探头4_直接探测时间
	unsigned short	Lin0_USS3_ToF_Direct;	//Lin0通道_318探头3_直接探测时间
	unsigned short	Lin0_USS2_ToF_Direct;	//Lin0通道_318探头2_直接探测时间
	unsigned short	Lin0_USS1_ToF_Direct;	//Lin0通道_318探头1_直接探测时间
	unsigned short	Lin0_USS4_ToF_Indirect;	//Lin0通道_318探头4_间接探测时间
	unsigned short	Lin0_USS3_ToF_Indirect;	//Lin0通道_318探头3_间接探测时间
	unsigned short	Lin0_USS2_ToF_Indirect;	//Lin0通道_318探头2_间接探测时间
	unsigned short	Lin0_USS1_ToF_Indirect;	//Lin0通道_318探头1_间接探测时间
	unsigned short	Lin1_USS4_ToF_Direct;	//Lin1通道_318探头4_直接探测时间
	unsigned short	Lin1_USS3_ToF_Direct;	//Lin1通道_318探头3_直接探测时间
	unsigned short	Lin1_USS2_ToF_Direct;	//Lin1通道_318探头2_直接探测时间
	unsigned short	Lin1_USS1_ToF_Direct;	//Lin1通道_318探头1_直接探测时间
	unsigned short	Lin1_USS4_ToF_Indirect;	//Lin1通道_318探头4_间接探测时间
	unsigned short	Lin1_USS3_ToF_Indirect;	//Lin1通道_318探头3_间接探测时间
	unsigned short	Lin1_USS2_ToF_Indirect;	//Lin1通道_318探头2_间接探测时间
	unsigned short	Lin1_USS1_ToF_Indirect;	//Lin1通道_318探头1_间接探测时间
	unsigned short	Lin2_USS1_ToF_Direct;	//Lin2通道_313探头1_直接探测时间
	unsigned short	Lin2_USS4_ToF_Direct;	//Lin2通道_313探头4_直接探测时间
	unsigned short	Lin2_USS2_ToF_Direct;	//Lin2通道_313探头2_直接探测时间
	unsigned short	Lin2_USS3_ToF_Direct;	//Lin2通道_313探头3_直接探测时间
	unsigned char	Lin2_USS4_Status;	//Lin2通道_313探头4_状态
	unsigned char	Lin2_USS3_Status;	//Lin2通道_313探头3_状态
	unsigned char	Lin1_USS4_Status;	//Lin1通道_318探头4_状态
	unsigned char	Lin1_USS3_Status;	//Lin1通道_318探头3_状态
	unsigned char	Lin0_USS4_Status;	//Lin0通道_318探头4_状态
	unsigned char	Lin0_USS3_Status;	//Lin0通道_318探头3_状态
	unsigned char	Lin0_USS2_Status;	//Lin0通道_318探头2_状态
	unsigned char	Lin0_USS1_Status;	//Lin0通道_318探头1_状态
	unsigned char	Lin1_USS2_Status;	//Lin1通道_318探头2_通信状态
	unsigned char	Lin1_USS1_Status;	//Lin0通道_318探头3_通信状态
	unsigned char	Lin2_USS2_Status;	//Lin0通道_318探头2_通信状态
	unsigned char	Lin2_USS1_Status;	//Lin0通道_318探头1_通信状态

	UltrasonicCAN(){
		key_hash_list.resize(36);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2022894376));
		key_hash_list.push_back(msghandle::type::fix_int32(911992374));
		key_hash_list.push_back(msghandle::type::fix_int32(1948574020));
		key_hash_list.push_back(msghandle::type::fix_int32(837672018));
		key_hash_list.push_back(msghandle::type::fix_int32(1800553289));
		key_hash_list.push_back(msghandle::type::fix_int32(153747535));
		key_hash_list.push_back(msghandle::type::fix_int32(654425429));
		key_hash_list.push_back(msghandle::type::fix_int32(1155103323));
		key_hash_list.push_back(msghandle::type::fix_int32(1959907758));
		key_hash_list.push_back(msghandle::type::fix_int32(849005756));
		key_hash_list.push_back(msghandle::type::fix_int32(1885587402));
		key_hash_list.push_back(msghandle::type::fix_int32(774685400));
		key_hash_list.push_back(msghandle::type::fix_int32(1111252839));
		key_hash_list.push_back(msghandle::type::fix_int32(1611930733));
		key_hash_list.push_back(msghandle::type::fix_int32(2112608627));
		key_hash_list.push_back(msghandle::type::fix_int32(465802873));
		key_hash_list.push_back(msghandle::type::fix_int32(831519543));
		key_hash_list.push_back(msghandle::type::fix_int32(2016741901));
		key_hash_list.push_back(msghandle::type::fix_int32(1942421545));
		key_hash_list.push_back(msghandle::type::fix_int32(905839899));
		key_hash_list.push_back(msghandle::type::fix_int32(519430750));
		key_hash_list.push_back(msghandle::type::fix_int32(966318236));
		key_hash_list.push_back(msghandle::type::fix_int32(822586111));
		key_hash_list.push_back(msghandle::type::fix_int32(1269473597));
		key_hash_list.push_back(msghandle::type::fix_int32(229555721));
		key_hash_list.push_back(msghandle::type::fix_int32(676443207));
		key_hash_list.push_back(msghandle::type::fix_int32(1123330693));
		key_hash_list.push_back(msghandle::type::fix_int32(1570218179));
		key_hash_list.push_back(msghandle::type::fix_int32(1183193036));
		key_hash_list.push_back(msghandle::type::fix_int32(1254752161));
		key_hash_list.push_back(msghandle::type::fix_int32(1884105468));
		key_hash_list.push_back(msghandle::type::fix_int32(2124640138));
		Clear();
	}

void Clear(){
	Lin0_USS4_ToF_Direct={};
	Lin0_USS3_ToF_Direct={};
	Lin0_USS2_ToF_Direct={};
	Lin0_USS1_ToF_Direct={};
	Lin0_USS4_ToF_Indirect={};
	Lin0_USS3_ToF_Indirect={};
	Lin0_USS2_ToF_Indirect={};
	Lin0_USS1_ToF_Indirect={};
	Lin1_USS4_ToF_Direct={};
	Lin1_USS3_ToF_Direct={};
	Lin1_USS2_ToF_Direct={};
	Lin1_USS1_ToF_Direct={};
	Lin1_USS4_ToF_Indirect={};
	Lin1_USS3_ToF_Indirect={};
	Lin1_USS2_ToF_Indirect={};
	Lin1_USS1_ToF_Indirect={};
	Lin2_USS1_ToF_Direct={};
	Lin2_USS4_ToF_Direct={};
	Lin2_USS2_ToF_Direct={};
	Lin2_USS3_ToF_Direct={};
	Lin2_USS4_Status={};
	Lin2_USS3_Status={};
	Lin1_USS4_Status={};
	Lin1_USS3_Status={};
	Lin0_USS4_Status={};
	Lin0_USS3_Status={};
	Lin0_USS2_Status={};
	Lin0_USS1_Status={};
	Lin1_USS2_Status={};
	Lin1_USS1_Status={};
	Lin2_USS2_Status={};
	Lin2_USS1_Status={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,Lin0_USS4_ToF_Direct,Lin0_USS3_ToF_Direct,Lin0_USS2_ToF_Direct,Lin0_USS1_ToF_Direct,Lin0_USS4_ToF_Indirect,Lin0_USS3_ToF_Indirect,Lin0_USS2_ToF_Indirect,Lin0_USS1_ToF_Indirect,Lin1_USS4_ToF_Direct,Lin1_USS3_ToF_Direct,Lin1_USS2_ToF_Direct,Lin1_USS1_ToF_Direct,Lin1_USS4_ToF_Indirect,Lin1_USS3_ToF_Indirect,Lin1_USS2_ToF_Indirect,Lin1_USS1_ToF_Indirect,Lin2_USS1_ToF_Direct,Lin2_USS4_ToF_Direct,Lin2_USS2_ToF_Direct,Lin2_USS3_ToF_Direct,Lin2_USS4_Status,Lin2_USS3_Status,Lin1_USS4_Status,Lin1_USS3_Status,Lin0_USS4_Status,Lin0_USS3_Status,Lin0_USS2_Status,Lin0_USS1_Status,Lin1_USS2_Status,Lin1_USS1_Status,Lin2_USS2_Status,Lin2_USS1_Status);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","Lin0_USS4_ToF_Direct","Lin0_USS3_ToF_Direct","Lin0_USS2_ToF_Direct","Lin0_USS1_ToF_Direct","Lin0_USS4_ToF_Indirect","Lin0_USS3_ToF_Indirect","Lin0_USS2_ToF_Indirect","Lin0_USS1_ToF_Indirect","Lin1_USS4_ToF_Direct","Lin1_USS3_ToF_Direct","Lin1_USS2_ToF_Direct","Lin1_USS1_ToF_Direct","Lin1_USS4_ToF_Indirect","Lin1_USS3_ToF_Indirect","Lin1_USS2_ToF_Indirect","Lin1_USS1_ToF_Indirect","Lin2_USS1_ToF_Direct","Lin2_USS4_ToF_Direct","Lin2_USS2_ToF_Direct","Lin2_USS3_ToF_Direct","Lin2_USS4_Status","Lin2_USS3_Status","Lin1_USS4_Status","Lin1_USS3_Status","Lin0_USS4_Status","Lin0_USS3_Status","Lin0_USS2_Status","Lin0_USS1_Status","Lin1_USS2_Status","Lin1_USS1_Status","Lin2_USS2_Status","Lin2_USS1_Status");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,Lin0_USS4_ToF_Direct,Lin0_USS3_ToF_Direct,Lin0_USS2_ToF_Direct,Lin0_USS1_ToF_Direct,Lin0_USS4_ToF_Indirect,Lin0_USS3_ToF_Indirect,Lin0_USS2_ToF_Indirect,Lin0_USS1_ToF_Indirect,Lin1_USS4_ToF_Direct,Lin1_USS3_ToF_Direct,Lin1_USS2_ToF_Direct,Lin1_USS1_ToF_Direct,Lin1_USS4_ToF_Indirect,Lin1_USS3_ToF_Indirect,Lin1_USS2_ToF_Indirect,Lin1_USS1_ToF_Indirect,Lin2_USS1_ToF_Direct,Lin2_USS4_ToF_Direct,Lin2_USS2_ToF_Direct,Lin2_USS3_ToF_Direct,Lin2_USS4_Status,Lin2_USS3_Status,Lin1_USS4_Status,Lin1_USS3_Status,Lin0_USS4_Status,Lin0_USS3_Status,Lin0_USS2_Status,Lin0_USS1_Status,Lin1_USS2_Status,Lin1_USS1_Status,Lin2_USS2_Status,Lin2_USS1_Status);
} UltrasonicCAN;
REGISTER(UltrasonicCAN,UltrasonicCAN_Func);
