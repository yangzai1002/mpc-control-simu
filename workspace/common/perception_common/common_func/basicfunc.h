#ifndef BASICFUNC_H
#define BASICFUNC_H

#include"perception_common_header.h"
#include "common_datatype.h"

static int64_t gtm(){
	struct timeval tm;
	gettimeofday(&tm, 0);
	int64_t re = (((int64_t)tm.tv_sec)*1000*1000 + tm.tv_usec);
    return re;
} 


void CalcCenterLine(Straightline *line,_StraightLaneStruct* m_StraightLine);//������������߼��㳵��������


void Cal_Point_Dis_to_Line(double pointx, double pointy, double linex1, double liney1, double linex2, double liney2, double *dis, double *u);

double unifytheta(double theta);//��thetaת��Ϊ0��2pi֮��


state_struct Convert_State_Local_to_Global(state_struct localstate, state_struct vehicle_state);//�˶�״̬�Ӿֲ�ת��ȫ��

state_struct Convert_State_Global_to_Local(state_struct globalstate, state_struct vehicle_state);//�˶�״̬�Ӿֲ�ת��ȫ��

/************************坐标转换2000*****************************/
state_struct LB2000_XY2000(double L, double B/*, double *xyH2000*/);
void XY2000_LB2000(double x0, double y0, double &L, double &B, int z_zone);

positioning_struct LocalPosition_Adjustment_Func(positioning_struct org_pos, double delta_x, double delta_y, double delta_z);

Eigen::Vector3d ConvertToLocal(double globalx_in, double globaly_in, const percep_datatype::Position& current_position);

string toDateTime(uint64_t time_in);
#endif // BASICFUNC_H
