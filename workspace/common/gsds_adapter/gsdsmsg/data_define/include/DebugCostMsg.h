#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
/*

*/

typedef struct DebugCostMsg{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	double	parent_x;	//
	double	parent_y;	//
	double	current_x;	//
	double	current_y;	//
	double	cost_sum;	//
	double	length_cost;	//
	double	balance_cost;	//
	double	last_cost;	//
	double	center_cost;	//
	double	no_circular_penalty;	//
	double	curvature_cost;	//
	double	middle_cost;	//
	double	approximate_cost;	//

DebugCostMsg(){
	parent_x = 0.0;
	parent_y = 0.0;
	current_x = 0.0;
	current_y = 0.0;
	cost_sum = 0.0;
	length_cost = 0.0;
	balance_cost = 0.0;
	last_cost = 0.0;
	center_cost = 0.0;
	no_circular_penalty = 0.0;
	curvature_cost = 0.0;
	middle_cost = 0.0;
	approximate_cost = 0.0;
}

void Clear(){
	parent_x = 0.0;
	parent_y = 0.0;
	current_x = 0.0;
	current_y = 0.0;
	cost_sum = 0.0;
	length_cost = 0.0;
	balance_cost = 0.0;
	last_cost = 0.0;
	center_cost = 0.0;
	no_circular_penalty = 0.0;
	curvature_cost = 0.0;
	middle_cost = 0.0;
	approximate_cost = 0.0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list, parent_x,parent_y,current_x,current_y,cost_sum,length_cost,balance_cost,last_cost,center_cost,no_circular_penalty,curvature_cost,middle_cost,approximate_cost);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","parent_x","parent_y","current_x","current_y","cost_sum","length_cost","balance_cost","last_cost","center_cost","no_circular_penalty","curvature_cost","middle_cost","approximate_cost");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, parent_x,parent_y,current_x,current_y,cost_sum,length_cost,balance_cost,last_cost,center_cost,no_circular_penalty,curvature_cost,middle_cost,approximate_cost);
} DebugCostMsg;
REGISTER(DebugCostMsg,DebugCostMsg_Func);
