#include "basicfunc.h"
/**
 * @brief Cal_Point_Dis_to_Line
 * @param pointx
 * @param pointy
 * @param linex1
 * @param liney1
 * @param linex2
 * @param liney2
 * @param dis
 * @param u
 * 计算点到直线距离
 */
void Cal_Point_Dis_to_Line(double pointx, double pointy, double linex1, double liney1, double linex2, double liney2, double *dis, double *u)
{
    double length=sqrt(pow(liney1-liney2,2)+pow(linex1-linex2,2));
    *dis=fabs((linex2-linex1)*(liney1-pointy)-(linex1-pointx)*(liney2-liney1))/length;

    *u=((pointx-linex1)*(linex2-linex1)+(pointy-liney1)*(liney2-liney1))/(length*length);
}
/**
 * @brief unifytheta
 * @param theta
 * @return
 * 角度统一，将角度约束在0～2*PI之间
 */
double unifytheta(double theta)
{
    double result=theta;
    if(theta<0)		result=theta+2*CV_PI;
    if(theta>=2*CV_PI)		result=theta-2*CV_PI;
    return result;
}

/**
 * @brief Convert_State_Local_to_Global
 * @param localstate
 * @param vehicle_state
 * @return
 * 局部坐标转全局坐标
 *
 */
state_struct Convert_State_Local_to_Global(state_struct localstate, state_struct vehicle_state)
{
    double theta=vehicle_state.theta;

//    二维局部坐标变换
    double newx,newy;
    newx=vehicle_state.position.x+sin(theta)*localstate.position.x+cos(theta)*localstate.position.y;
    newy=vehicle_state.position.y-cos(theta)*localstate.position.x+sin(theta)*localstate.position.y;

    state_struct result;
    result.position.x=newx;
    result.position.y=newy;
    result.theta=localstate.theta+vehicle_state.theta-CV_PI/2;
    result.theta=unifytheta(result.theta);

    result.s=localstate.s;
    result.global_s = localstate.global_s;
    result.forward = localstate.forward;

    result.steering_angle=localstate.steering_angle;//转向角度不变
    return result;
}

/**
 * @brief Convert_State_Global_to_Local
 * @param globalstate
 * @param vehicle_state
 * @return
 * 全局坐标转局部坐标
 *
 */
state_struct Convert_State_Global_to_Local(state_struct globalstate, state_struct vehicle_state)
{
    double deltax,deltay;
    deltax=globalstate.position.x-vehicle_state.position.x;
    deltay=globalstate.position.y-vehicle_state.position.y;

    double theta=vehicle_state.theta;

    //全局坐标转局部坐标
    double newx,newy;
    newx=sin(theta)*deltax-cos(theta)*deltay;
    newy=cos(theta)*deltax+sin(theta)*deltay;

    state_struct result;
    result.position.x=newx;
    result.position.y=newy;
    result.theta=CV_PI/2+globalstate.theta-vehicle_state.theta;
    result.theta=unifytheta(result.theta);

    result.s=globalstate.s;
    result.global_s = globalstate.global_s;
    result.forward = globalstate.forward;

    result.steering_angle=globalstate.steering_angle;//Ŀǰû��
    return result;
}


/***********************其他**************************/
/*****************2000经纬度转换为2000直角坐标**********************************/
state_struct LB2000_XY2000(double L, double B/*, double *xyH2000*/)
{
    state_struct result;
    double L0,delta_L;
    double gmmac;
    unsigned int unum;

    double sx,N,ita2,m,t;
    double A0,B0,C0,D0,E0;
    double t2,t4,ita4;
    double a2000,b2000,e2,e1;

    unum= ((int)floor(L*180.0/CV_PI))/6+1;  //计算区号

    L0=(double)unum*6.0-3.0;//计算中心经度
    delta_L=L-L0*CV_PI/180.0;

    a2000=6378137.e0;
    b2000=6356752.31e0;

    e2= (a2000*a2000-b2000*b2000)/(a2000*a2000);
    e1= sqrt((a2000*a2000-b2000*b2000)/(b2000*b2000));

    L=L*180.0/CV_PI;
    ita2=e1*e1*cos(B)*cos(B);
    t=tan(B);
    t2=t*t;
    t4=t2*t2;
    ita4=ita2*ita2;
    N=a2000/(sqrt(1-e2))/sqrt(1+ita2);
    m=cos(B)*(CV_PI)/180.0*(L-L0);

    A0= 6367449.1436;
    B0= 32009.8248;
    C0= 133.998;
    D0= 0.6975211868;
    E0= 0.003914074778;

    sx=A0*B-(B0+C0*sin(B)*sin(B)+D0*sin(B)*sin(B)*sin(B)*sin(B)+E0*sin(B)*sin(B)*sin(B)*sin(B)*sin(B)*sin(B))*sin(B)*cos(B);
    result.position.y=sx+N*t*(0.5e0*pow(m,2.e0)+1.e0/24.e0*(5.e0-t2+9.e0*ita2+4.e0*ita4)*pow(m,4.0)+(61.e0-58.e0*t2+t4)*pow(m,6.0)/720.e0);
    result.position.x=N*(m+(1.0-t2+ita2)*pow(m,3.0)/6.e0+(5.0-18.0*t2+t4+14.0*ita2-58.0*ita2*t2)*pow(m,5.0)/120.0);
    result.position.x=result.position.x+500000.0;

    result.zone = unum;

    return result;
//    gmmac=delta_L*sin(B);

//    xyH2000[2]= gmmac;
//    xyH2000[3]=(double)unum;

}

/*****************2000直角坐标转换为2000经纬度**********************************/
void XY2000_LB2000(double y0, double x0, double& L,double& B,int z_zone/*区号*/)
{
    double a2000,b2000;
    double ep2,lg2,V,c,N,t,V2,B0,Bf,y1,aa,unum,L0,gmmac;
    double delta_L;

    a2000=6378137.e0;
    b2000=6356752.31e0;

    y0 = y0-500000;
    unum = z_zone/*LB2000[3]*/;

    B0 = x0/ 6367449.1437023;
    Bf = B0+0.00251882708512*sin(2.0*B0)+0.00000370095115*sin(4.0*B0)+0.0000000053764*sin(6.0*B0);

    ep2 = (a2000*a2000-b2000*b2000)/(b2000*b2000);
    lg2 = ep2*cos(Bf)*cos(Bf);
    V = sqrt(1+lg2);
    c= a2000*a2000/b2000;
    N = c/V;
//	M = c/(1+lg2)/V;
    t = tan(Bf);
    y1 = y0/N;
    V2 = 1+lg2;

    aa =1/cos(Bf);
    L0 =unum*6-3;

    B =Bf-0.5*V*V*t*y1*y1+1/24.0*(5.0+3.0*t+lg2-9.0*lg2*t*t)*V2*t*y1*y1*y1*y1
        -1/720.0*(61+90*t*t+45*t*t*t*t)*V2*t*y1*y1*y1*y1*y1*y1;

    L= aa*y1-1.0/6*(1+2.0*t*t+lg2)*aa*y1*y1*y1+1.0/120.0*(5+28.0*t*t+24.0*t*t*t*t
        +6.0*lg2+8.0*lg2*t*t)*aa*y1*y1*y1*y1*y1;

    L = L0/180.0*CV_PI +L;

    delta_L=L-L0/180.0*CV_PI;
    B = B / CV_PI * 180.0;
    L = L / CV_PI * 180.0;

}

/*以GPS为参考点，计算偏差delta*/
positioning_struct LocalPosition_Adjustment_Func(positioning_struct org_pos, double delta_x, double delta_y, double delta_z)
{
    /**
     * 全局坐标点
     */
    positioning_struct result;
    double lng = org_pos.longitude;
    double lat = org_pos.latitude;
    state_struct gpsdata=LB2000_XY2000(lng*CV_PI/180.0,lat*CV_PI/180.0);//经纬度转换为米制

    result.z_zone = gpsdata.zone;
    double theta = CV_PI/2.0 - org_pos.heading*CV_PI/180.0;
    if(theta<0)
        theta=theta+2.0*CV_PI;
    if(theta>=2.0*CV_PI)
        theta=theta-2.0*CV_PI;
    /**
      二维坐标角度变换
      */
    result.global_x = gpsdata.position.x + sin(theta)*delta_x + cos(theta)*delta_y;
    result.global_y = gpsdata.position.y - cos(theta)*delta_x + sin(theta)*delta_y;

    XY2000_LB2000(result.global_x,result.global_y,result.longitude,result.latitude,result.z_zone);
    return result;
}

Eigen::Vector3d ConvertToLocal(double globalx_in, double globaly_in, const percep_datatype::Position& current_position){
    state_struct global_state,origin_state;
    origin_state.position.x = current_position.global_x;
    origin_state.position.y = current_position.global_y;
    origin_state.theta = M_PI/2-current_position.heading*M_PI/180.0;

    if(origin_state.theta < 0)   origin_state.theta = origin_state.theta + 2*M_PI;
    if(origin_state.theta >= 2*M_PI) origin_state.theta = origin_state.theta - 2*M_PI;

    global_state.position.x = globalx_in;
    global_state.position.y = globaly_in;

    //将局部坐标变换到全局坐标
    state_struct local_vehicle = Convert_State_Global_to_Local(global_state,origin_state);
	Eigen::Vector3d result;
	result[0] = local_vehicle.position.x;
	result[1] = local_vehicle.position.y;
	result[2] = local_vehicle.theta;
	return result;
}


string toDateTime(uint64_t time_in){
	// Convert to chrono nanoseconds type
  	chrono::nanoseconds tp(time_in), tt_nano(time_in);
  	// Get nanoseconds part
  	tp -= chrono::duration_cast<chrono::seconds>(tp);
	//to_time_t方法把时间点转化为time_t
	time_t tt =  std::time_t(chrono::duration_cast<chrono::seconds>(chrono::nanoseconds(tt_nano)).count());
  	//把time_t转化为tm
 	tm *localTm =  localtime(&tt);
 	//tm格式化输出
 	char buffer[20];
 	char format[18] = "%Y-%m-%d %H:%M:%S";
 	strftime(buffer, sizeof(buffer), format, localTm);
  	stringstream ss;
  	ss<<buffer;
  	return ss.str();
}