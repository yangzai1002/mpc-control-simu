#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionLaneLinePoint.h"
/*
感知车道线部分拟合线
*/

typedef struct PerceptionLinePart{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<double>	coeff;	//c0 ... c3.  y = c0 + c1*x + c2*x^2 + c3*x^3 + c4*x^4 + ...
	std::vector<PerceptionLaneLinePoint>	points;	//车道线匹配点对
	PerceptionLaneLinePoint	start_point;	//车道线起始点，最近点
	PerceptionLaneLinePoint	end_point;	//车道线结束点，最远点
	int	line_mark;	//虚实线: Unknown = 0 SolidLine = 1 DashedLine = 2 ShortDashedLine = 3 DoubleSolidLine = 4 DoubleDashedLine = 5 LeftSolidRightDashed = 6 RightSolidLeftDashed = 7 ShadedArea = 8 LaneVirtualMarking = 9 IntersectionVirualMarking = 10 CurbVirtualMarking = 11 UnclosedRoad = 12 RoadVirtualLine = 13 DecelerationSolidLine = 14 DecelerationDahLine =  15
	int	line_location;	//车道线相对位置 LEFT=0;LEFTL=1;LEFTLL=2;RIGHT=3;RIGHTR=4;RIGHTRR=5;LEFTOUTSIDE=6;RIGHTOUTSIDE=7;UNKNOWN=8

	PerceptionLinePart(){
		key_hash_list.resize(10);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1784020322));
		key_hash_list.push_back(msghandle::type::fix_int32(573823340));
		key_hash_list.push_back(msghandle::type::fix_int32(1617658956));
		key_hash_list.push_back(msghandle::type::fix_int32(1343356272));
		key_hash_list.push_back(msghandle::type::fix_int32(1759564139));
		key_hash_list.push_back(msghandle::type::fix_int32(924109183));
		Clear();
	}

void Clear(){
	coeff={};
	points={};
	start_point={};
	end_point={};
	line_mark={};
	line_location={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,coeff,points,start_point,end_point,line_mark,line_location);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","coeff","points","start_point","end_point","line_mark","line_location");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,coeff,points,start_point,end_point,line_mark,line_location);
} PerceptionLinePart;
REGISTER(PerceptionLinePart,PerceptionLinePart_Func);
