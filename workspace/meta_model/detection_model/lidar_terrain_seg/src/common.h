#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <stdlib.h>
#include <time.h> 
#include <algorithm>
#include <deque>
#include <unordered_set>
#include <math.h>
#include <ctime>
#include <string>
#include <unordered_map>
//PCL
// #include <pcl/sample_consensus/method_types.h>
// #include <pcl/sample_consensus/model_types.h>
// #include <pcl/segmentation/sac_segmentation.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/impl/passthrough.hpp>
#include <pcl/filters/extract_indices.h>
#include <pcl/features/normal_3d.h> 

//Eigen
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <queue> 
#include <thread> 

// #include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

//RM STATUS
#define UNKNOWN			0
#define NONVALID		-9999
#define DONTCARE        -99
#define EDGEPT			-9
#define	INVALIDDOUBLE		99999999.9
#define	nint(x)			(int)((x>0)?(x+0.5):(x-0.5))
//DEM LAB STATUS
#define TRAVESABLE		1
#define NONTRAVESABLE	2


#define POSSIOBSTA		3
#define	NEGATOBSTA		4
#define HANGDOWNTR		5
#define HANGDOWNUN		6
#define	FLATGROUND		10
#define DOWNSLOPE		11
#define UPSLOPE			12
#define	LEFTSIDESLOPE	13
#define	RIGHTSIDESLOPE	14
#define	EDGEPOINTS		15
#define NEGATOBSTA16    16
#define DITCH           17
#define WALL            18




struct PointXYZIRT{
    PCL_ADD_POINT4D;
    float intensity;
    int ring;
    float timeflag;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  }EIGEN_ALIGN16;

  POINT_CLOUD_REGISTER_POINT_STRUCT(PointXYZIRT,
    (float,x,x)
    (float,y,y)
    (float,z,z)
    (float,intensity,intensity)
    (int, ring, ring)
    (float, timeflag, timeflag)
  )

struct point2d
{
    double x=0;
    double y=0;
};

struct pointopt  
{
	int x=0,y=0;
	double opti=0;
};


struct point3d
{
    double x=0;
    double y=0;
    double z=0;
};

typedef double  MATRIX[3][3] ;

typedef struct {
    point3d	ang;
    point3d	shv;
    MATRIX	rot;
} TRANSINFO;

typedef double  MAT2D[2][2] ;

typedef struct {
    double	ang;
    point2d	shv;
    MAT2D	rot;
} TRANS2D;

typedef struct {
    float	x=0, y=0, z=0;
    float	i=0;
} point3fi;

struct point2i{
    int x=0, y=0;
} ;

typedef struct {
    float	x=0, y=0, z=0;
    float	o=0;      // 排序关键字（按照16线扫描的水平角度）
} point3fo;


typedef	struct {
    unsigned short	lab = 0;
    point2i		dmin;
    point2i		dmax;
    PointXYZIRT	maxxp,maxyp, maxzp;
    PointXYZIRT	minxp,minyp, minzp;
    point3d		cp;
    int			ptnum = 0;
    point3d		norm;
    double		var = 0;
} SEGBUF;

typedef struct {
    int	 regnum;
    std::vector<PointXYZIRT> pts;
    std::vector<float> range;
    std::vector<int> idx;
    std::vector<unsigned char>	di;		//for data alignment only
    std::vector<int> regionID;
    std::vector<SEGBUF> segbuf;
} RMAP;


typedef struct {
    int			x0,x1;		//DEM中的地面点开始和结束像素序号[0,dm.wid)
    double		h;			//中心点位置(x0+x1)/2))的地面高度
    double		dl;			//纵向距离y处的地面扫描线与前一条扫描线间的正常水平距离
                            //前一条扫描线为与车体更近的那条，两个扫描线之间的角度d_ang=(VMAXANG-VMINANG)/63)
} CENTERLN;

typedef struct {
    double grad;        // 坡度
    cv::Point startP;     // 坡的起点
    cv::Point endP;       // 坡的终点
} SLOPE;

// 检测壕沟和垂直墙的结果
typedef struct {
    int detectPixels;  // 检测到的像素数量
    int detectFrames;  // 连续检测到的帧数
    int x, y;       // 中心点坐标
}DITCH_WALL_INFO;

typedef struct {
    int				wid;
    int				len;
    std::vector<double>			demg;			//ground mean height
    std::vector<int>			demgnum;       //ground points number
    std::vector<double>			demhmin;		//non-ground
    std::vector<double>			demhmax;		//non-ground
    std::vector<int>			demhnum;       //non-ground points number
    std::vector<unsigned char>	lab;           // grid status
    std::vector<double>			groll;
    std::vector<double>			gpitch;
    std::vector<double>			prob;			//probability of the lab
    std::vector<CENTERLN>		centerln;//center line
    TRANS2D			trans;
    bool			dataon;
    SLOPE           slope;
    DITCH_WALL_INFO ditch;
    DITCH_WALL_INFO wall;
} DMAP;

struct CalibParam{
    float pitch;
    float roll;
    float yaw;
    float x;
    float y;
    float z;
};

struct ConfigParam{
    float vehicle_height;
    float vehicle_width;
    float vehicle_length;
    // range image param
    float r_width;
    float r_height;
    float laser_num;
    float max_angle;
    float min_angle;
    float w_res;
    float h_res;

    //range image smooth param
    double base_smooth_error;
    double max_smooth_error;
    double smooth_hori_factor;
    float far_range;
    float mid_range;
    float far_range_neg;

    double pp_vertical_angle_thresh;       // range image 中相邻点高度差很小时，角度的限制。避免regionGrow几乎竖直的墙面
    double pp_height_thresh;       // range image中相邻点【高度差】小于此阈值，可以继续扩展（可通行区域）
    double pp_height_thresh_far;   // (远距离)range image中相邻点【高度差】小于此阈值，可以继续扩展（可通行区域）
    double pp_horizon_dist_thresh; // range image中相邻点高度差较大时，【水平距离】小于此阈值（比较陡），不可继续扩展
    double pp_angle_thresh;        // 约5degree 弧度制 range image中相邻点【坡度】小于此阈值，可继续扩展

    double road_lowerbound;        // -3 // m 可通行区域高度下限  地面高度为0m
    double road_upperbound;        // 3  // m 可通行区域高度上限  地面高度为0m
    double seg_plane_znorm;        // 拟合平面法向量的参数
    int road_region_min_point;     // 100 如果某个平面的点数少于该值，则不是路面（可能是土堆、水泥台的顶面）

    double vertifactor;
    double horizfactor;

    float ogm_width;
    float ogm_height;
    float offset_x;
    float offset_y;
    float ogm_resolution;

    double	pos_minheight;            //  1.0		// 0.6m 正障碍的最小高度
    double  neg_minheight;

    CalibParam front_middle_lidar;
    CalibParam front_left_lidar;
    CalibParam front_right_lidar;
};

void Calculate_Plane(int Points_Total, double *X_Coord, double *Y_Coord, double *Z_Coord,
    int Origin_Flag, double Plane_Eq[4]);
void Calculate_Residuals(double *X, double *Y, double *Z, double Equation[4],
        double *Error, int PointsTotal);

#endif