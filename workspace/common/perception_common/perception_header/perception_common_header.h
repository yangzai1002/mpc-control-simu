#ifndef PERCEPTION_COMMON_HEADER_H
#define PERCEPTION_COMMON_HEADER_H

#include <stdio.h>
#include <vector>
#include <list>
#include <string>
#include <typeinfo>
#include <math.h>
#include <queue> //队列，用于保存GPS
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <time.h>
#include <string.h>

//#include <highgui.h>
//#include <cv.h>
//#include <ml.h>
//#include <cxcore.h>
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include "macro_definition_file.h"

#include <Eigen/Eigen>
//EIGEN
#include <Eigen/Core>
#include <Eigen/Geometry>

#include <opencv2/video/tracking.hpp>    //当出现KF问题时可以加上
#include <opencv2/highgui/highgui.hpp>

//for opencv hight version
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h> 
#include <opencv2/calib3d.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core.hpp>


using namespace Eigen;

//BOOST
#include <boost/filesystem/path.hpp> 
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp>  
#include <boost/tokenizer.hpp>

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
//#include "kalman_filter.h"

//OpenCV2.0
//#include<opencv2/opencv.hpp>
//using namespace cv;



#include <sstream>                      //用于字符串处理

using namespace std;
/*******************宏定义********************/

#define LOCALMAP_RESOLUTION 0.2         //地图分辨率
#define LOCALMAP_FRONT_DISTANCE 50.0    //地图前方检测50m
#define LOCALMAP_BACK_DISTANCE 15.0     //地图后方检测15m
#define LOCALMAP_LENGTH 65.0            //地图总的长度
#define LOCALMAP_WIDTH  30.0            //地图总的宽度

#define OBSTACLE_SLOPE_HEIGHT 0.10
#define OBSTACLE_MIN_HEIGHT 0.35
#define OBSTACLE_MAX_HEIGHT 1.40
#define SLOPE_ANGLE_THRESHOLD   3.0     //坡度阈值，低于此为坡道
//#define OBSTACLE_ABSOLUTE_HEIGHT        //利用绝对高度进行障碍物判断
#define OBSTACLE_COUNT_THRESH 5         //障碍物统计的最低阈值

//AVT相机图像原始分辨率
#define AVT_IMAGE_WIDTH     2048
#define AVT_IMAGE_HEIGHT    1088

//KITTI图像原始分辨率
#define KITTI_IMAGE_WIDTH     1241
#define KITTI_IMAGE_HEIGHT    376

#define LOCALMAP_HEIGHT_PIXEL   cvRound(LOCALMAP_LENGTH/LOCALMAP_RESOLUTION)
#define LOCALMAP_WIDTH_PIXEL    cvRound(LOCALMAP_WIDTH/LOCALMAP_RESOLUTION)

//色彩定义
#define YELLOW_COLOR    cv::Scalar(255,255,0)
#define BLUE_COLOR    cv::Scalar(0,0,255)
#define GREEN_COLOR    cv::Scalar(0,255,0)
#define LIGHT_GREEN_COLOR    cv::Scalar(0,127,0)
#define RED_COLOR    cv::Scalar(255,0,0)
#define GREY_COLOR  cv::Scalar(127,127,127)
#define CYAN_COLOR  cv::Scalar(0,255,255)
#define BLACK_COLOR cv::Scalar(0,0,0)
#define WHITE_COLOR cv::Scalar(255,255,255)
#define PURPLE_COLOR cv::Scalar(128,0,255)

#define UNKNOWN					0
#define PASSABLE				1       //可通行
#define RIGIDNOPASSABLE			2       //不可通行
#define RADARNOPASSABLE			4
#define UNFLATNESS				6

#if 0
enum TASK_MODE                          //驾驶模式，自动驾驶/人员跟随/编队行驶
{
    TASK_IDLE,
    AUTO_NAVIGATION = 258,    //自主导航，路径跟踪
    TRACKING_PERSON = 260,    //人员跟随
    TRACKING_VEHICLE = 263,   //车辆跟随
    AUTO_RETURN = 259         //一键返航
};
#else
enum TASK_MODE                          //驾驶模式，自动驾驶/人员跟随/编队行驶
{
    TASK_IDLE = 0,
    RNDF_RECORDER = 1,                 //路点记录
    AUTO_NAVIGATION = 2,                //自主导航，路径跟踪
    AUTO_RETURN = 3,                    //一键返航
    TRACKING_PERSON_LOW_SPEED = 4,      //人员跟随,低速
    TRACKING_PERSON_WINDING_PILE = 5,   //人员跟随，绕桩
    TRACKING_PERSON_FAR_DISTANCE = 6,   //人员跟随，远距离测试
    TRACKING_VEHICLE = 7,               //车辆跟随
    AUTO_NAVI_PAUSE = 8,                //暂停
    AUTO_NAVI_RECOVERY = 9,             //恢复自动驾驶功能
    AUTO_NAVI_STOP = 10,                //停止自动驾驶功能
    VOICE_GUIDANCE = 11,                //语音引导跟随
    GESTURE_GUIDANCE = 12               //手势引导跟随
};
#endif
enum SUB_TASK_MODE
{
    _NORMAL_SUB_MODE = 1,           //普通模式
    _PURE_TRACKING_SUB_MODE,    //纯跟踪
    _LOOP_SUB_MODE,             //绕圈模式
    _TRACKING_LOOP_MODE,         //跟踪和绕圈模式
    _RNDF_RECORDER_ON,          //路点记录开始
    _RNDF_RECORDER_OFF          //路点记录结束
};


enum data_source{
    UDP_SOCKET_DATA_TYPE,
    PCAP_OFFLINE_DATA_TYPE,
    PCD_OFFLINE_DATA_TYPE,
    KITTI_DATA_TYPE,
    DNET_DATA_TYPE          //从dnet获取数据
};

//#define Z_MAX					2.0
//#define Z_MIN                   -2.0
#define SAC_MAX_ITERATIONS		100
#define MIN_DISTANCE_THRESHOLD	1.0
#define MAX_DISTANCE_THRESHOLD	80.0
//#define pi						3.1415926
#define EPSILON					0.001

#define BIT_VEHICLELEN_M 2.8//;;;	//4.0 //车头到后轴 //----------------LN 2014 0804-wait for correct
#define  BIT_LFR	1.70 /*2.85*/	//前后轴距 // -----------------LN 2014 08 04 correct
#define BIT_VEHICLEWIDTH_M	2.4//2.8车宽//----------------LN 2014 0804-wait for correct
#define BIT_DEC -1.0/*-0.5*/
#define  BIT_ACC	1.0/*0.5*/
#define BIT_MAXLATERALACC	4.0//最大侧向加速度 原来为4.0
#define MAX_TURN_WHEEL_ANGLE    20.0
#define BIT_INVALIDVALUE	1000.0

//属性图
//enum ATTRIBUTE_OF_MAP
//{
//    UNKNOWN_ZONE,       //未知，0，雷达线未扫描到        //PERFECT_PASSABLE,   //完全可通行，0
//    WATER_ZONE,         //水，1
//    DYNAMIC_OBSTACLE,   //动态障碍物，2
//    DYNAMIC_VEHICLE,    //车，3
//    DYNAMIC_PERSON,     //人，4
//    SLOPE_ZONE,         //坡道，5
//    NEGATIVE_OBSTACLE,  //负障碍，6
//    WELL_PASSABLE,      //可通行，7
//    POSITIVE_OBSTACLE,  //正障碍，8
//    CLIFF_ZONE,         //悬崖,9
//    HISTORY_OBSTACLE    //地图信息重建障碍物,10
//};

enum ATTRIBUTE_OF_MAP
{
    UNKNOWN_ZONE = 0,       //未知，0
    WELL_PASSABLE,      //可通行，1
    HIS_POSITIVE_OBSTACLE,  //正障碍，2
    HIS_NEGATIVE_OBSTACLE,  //负障碍，3
    TRENCH_ZONE,            //壕沟，4(之前是水）
    WALL_ZONE/*DYNAMIC_OBSTACLE*/,   //垂直墙，5***********动态障碍物，5
    DYNAMIC_VEHICLE,    //车，6
    DYNAMIC_PERSON,     //人，7
    SLOPE_ZONE,         //坡道，8
    CLIFF_ZONE,         //悬崖,9
    HISTORY_OBSTACLE,    //地图信息重建障碍物,10
    CUR_POSITIVE_OBSTACLE = 102,    //当前帧数据
    CUR_NEGATIVE_OBSTACLE = 103,  //当前帧数据
    CUR_GROUND = 104     //当前帧数据
};

struct radar_data_node
{
    float range;
    float angle;    //angle 0~180
    float velocity;
    float accel;
    float lat_rate;
    float width;
    bool flag;
    double localX;
    double localY;
    int px,py;
    int times;
    double lastX;
    double lastY;
    double globalX;
    double globalY;
    bool update;
    int idx;
};


/* motion parameters*/
typedef struct
{
    Eigen::Matrix3d R;			/* Rotation */
    Eigen::Vector3d t;			/* Translation */
    Eigen::Vector3d A;			/* angles, {dyaw, dpitch, droll} */
}motionp;

struct voxel
{
    float x;
    float y;
    float z;
};

struct gauss_score
{
    int width;
    int closeID;
    float score;
};
struct dist_rank
{
    int id;
    int width;
};

struct hdl16data
{
    vector<voxel> data[16];
};

struct roadedgepts
{
    vector<voxel> points;
};

struct roadedgeresult
{
    bool left;
    bool right;
    voxel e_left;
    voxel e_right;
};

struct replanning_point
{
    double x;
    double y;
    int index;
    bool flag;
};

struct cluster_point
{
    float x,y,z;        //真实车体局部坐标系
    int h,w;            //图像坐标系
    float intensity;    //回波强度
    int area_num;       //所属的聚类区域
};

struct rect_person
{
    float x1,y1,z1;                     //左后下角
    float x2,y2,z2;                     //右前上角
};

struct cluster_area
{
    float center_x,center_y,center_z;   //真实车体局部坐标系
    float x1,y1,z1;                     //左后下角
    float x2,y2,z2;                     //右前上角
    float radius;
    float width,length,height;          //
    int center_h,center_w;              //图像坐标系
    float average_intensity;            //回波强度
    float area;                         //区域面积
    float volume;                       //区域体积
    int total;                          //点的数量
    int intensity_count;                //统计回波强度较高的雷达点个数
    vector<CvPoint3D64f> lidars;        //区域内雷达点云
    rect_person global_person;          //全局坐标系下的行人位置
    double score;
    double distribution;                //分布情况
    double ZDiffupGround;                   //地面高度差
};
struct tracking_target
{
    cluster_area cube_;
    bool flag;
    CvPoint2D32f imgPos;             //当前图像位置
    CvPoint2D32f predImgPos;             //预测图像位置，2020-03-23
    double local_x;
    double local_y;
    double global_x;
    double global_y;
    double global_z;
    float velocity;
    float vx,vy;
    double heading;
    float px,py;        //预测位置
    float refx,refy;    //参考位置
    int tracking_time;  //跟踪次数
    CvRect rect;
    vector<CvPoint3D64f> lidars;//行人的当前点云
    vector<double> h_global_x;   //历史轨迹
    vector<double> h_global_y;   //历史轨迹
    vector<double> h_heading;    //历史朝向
    vector<long> h_times;      //时间序列
    vector<rect_person> h_person;           //每个行人目标的历史位置
    bool cur_update;            //每个已有跟踪目标只能被匹配一次

    cv::KalmanFilter KF;
    int stateNum;
    int measureNum;
    cv::Mat state;
    cv::Mat ProcessNoise;
    bool is_target_;
    int index;
    vector<int> occupiedImgPos; //行人目标占据的网格
//    Mat measurement;
    long curTime;             //当前OpenCV系统时间
};

/****************结构体**********************/
struct positioning_struct
{
    double longitude;   //经度
    double latitude;    //纬度

    double global_x;    //无人车全局坐标X值(北) 北东地坐标系
    double global_y;    //无人车全局坐标Y值(东)
    double global_h;    //无人车全局高程H值(地)

    double global_vx;   //无人车全局X轴向速度(北)
    double global_vy;   //无人车全局Y轴向速度(东)
    double global_vz;   //无人车全局H轴向速度(天)

    //角度单位统一为角度
    double heading;     //航向
    double pitch;       //俯仰角
    double roll;        //倾斜角
    double heading_rate;    //航向角速率

    double global_wx;   //航向角速度
    double global_wy;   //俯仰角速度
    double global_wz;   //倾斜角速度

    double mileage;     //里程

    double Velocity;    //里程计速度
    double Wheelangle;  //前轮偏角
    double Curvature;   //曲率

    int GPSstate;       //GPS差分信号,差分为4
    unsigned int z_zone;         //区号

    long curTime;       //当前时间
    float yawCorrect;   //角度校准

};
struct position_3D
{
    double global_x;    //无人车全局坐标X值(北) 北东地坐标系
    double global_y;    //无人车全局坐标Y值(东)
    double global_h;    //无人车全局高程H值(地)
    //角度单位统一为角度
    double heading;     //航向
    double pitch;       //俯仰角
    double roll;        //倾斜角

    cluster_area cube_;
};

struct vehicle_size_parameter
{
    float left;
    float right;
    float width;
    float front;
    float back;
    float length;
    float top;
    float bottom;
};

typedef struct
{
    int x;
    int y;
    int z;
    int i;
    int c;      //雷达序号
    int rot;
    int t;      //多帧融合
}lidarPoint;

typedef struct
{
    lidarPoint pt[4000];
    int pt_count;
} Point_laser;


/********************************地图显示*************************************/
#define		BIT_WAYPOINT		1
#define		BIT_INTERSECTION	2
#define BIT_EPSILON_ERROR	0.000001
/***********全局地图常量*************/
#define BIT_MAP_WIDTH_CELL 600
#define	BIT_MAP_HEIGHT_CELL	600
//驾驶行为，取决于环境与路网信息
#define BIT_DRIVERMODE_UNKNOWN	0
#define BIT_DRIVERMODE_LANE	1	//车道内行驶
#define BIT_DRIVERMODE_ROADMAP		2	//自由区域跟随路网行驶
#define BIT_DRIVERMODE_FIELDMETHOD	3	//纯环境感知行驶
#define BIT_DRIVERMODE_FIELDMETHOD_FINE	4	//精细纯环境感知行驶
#define BIT_DRIVERMODE_ROADMAPBACKUP	5

#define BIT_OGMRESOLUTION 0.2//;	//0.2
#define	BIT_OGMZOOM	1

#define	BIT_OGMWIDTH_CELL 150//;	//201//OGMWIDTH_M/OGMRESOLUTION+1
#define	BIT_OGMHEIGHT_CELL 325//;//	501//OGMHEIGHT_M/OGMRESOLUTION+1
#define	BIT_VEHICLE_DIS_TO_BOTTOMInOGM_M	cvRound(BIT_OGMHEIGHT_M-BIT_VEHICLEPOSINOGM_Y_M)//后轴到bottom的距离 10m

#define BIT_VEHICLELEN_M 2.8//;;;	//4.0 //车头到后轴 //----------------LN 2014 0804-wait for correct
#define BIT_VEHICLEWIDTH_M	2.4//2.8车宽//----------------LN 2014 0804-wait for correct
#define BIT_VEHICLEPOSINOGM_X_M 15//;// 15//车后轴在局部坐标系中的横坐标（m）
#define BIT_VEHICLEPOSINOGM_Y_M 50//;// 50车后轴在局部坐标系中的纵坐标（m）
#define	BIT_VEHICLE_DIS_TO_BOTTOMInOGM_CELL	cvRound(BIT_VEHICLE_DIS_TO_BOTTOMInOGM_M/BIT_OGMRESOLUTION)//后轴到bottom的距离50cell
#define BIT_VEHICLEPOSINOGM_X_CELL cvRound(BIT_VEHICLEPOSINOGM_X_M/BIT_OGMRESOLUTION)
#define BIT_VEHICLELENGTHINCELL	cvRound(BIT_VEHICLELEN_M/BIT_OGMRESOLUTION)
#define	BIT_OUTTERINFLATEDHEIGHT_CELL BIT_VEHICLELENGTHINCELL//CELL 5m
#define BIT_HALFOUTTERINFLATEDWIDTH_CELL	cvRound(BIT_VEHICLEWIDTH_M/2/BIT_OGMRESOLUTION) //CELL

#define BIT_OGMWIDTH_M 30//;//		30
#define BIT_OGMHEIGHT_M 65//;//		65

#define ROAD_ANGLE_DELTA_THRESHOLD 40                   //基于路网点的大角度转弯判定,原定为45度
#define ROAD_ANGLE_DELTA_THRESHOLD2 15                 //基于路网点小角度转弯判定
#define SLOWDOWN_DISTANCE 40                                   //路网减速距离,之前为40米


//来自路网的道路类型
#define	BIT_UNKNOWN	0
#define	BIT_LANE	1
#define	BIT_CURVE	2
#define	BIT_FIELD	3
#define	BIT_TURNENTER	4
#define	BIT_TURNINNER	5
#define	BIT_NOTURNENTER	6
#define	BIT_NOTURNINNER	7
#define	BIT_UTURNENTER	8
#define	BIT_UTURNINNER	9
#define	BIT_WORKINGZONE	10
#define	BIT_TURNEXIT	11

//来自路网的方向
#define	BIT_CENTER 1
#define	BIT_LEFT	2
#define	BIT_RIGHT	3

#define BIT_COLORRED            cv::Scalar(0,0,255)
#define BIT_COLORBLUE           cv::Scalar(255,0,0)
#define BIT_COLORGREEN          cv::Scalar(0,255,0)
#define BIT_COLORWHITE          cv::Scalar(255,255,255)
#define BIT_COLORCYAN           cv::Scalar(255,255,0)
#define BIT_COLORYELLOW         cv::Scalar(0,255,255)
#define BIT_COLORPINK           cv::Scalar(255,0,255)
#define BIT_COLORGREY           cv::Scalar(122,122,122)
#define BIT_COLORORANGE         cv::Scalar(0,128,255)
#define BIT_COLORPURPLE         cv::Scalar(255,0,128)
#define BIT_COLORDARKGREY       cv::Scalar(188,188,188)
#define BIT_COLORLIGHTGREY      cv::Scalar(100,100,100)

struct pose
{
    double x;
    double y;
    double theta;
    double weight;
};

struct pos_int
{
    int x;
    int y;
};

struct pos
{
    double x;
    double y;
    double h;
};
struct state_struct
{
    pos position;
    double s;//弧长
    double theta;//弧度，车头与正东方向夹角，heading=M_PI/2-theta; heading_g=heading_l+heading_v
    double global_theta_E;
    bool forward;//前进或者后退

    //state_struct用于描述原始全局期望路径，以下属性基本没用 ps：没用写个鬼啊！
    double global_s;
    double steering_angle;
    vector<pos_int> footprint;

    //来自路网的属性
    int index;
    char type;
    char side;
    double maxvel;
    char dynamic_obstacle;
    char curb;
    char seg;
    int waitcounter;//单位100ms
    //------------------------------------------------LN 2014 07 26 add-------------------------------
    int recovery;
    int reconstruct;
    //------------------------------------------------LN 2014 07 26 add-------------------------------
    double road_delta_angle;           //LN 2016 0707 add用于路的转弯大小的判断

    unsigned int zone;                 //区号，LN 2018 01 26 添加
};

struct Straightline
{
    double x1;
    double y1;
    double x2;
    double y2;
    double theta;
    char continuous;
    char color;
    double len;
};

struct _StraightLaneStruct
{
    //x1,x2,y1,y2: mm
    double x1;
    double y1;
    double x2;
    double y2;
    double x3;
    double y3;
    double x4;
    double y4;
    char rightcon;//0没有//1间断//2连续
    char leftcon;//0没有//1间断//2连续
    int lanestatus;//车道数
    int prev_lanestatus;
    //LARGE_INTEGER	lTimeStamp;
};

typedef struct struct_VehicleStateToNet			// 车辆状态
{
    double fPos_east;					// 车辆东向位置(Unit:m)	绝对的GPS坐标，定时初始化integrated位置，用于全局定位，更新周期1s
    double fPos_north;					// 车辆北向位置(Unit:m)

    double fDRPos_E;					// 航迹推算位置东向(m) 绝对坐标系下的DR结果（从起点起），用于生成全局期望路径路径跟踪控制
    double fDRPos_N;					// 航迹推算位置北向(m)

    double fold_Pos_east;				//记录上次gps定位结果
    double fold_Pos_north;
    double fold_DRPos_E;				//记录上次DR定位结果
    double fold_DRPos_N;

    double f_integrated_east;			//用于车辆在全局路径中定位，并从roadmap中确定全局期望路径的起点，更新周期100ms。
    double f_integrated_north;

    /*******地图匹配相关定位结果********/
    double f_slam_theta;
    double f_slam_east;					//粒子滤波SLAM定位结果，相对位置。以DR位置为运动输入
    double f_slam_north;

    double f_filtered_east;				//粒子滤波定位结果与DR定位结果的Kalman滤波结果，修正DR定位结果用于路径跟踪控制
    double f_filtered_north;

    /***************/
    double f_corrected_DRPos_E;			//地图匹配修正过的DR位置
    double f_corrected_DRPos_N;

    double current_curvature;			//当前曲率
    double fForwardVel;					// 车辆纵向速度(Unit:m/s)
    double fDeForwardVel;
    double fFLRWheelAverAngle;			// 名义前轮偏角，对应电机或方向盘的角度(Unit:°)
    double fHeading;						// 车辆真北航向角(Unit:弧度),顺时针由0至360
    double fTheta;						//车辆到正东的夹角(Unit:弧度)
    double fPitch;                          //车辆的俯仰角
    unsigned char f_shift;				//档位 0无效1P2R3N4N5D6M7S8+9-
    unsigned char f_shift1;				//具体档位
    unsigned char f_estop;				//紧急制动
    unsigned char f_leftlamp;			//左转向灯
    unsigned char f_rightlamp;			//右转向灯
    bool lateralctrl_enabled;
    bool longitutdectrl_enabled;
    bool brake_enabled;

    //LARGE_INTEGER	lTimeStamp;			// 时间戳(Unit:ms)

}SVehicleStateToNet,*PSVehicleStateToNet;

struct gps_xy_struct
{
	double longitude;
	double latitude;
	double x;
	double y;
	double fTheta;
	int GPSstate;
};

struct TrajPoint
{
    double x;//
    double y;//
    double theta;//车头与正东方向夹角，而坐标系之间的夹角应该是车头与正北的夹角（局部坐标系中与横轴正向夹角，顺时针为正）
    vector<pos_int> footprint;//该轨迹点的对应的车身位置，在ogm坐标系中
    double s;//到零点的弧长
    double global_s;
    double k;//曲率，对应前轮偏角
    double steeringangle;//该路径点的转向角
    //bool robustness;

    double maxvel_by_radius;
    double maxvel_by_backstepping;
    double maxvel_by_maximumrange;
    double maxvel;//两个trajpoint之间的最大允许速度

    double vel;//
    double t;//到零点的时间
};

struct Trajectory
{
	vector<TrajPoint> Traj_Points;
	vector<TrajPoint> Traj_Points_lower;
	vector<TrajPoint> Traj_Points_upper;

	double lateral_offset;//用于局部路径时，表示与期望路径的关系

	//属性
	double s;//路径长度
	double global_s;
	//double robustness_s;
	//double robustness_global_s;
	double cost;

	double max_vel;
	double terminalvel;
	bool truncated;
	double safety_dis;
	int dangerour_millimeter_obj_index;
	double elegant_distance;
	double mindis_2_reference;

	bool selectable;

	double maximum_range;
	//轨迹对应的控制量
	double steeringangle;
	double vel;

	//20170215 添加每条跟踪轨迹的id
	int id;
};

struct _OGMDataStruct
{
    unsigned char* m_OccupiedMap;
    //LARGE_INTEGER	lTimeStamp;			// 时间戳(Unit:ms)
};

#endif // PERCEPTION_COMMON_HEADER_H
