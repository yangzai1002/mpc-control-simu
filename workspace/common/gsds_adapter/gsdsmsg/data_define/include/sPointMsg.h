#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct sPointMsg{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	x;	//x
	double	y;	//y
	double	length;	//length
	double	velocity;	//v
	double	acceleration;	//a
	double	t;	//t
	double	angle;	//angle
	double	curvature;	//kappa
	double	delta;	//delta
	double	xg;	//xg
	double	yg;	//yg
	double	angleglobal;	//angleglobal
	double	acc_desired_a;	//acc_desired
	int	direction;	//2:原地转向
	bool	emergency_stop;	//emergency
	bool	is_follow;	//是否是跟随模式

	sPointMsg(){
		key_hash_list.resize(20);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(82466913));
		key_hash_list.push_back(msghandle::type::fix_int32(966587581));
		key_hash_list.push_back(msghandle::type::fix_int32(1712066697));
		key_hash_list.push_back(msghandle::type::fix_int32(15663758));
		key_hash_list.push_back(msghandle::type::fix_int32(948096978));
		key_hash_list.push_back(msghandle::type::fix_int32(840951537));
		key_hash_list.push_back(msghandle::type::fix_int32(1553804037));
		key_hash_list.push_back(msghandle::type::fix_int32(417407073));
		key_hash_list.push_back(msghandle::type::fix_int32(313558577));
		key_hash_list.push_back(msghandle::type::fix_int32(1919917831));
		key_hash_list.push_back(msghandle::type::fix_int32(193013681));
		key_hash_list.push_back(msghandle::type::fix_int32(370795721));
		key_hash_list.push_back(msghandle::type::fix_int32(1101561447));
		key_hash_list.push_back(msghandle::type::fix_int32(1013357522));
		key_hash_list.push_back(msghandle::type::fix_int32(440614181));
		key_hash_list.push_back(msghandle::type::fix_int32(1901797334));
		Clear();
	}

void Clear(){
	x={};
	y={};
	length={};
	velocity={};
	acceleration={};
	t={};
	angle={};
	curvature={};
	delta={};
	xg={};
	yg={};
	angleglobal={};
	acc_desired_a={};
	direction={};
	emergency_stop={};
	is_follow={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,length,velocity,acceleration,t,angle,curvature,delta,xg,yg,angleglobal,acc_desired_a,direction,emergency_stop,is_follow);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","length","velocity","acceleration","t","angle","curvature","delta","xg","yg","angleglobal","acc_desired_a","direction","emergency_stop","is_follow");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,length,velocity,acceleration,t,angle,curvature,delta,xg,yg,angleglobal,acc_desired_a,direction,emergency_stop,is_follow);
} sPointMsg;
REGISTER(sPointMsg,sPointMsg_Func);
