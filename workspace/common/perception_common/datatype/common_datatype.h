#ifndef COMMON_STRUCT
#define COMMON_STRUCT

#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sstream>
#include <unordered_set>

//PCL
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/common/time.h> //fps calculations
#include <pcl/console/parse.h>
// #include <pcl/visualization/boost.h>
// #include <pcl/visualization/mouse_event.h>
#include <pcl/sample_consensus/sac_model_perpendicular_plane.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/common/impl/angles.hpp>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/common/transforms.h>
#include <pcl/io/pcd_io.h>
#include <pcl/filters/passthrough.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/search/impl/search.hpp>

//适应新的pcl
#include <pcl/pcl_base.h>
#include <pcl/common/impl/common.hpp>

//EIGEN
#include <Eigen/Core>
#include <Eigen/Geometry>

//OPENCV
#include <opencv2/video/tracking.hpp>    //当出现KF问题时可以加上
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h> 
//BOOST
#include <boost/filesystem/path.hpp> 
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp>  


struct PointCfans{
    PCL_ADD_POINT4D;
    float intensity;
    int laserid;
    float timeflag;
    float hangle;
    float vangle;
    float range;
    unsigned char mirrorid;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
}EIGEN_ALIGN16;

POINT_CLOUD_REGISTER_POINT_STRUCT(PointCfans,
    (float,x,x)
    (float,y,y)
    (float,z,z)
    (float,intensity,intensity)
    (int, laserid, laserid)
    (float, timeflag, timeflag)
    (float, hangle, hangle)
    (float, vangle, vangle)
    (float, range, range)
    (unsigned char, mirrorid, mirrorid)
)

namespace percep_datatype{

    struct TwodObject
    {
        cv::Rect_<float> rect;
        int label;
        float prob;
    };


    struct TrackPosition{
        double x;
        double y;
    };
    
    struct ColorLut
    {
        cv::Scalar DarkGray = cv::Scalar(169,169,169);
        cv::Scalar Green = cv::Scalar(0,255,0);
        cv::Scalar DimGray = cv::Scalar(105,105,105);
        cv::Scalar White = cv::Scalar(255,255,255);
        cv::Scalar Silver = cv::Scalar(192,192,192);
        cv::Scalar Yellow = cv::Scalar(0,255,255);
        cv::Scalar Cyan = cv::Scalar(255,255,0);
        cv::Scalar Fuchsia = cv::Scalar(255,0,255);
        cv::Scalar Black = cv::Scalar(0,0,0);
        cv::Scalar Red = cv::Scalar(0,0,255);    
    };
    
    /****************结构体**********************/
    struct Position
    {
        double longitude = 0;   //经度
        double latitude = 0;    //纬度
    
        double longitude_ins = 0;   //经度
        double latitude_ins = 0;    //纬度
    
        double longitude_bd = 0;   //经度
        double latitude_bd = 0;    //纬度
    
        double global_x = 0;    //无人车全局坐标X值(北) 北东地坐标系
        double global_y = 0;    //无人车全局坐标Y值(东)
        double global_h = 0;    //无人车全局高程H值(地)
    
        double global_vx = 0;   //无人车全局X轴向速度(北)
        double global_vy = 0;   //无人车全局Y轴向速度(东)
        double global_vz = 0;   //无人车全局H轴向速度(天)
    
        //角度单位统一为角度
        double heading = 0;     //航向
        double pitch = 0;       //俯仰角
        double roll = 0;        //倾斜角
        double heading_rate = 0;    //航向角速率
    
        double global_wx = 0;   //航向角速度
        double global_wy = 0;   //俯仰角速度
        double global_wz = 0;   //倾斜角速度
    
        double mileage = 0;     //里程
    
        double Velocity = 0;    //里程计速度
        double Wheelangle = 0;  //前轮偏角
        double Curvature = 0;   //曲率
    
        int GPSstate = 1;       //GPS差分信号,差分为4
        unsigned int z_zone = 1;         //区号
    
        long curTime = 1;       //当前时间
        float yawCorrect = 0;   //角度校准
    };
    
    
    struct RectPerson
    {
        float x1,y1,z1;                     //左后下角
        float x2,y2,z2;                     //右前上角
    };
    
    struct RangePix{
        float range = 0;
        float index = -1;
    };
    
    struct ClusterArea
    {
        float center_x = 0;
        float center_y = 0;
        float center_z = 0;                 //真实车体局部坐标系
        float x1,y1,z1;                     //左后下角
        float x2,y2,z2;                     //右前上角
        float box_x1,box_y1;                //矩形框离本车最近的角点
        float box_x2,box_y2;
        float radius;
        float width,length,height;          //
        int center_h,center_w;              //图像坐标系
        float average_intensity;            //回波强度
        float area;                         //区域面积
        float volume;                       //区域体积
        int total;                          //点的数量
        int intensity_count;                //统计回波强度较高的雷达点个数
        std::vector<CvPoint3D64f> lidars;        //区域内雷达点云
        RectPerson global_person;           //全局坐标系下的行人位置
        double score;
        double distribution;                //分布情况
        double ZDiffupGround;               //地面高度差
        bool cam_match = false;
    };
    
    struct TrackingTarget
    {
        ClusterArea cube_;
        bool flag = false;
        CvPoint2D32f imgPos;             //当前图像位置
        CvPoint2D32f predImgPos;             //预测图像位置，2020-03-23
        double local_x = 0;
        double local_y = 0;
        double global_x;
        double global_y;
        double global_z;
        float velocity;
        float vx,vy;
        double heading;
        float px,py;        //预测位置
        float refx,refy;    //参考位置
        double global_refx;
        double global_refy;
        float offsetx = 0; 
        float offsety = 0; //偏移位置
        double global_offsetx;
        double global_offsety;
        int tracking_time = 0;  //跟踪次数
        int cam_match_time = 0;
        CvRect rect;
        std::vector<CvPoint3D64f> lidars;//行人的当前点云
        std::vector<double> h_global_x;   //历史轨迹
        std::vector<double> h_global_y;   //历史轨迹
        std::vector<double> h_heading;    //历史朝向
        std::vector<long> h_times;      //时间序列
        std::vector<RectPerson> h_person;           //每个行人目标的历史位置
        bool cur_update = false;            //每个已有跟踪目标只能被匹配一次
    
        cv::KalmanFilter KF;
        int stateNum;
        int measureNum;
        cv::Mat state;
        cv::Mat ProcessNoise;
        bool is_target_;
        int index;
        std::vector<int> occupiedImgPos; //行人目标占据的网格
    //  Mat measurement;
        long curTime;             //当前OpenCV系统时间
    };
    
    
    struct VehicleROI
    {
        Eigen::Vector3f bboxT;
        Eigen::Quaternionf bboxQ;
        Eigen::Vector3f whd;
        pcl::PointXYZI cp;  //质心，所有点云的均值点
        pcl::PointXYZI pcX;
        pcl::PointXYZI pcY;
        pcl::PointXYZI pcZ;
        pcl::PointXYZI bottomBox[4];
        pcl::PointXYZI min_src;
        pcl::PointXYZI max_src;
        pcl::PointXYZI shapeCenter;  //形心
        Eigen::Vector3f eigenValuesPCA; //特征值
        Eigen::Vector3f eigenValuesPCA_XYZ; //包含z向的特征值
        Eigen::Matrix3f eigenVectorsPCA_xyz;//特征向量
        int totalPoints;
        int planeCount;
        double anisotropy;  //各向异性，值越大，表示不同方向的差异越大，对于车，越大越好，(e1-e3)/e1
        double planarity;   //平面性 (e2-e3)/e1
        double linearity;   //线性，对于灯柱等，该值比较大 (e1-e2)/e1
        bool eigenVectorZmax[3];    //三个方向中z方向最大的可能性
        bool eigenZmax;
        int nearSection; //近处部分
        int bottomSection;  //底下部分
        int freeSection;
    };
    
    struct PersonTrackingParam
    {
        double pointcount_thresh;          //有效回波强度的雷达点数
        double max_width_thresh;           //最大宽度差
        double max_length_thresh;          //最大纵向差
        double max_height_thresh;          //最大高度差
        double min_height_thresh;          //最小高度差
        double radius_thresh;              //人员跟踪半径
        double init_width_thresh;          //初始位置的宽度
        double init_height_thresh;         //初始位置的长度
    };
    
    
    struct VehicleTrackingParam
    {
        double safe_velocity;        //安全车速
        double safe_dist;
        double target_velocity;
        float estop_width;
        double radius_vehicle;      //车辆连续跟踪时，感兴趣区域半径
        double width_vehicle_init;  //车辆初始化感兴趣区域宽度
        double height_vehicle_init; //车辆初始化感兴趣区域纵向距离
        int tracking_time;          //跟踪上限
        int continue_fail_time;     //连续初始化失败
    };
    
    struct GridPix{
        int x;
        int y;
        int nground_num = 0;
        std::vector<int> id;
    };
    
    struct TargetSelect{
        int x;
        int y;
        bool flag;
    };
    
    #pragma pack(push,1)
    struct RadarDataNode
    {
        float range;
        float angle;    //angle 0~180
        float velocity;
        float accel;
        float lat_rate;
        float width;
        bool flag=0;
        int dnyprop;
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
        float snr;
    };
    #pragma pack(pop)

    struct RadarDetect{
        float x;
        float y;
        double global_x;
        double global_y;
        int flag;
        bool inital;
        int lost_count;
        int track_count;
        int id;
        float velocity;
        int stop_count;
        float snr;
    };

    struct dynamicTrack//axy
    {
        int left;
        int top;
        int right;
        int bottom;
        int type;   //0 车辆,2行人
        float prob; //概率
        int id;
    };

    struct Param{
        float vehicle_size[6];
        float vehicle_length;
        float vehicle_width;
	    float radar_roi_range;
	    float radar_to_vehicle;
        float perception_minz;
        float perception_maxz;

        float height_diff;
        float height_diff_tracking_person;
        float height_diff_tracking_vehicle;
        int   obstacle_count_thresh;
        float blind_area;
        float blind_area_x;
        float blind_area_height;
        float remove_noise_ratio;
        float remove_noise_height;

        float break_point_th1;
        float break_point_th2;
        float break_line_th;
        float blind_area_obnums;
        float grid_num_th1;
        float grid_num_th2;
        float grid_num_th3;
        float ring_based_th;
        float dilate_range;
        int   dilate_flag;
        float insx_pos;
        float insy_pos;
        float insz_pos;

        double person_pointcount_thresh;
        double person_max_width_thresh;
        double person_max_length_thresh;
        double person_max_height_thresh;
        double person_min_height_thresh;
        double person_radius_thresh;
        double person_init_width_thresh;
        double person_init_height_thresh;

        double vehicle_safe_velocity; 
        double vehicle_safe_dist;
        double vehicle_estop_width;
        double vehicle_target_velocity;
        double radius_vehicle;
        double width_vehicle_init;
        double height_vehicle_init;
        double continue_fail_time;
        double fusion_dist_th;

        double tracking_lost_time;
        double vmin_wsize;
        double vmax_wsize;
        double vmin_hsize;
        double vmax_hsize;
        double vmin_lsize;
        double vmin_pixel_size;
        double vwidth_ref;


        int cam_fusion;
        int aeb;

        bool debug;
    };

    struct TrackingSingleTarget
    {
        float position_x;
        float position_y;
        float position_z;
        float velocity_total;
        char  track_type;    //1 person 2 car 
        char  track_state;  //0:未初始化 1:初始化失败 2：初始化成功 3：正在跟踪 4：目标丢失 5:跟車干預
        float Millimeter_x;   //毫米波找车坐标x
        float Millimeter_y;   //毫米波找车坐标y  200代表没有检测到目标
        float target_speed;   //目标车速
        float Relative_velocity; //与前车相对速度
        int eStopCount;          //紧急停车信号,ADD 2020-05-20
    };
}

#endif
