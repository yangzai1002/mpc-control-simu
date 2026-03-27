#ifndef MACRO_DEFINITION_FILE_H
#define MACRO_DEFINITION_FILE_H
//#define DELL_PLATFORM                      //DELL平台
#define DEBUG_LN_2021_06_29                 //
#define NUVO_6108_PLATFORM                 //NUVO车上平台
#define F1_201_B
//#define DISPLAY_INTENSITY                         //显示回波强度图
//#define USE_CAMERA_ON                             //接入视频图像
#define SENSE_CAM_DRIVER_ON                       //态势感知相机
//#define SAVE_IMG_MAT                      //使用mat格式保存数据
//#define PCL_VIEW_3D                             //PCL点云数据的三维显示
//#define CURRENT_TIME_ZONE 20    //当前时区，北京
//#define CURRENT_TIME_ZONE 19    //当前时区，包头
#define CURRENT_TIME_ZONE  20   //当前时区，阿拉善/重庆
#define INS_ADJUSTMENT          //根据惯导调整位姿关系
#define C_FANS_32_LIDAR         //C_FANS_32_LIDAR 固态激光雷达固态
//#define PANDARXT32_DEVICE_ON      //PandarXT-32激光雷达
#define RADAR2ORGIN 0.94 //车辆坐标距离毫米波雷达

#define SITUATIONAL_ENVIRONMENTAL   //态势感知使能

#ifdef SITUATIONAL_ENVIRONMENTAL
#define RGBD_DISPLAY                    //图像和雷达融合
#define NEGATIVE_OBSTACLE_DETECTION     //负障碍检测使能
#define NEGATIVE_CLIFF_DETECTION        //负障碍悬崖检测
#define POSITIVE_OBS_DISPLAY            //正障碍显示
#define FREE_AREA_DISPLAY               //可通行区域显示
//#define NEG_ZONE_DISABLE                   //负障碍物超出图像边界保留，以防止负障碍物不显示
#endif

//#define ROADPLANE_DISPLAY               //显示道路平面

#define DYNAMIC_PERSON_DETECTION                    //行人检测
#define DYNAMIC_VEHICLE_DETECTION                    //车辆检测
//#define VEHICLE_DETECTION                           //车辆检测
#define TARGET_PUBLISH_DNET               //通过dnet网络发布人员检测目标
#define RADAR_TRACKING                    //基于毫米波雷达的跟踪
//#define RADAR_DATA_PLAY                   //毫米波数据回放功能

//#define DEFAULT_NODE "/dev/pcan2"

#ifdef F1_201_B
#define PERCEPTION_PARAM_FILE "../PerParamFile/F1_201_B_PerParam/F1_201_B_param_file_Perception.xml"
#define PERSON_TRACKING_PARAM_FILE "../PerParamFile/F1_201_B_PerParam/param_person_tracking.xml"
#define NAVI_RECIFICATION_FILE "../PerParamFile/F1_201_B_PerParam/NaviPosition.xml"
#define RADAR_PARAM_FILE "../PerParamFile/F1_201_B_PerParam/radarParam.xml"
#define VEHICLE_TRACKING_PARAM_FILE "../PerParamFile/F1_201_B_PerParam/vehicle_tracking_param.xml"
#endif




//#define SLAM_ENABLED                                //地图重建
//#define NAVIGATION_RECIFICATION
//#define NAVIGATION_RECIFICATION         //根据车体后轴与后GPS天线位置，进行坐标位置变换

//#define ROADMAPLOADING                            //载入全局地图
//#define SLAM_ENABLED                              //显示地图重建结果
//#define MAP_ORIGIN_FIXED                          //车辆位置在地图固定
//#define RNDF_IN_DETAIL                            //显示详细的RNDF路线图
//#define GOOGLE_EARTH_MAP                          //google earth地图显示
//#define MAP_REBUILD_RECORDER                      //地图重建记录

#ifdef SLAM_ENABLED
#define SLAM_INVALID_DIST 5.5                      //车周身6米范围内不重建（根据标定得到）
//#define SLAN_PTS_RADIUS 50.0                      //slam地图重建时，半径范围
//#define SHOW_GREAT_MAP                            //500米×500米
#define SHOW_LOCAL_SLAM_MAP                       //车体局部坐标系(50米×50米）
//#define SHOW_SLAM_MAP                             //大地坐标系(50米×50米）
//#define REPLANNING_MAP                            //重规划用地图
//#define ROADMAPLOADING
#endif




//#define DYNAMIC_PERSON_DETECTION                  //行人检测

#define PERSON_INVALID                            //用于跟踪过程中将行人位置处变成非障碍物
//#define ECHO_INTENSITY_FILTER   /*5.0*/           //将低于此回波强度值的雷达点去掉，默认5
//#define ECHO_INTENSITY_FILTER   150.0               //将低于此回波强度值的雷达点去掉
//#define INTENSITY_COUNT_THRESHOLD 5               //回波强度数量统计阈值
//#define INTENSITY_THRESHOLD   150                 //动态行人检测的回波强度阈值

//#define NAVIGATION_AND_POSITION       //导航定位测试
#ifdef NAVIGATION_AND_POSITION
#define SLAM_ENABLED
#define SHOW_GREAT_MAP                  //500米×500米
#define SHOW_SLAM_MAP
#endif

#endif // MACRO_DEFINITION_FILE_H
