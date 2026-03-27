#pragma once

#include <opencv2/core.hpp>
#include "pcl/point_cloud.h"
#include "pcl/point_types.h"
#include "log/log.h"
#include "common/time/node_time.h"
#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <Eigen/Core>
#include "perception_fusion_common/common/utils/pcl_types.h"
#include "imu_message.h"
namespace avos {
namespace perception {

const float DEG2RAD = (3.1415926 / 180.0);
const float RAD2DEG = (180.0 / 3.1415926);
// static std::map<std::string,int> CamIdxMap{{"LEFT_FRONT",0},{"RIGHT_FRONT",1},{"FRONT_H120",2}, \
//                                     {"LEFT_BACK",3},{"RIGHT_BACK",4},{"BACK",5},{"FRONT_H120_CROP",6}};


// #define TOP_LEFT 0
// #define TOP_CENTER 1
// #define TOP_RIGHT 2
// #define CENTER_LEFT 3
// #define CENTER 4
// #define CENTER_RIGHT 5
// #define BOTTOM_LEFT 6
// #define BOTTOM_CENTER 7
// #define BOTTOM_RIGHT 8


#define VISION_SOURCE 0x0000
#define LIDAR_SOURCE 0x0100
#define RADAR_SOURCE 0x0200
#define ARS_0_SOURCE 0x0201
#define ARS_1_SOURCE 0x0202
#define HuaWeiFR671 0x0203
#define DOGM_SOURCE 0x0300
#define MONO_SOURCE 0x0400
#define SOURCE_MASK 0xFF00

//*****cam define start*****
#define CAM_LEFT_FRONT 1
#define CAM_RIGHT_FRONT 2
#define CAM_FRONT_H100 0
#define CAM_LEFT_REAR 3
#define CAM_RIGHT_REAR 4
#define CAM_REAR 5
#define CAM_MAX_NUM 6 //表示感知（目标检测，语义分割）使用视觉模型相机数量，获取相机检测结果
                      //融合专用相机数目 包含[CAM_LEFT_FRONT,CAM_RIGHT_FRONT,CAM_FRONT_H100，CAM_LEFT_BACK，CAM_RIGHT_BACK，CAM_BACK]
#define CAM_FRONT_H120_CROP 6 //TODO:delete 暂时不删融合代码还有依赖
#define CAM_FRONT_H30 7  //红绿灯模型专用


// new rules
#define SEG_BACKGROUND 0     // background
#define SEG_ROAD 1           // road 可通行道路
#define SEG_CURB 2           // curb+fence+wall 路缘石+围栏+墙壁
#define SEG_ROADBLOCK 3      // road_block+guide_board 路障
#define SEG_TRAFFICSIGN 4    // traffic_sign
#define SEG_CAR 5            // car
#define SEG_PEDCYC 6         // pedstrain,cyclist
#define SEG_EGOCAR 7         // ego car  自车
#define SEG_DEFLECTOR 8      // Deflector area 导流区
#define SEG_ZEBRA 9          // zebra crossing 人行横道
#define SEG_BIKE 10          // 无人骑的自行车
#define RESA_BACKGROUND 0    // background
#define RESA_POLE 1          // pole 杆子
#define RESA_LANE_TYPE_0 2   // lane type 0 实线
#define RESA_LANE_TYPE_1 3   // lane type 1 虚线
#define RESA_LANE_STOPLINE 4 // stopline 停止线

#define SEGMENT_LABEL_NUM 10
//dl_vision中相机名称转融合序号                                                            
const std::map<std::string,int> CAMERA_NAME_TO_ID = {{"front_h100",CAM_FRONT_H100},
                    // {"front_h120_crop",CAM_FRONT_H120_CROP},
                    {"left_front",CAM_LEFT_FRONT},
                    {"right_front",CAM_RIGHT_FRONT},
                    {"left_back",CAM_LEFT_REAR},
                    {"right_back",CAM_RIGHT_REAR},
                    {"back",CAM_REAR}};
enum class PosSource {
  VISION = 0,
  RADAR = 1,
  LIDAR = 2,
  VSEG = 3,
  VISION_RADAR = 4,
  VISION_LIDAR = 5,
  UNDEFINED = 20
};
enum class FusionObjType {
  FUS_UNDEFINED = 0,

  FUS_CAR = 1,
  FUS_BUS = 3,
  FUS_TRUCK = 4,
  FUS_PEDESTRIAN = 7,
  FUS_CYCLIST = 8, // 人骑两轮车，包括自行车和摩托车
  FUS_BICYCLE = 11, // 无人骑的停着的两轮车
  FUS_TRICYCLE = 13, // 三轮车，不区分有没有人

  FUS_CONE = 16, // 锥筒
  FUS_BARREL = 17, // 其他静态障碍物如水马等
  FUS_TRIANGLE = 18, // 三角牌
  FUS_TrashCan = 19, // 垃圾箱
  FUS_ROCK = 20, // 石墩
  FUS_ISOPILE = 21, // 隔离柱
  FUS_BIANDUI = 22 //编队目标
};
    #define VISION_PEOPLE         0   // 人
    #define VISION_RIDER          1   // 骑行者
    #define VISION_CAR            2   // 乘用车
    #define VISION_BUS            3   // 公交车
    #define VISION_TRUCK          4   // 货车
    #define VISION_CROWD          5   // 人群
    #define VISION_TRICYCLE       6   // 三轮车
    #define VISION_BIKE           7   // 两轮车
    #define VISION_TRICYCLE_RIDER 8   // 人骑机动三轮车
    #define VISION_BIKE_RIDER     9   // 人骑摩托车
    #define VISION_SUBULATE       10  // 锥形筒
    #define VISION_TRIANGLE       11  // 三角牌
    #define VISION_ROADBLOCK      12  // 路障
    #define VISION_OTHER          13
    #define VISION_WHEEL          14  // 车轮
    #define VISION_WHEEL_POINT    15  // 车轮接地点
    #define PERMISSION_ARROW      16  // 允许通行<左右转、直行、等>，对下游输出，合并后类型为arrow
    #define SPEED_LIMIT           17  // 限速，合并到19
    #define FORBISSANCE_ARROW     18  // 禁止通行<左右转、直行、等>，合并到16
    #define WORD                  19  // 普通文字，对下游输出
    #define NOTICE                20  // 人行横道、减速慢行<倒三角>，合并到16
    #define ISOLATION_PILE        21  // 隔离桩
    #define SHIDUN                22  // 石墩
    #define VISION_BIANDUI        100 //编队目标
const std::unordered_map<int, FusionObjType> Cam2FusionTypeMap = {
  {VISION_PEOPLE,         FusionObjType::FUS_PEDESTRIAN},
  {VISION_RIDER,          FusionObjType::FUS_CYCLIST},
  {VISION_CAR,            FusionObjType::FUS_CAR},
  {VISION_BUS,            FusionObjType::FUS_BUS},
  {VISION_TRUCK,          FusionObjType::FUS_TRUCK},
  {VISION_CROWD,          FusionObjType::FUS_PEDESTRIAN},
  {VISION_TRICYCLE,       FusionObjType::FUS_CYCLIST},
  {VISION_BIKE,           FusionObjType::FUS_BICYCLE},
  {VISION_TRICYCLE_RIDER, FusionObjType::FUS_CYCLIST},
  {VISION_BIKE_RIDER,     FusionObjType::FUS_CYCLIST},
  {VISION_SUBULATE,       FusionObjType::FUS_CONE},
  {VISION_TRIANGLE,       FusionObjType::FUS_TRIANGLE},
  {VISION_ROADBLOCK,      FusionObjType::FUS_ROCK},
  {VISION_OTHER,          FusionObjType::FUS_UNDEFINED},
  {VISION_WHEEL,          FusionObjType::FUS_UNDEFINED},
  {VISION_WHEEL_POINT,          FusionObjType::FUS_UNDEFINED},
  {PERMISSION_ARROW,          FusionObjType::FUS_UNDEFINED},
  {SPEED_LIMIT,          FusionObjType::FUS_UNDEFINED},
  {FORBISSANCE_ARROW,          FusionObjType::FUS_UNDEFINED},
  {WORD,          FusionObjType::FUS_UNDEFINED},
  {NOTICE,          FusionObjType::FUS_UNDEFINED},
  {ISOLATION_PILE,          FusionObjType::FUS_UNDEFINED},
  {SHIDUN,          FusionObjType::FUS_UNDEFINED},
  {VISION_BIANDUI,  FusionObjType::FUS_BIANDUI}
};



struct ObstacleObject {
  size_t id = 0;
  size_t age = 0;
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double width = 0.0;
  double length = 0.0;
  double height = 0.0;
  double heading = 0.0; // deg
  size_t type = 0; // 0:car/bus/truck 2:ped 3:cyclist
  double confi = 0.0;
};

struct NodeLidarObjects {
  double trigger_stamp = 0.0;
  std::vector<ObstacleObject> objs;
};

//传感器安装位置，相对后轴中心
struct SensorOffsetInfo{
  double x_offset = 5.5;
  double y_offset = 1.2;
  double z_offset = 1.1;
};


typedef struct ClusterCloudInImage {
  cv::Point3f point; // img x,y,label;
  int cam_idx = -1;
} ClusterCloudInImage;
typedef struct Idx_lidar2cam{
  int cam_idx = -1; //相机编号
  int pos_idx = -1; //点云序号
}Idx_lidar2cam;

//车辆基础信息
struct EgoCarBaseInfo{
  double car_length=0; //车长
  double car_width=0;  //车宽
  double front_rear_axle_length=0;//前后轴距离
  double front_axle_tofront=0;//前轴到车头保险杠距离
  double rear_axle_torear=0;// 后轴到车尾保险杠距离
};


typedef struct ForegroundCloudsType {
  double trigger_stamp = 0.0;
  avos::perception::pcl_util::PointCloudPtr cloudPtr;
  std::vector<size_t> foreground_indices;
  std::vector<size_t> ground_indices;
  void reset() {
    trigger_stamp = 0.0;
    cloudPtr = nullptr;
    foreground_indices.clear();
    ground_indices.clear();
  }
} ForegroundCloudsType;
typedef struct PointLabels {
  int total_label = 0;
  int out_bounding_label = 0;//出了图像边界的点
  int background = 0;
  int road_curb = 0;
  int fence = 0;
  int roadblock = 0;
  int roaduser_car = 0;
  int roaduser_other = 0;
} PointLabels;

typedef struct Lidar2CamConfig
{
 bool show_image = false;
 bool enable_logging = false;
 bool _high_way = false;
 bool _enable_h120_crop = true;
 bool _enable_2d_object = true;
 bool _enable_vision_tracker = false;
 float object_2d_output_range = 100;
 cv::Matx33d _front_radar_tf; 
}Lidar2CamConfig;


typedef struct OccupyCell {
  uint32_t idc = 0;
  double x = 0.0;
  double y = 0.0;
  double xg = 0.0;
  double yg = 0.0;
  double zg = 0.0;
  float x_ocs = 0; // 障碍物坐标系
  float y_ocs = 0;
} OccupyCell;


//*************************************


inline bool IsCarOrTruck(const FusionObjType &type) {
  if (type == FusionObjType::FUS_CAR
      || type == FusionObjType::FUS_TRUCK||
      type == FusionObjType::FUS_BUS) {
    return true;
  }
  return false;
}
inline bool IsPedestrainOrCyclist(const FusionObjType &type) {
  if (type == FusionObjType::FUS_PEDESTRIAN
      || type == FusionObjType::FUS_CYCLIST
      || type == FusionObjType::FUS_BICYCLE
      || type == FusionObjType::FUS_TRICYCLE) {
    return true;
  }
  return false;
}
inline bool IsCar(const FusionObjType &type) {
  if (type == FusionObjType::FUS_CAR) {
    return true;
  }
  return false;
}
inline bool IsTruck(const FusionObjType &type) {
  if (type == FusionObjType::FUS_TRUCK||
      type == FusionObjType::FUS_BUS) {
    return true;
  }
  return false;
}

inline bool IsPedestrain(const FusionObjType &type) {
  if (type == FusionObjType::FUS_PEDESTRIAN) {
    return true;
  }
  return false;
}
inline bool IsCyclist(const FusionObjType &type) {
  if (type == FusionObjType::FUS_CYCLIST
      || type == FusionObjType::FUS_BICYCLE
      || type == FusionObjType::FUS_TRICYCLE) {
    return true;
  }
  return false;
}
inline bool IsBike(const FusionObjType &type) {
  if (type == FusionObjType::FUS_BICYCLE) {
    return true;
  }
  return false;
}


inline bool IsSubulate(const FusionObjType &type) {
  if (type == FusionObjType::FUS_CONE) {
    return true;
  }
  return false;
}

inline bool IsRoadblock(const FusionObjType &type) {
  if (type == FusionObjType::FUS_BARREL) {
    return true;
  }
  return false;
}

inline bool IsTriangle(const FusionObjType &type) {
  if (type == FusionObjType::FUS_TRIANGLE) {
    return true;
  }
  return false;
}
inline bool IsISOPILE(const FusionObjType &type) {
  if (type == FusionObjType::FUS_ISOPILE) {
    return true;
  }
  return false;
}


inline bool IsUnknown(const FusionObjType &type) {
  if (type == FusionObjType::FUS_UNDEFINED) {
    return true;
  }
  return false;
}
inline bool IsUnknownUnmovable(const FusionObjType &type) {
  if (type == FusionObjType::FUS_UNDEFINED) {
    return true;
  }
  return false;
}
inline bool IsSameType(const FusionObjType &_type1, const FusionObjType &_type2) {
  
  if (_type1 == _type2) {
    return true;
  }
  if (IsCarOrTruck(_type1) && IsCarOrTruck(_type2)) {
    return true;
  }
  return false;
}







typedef struct PointLabel {
  cv::Point3f point;
  cv::Point2f point_img;
  int label = -1;
  int cam_idx = -1;
} CloudLabel;

//*************************************

typedef struct SensorObstacle {
  uint32_t id = 0;
  uint32_t vision_3d_id = -1;//匹配上的视觉3d trk_id
  uint32_t age = 0;
  float x = 0.0; //滤波后的位置
  float y = 0.0;
  float z = 0.0;
  float x_det = 0.0; //原始检测框的位置
  float y_det = 0.0;
  float z_det = 0.0;
  float vxabs = 0.0; // 绝对速度vx
  float vyabs = 0.0; // 绝对速度vy
  float vzabs = 0.0; // 绝对速度vz
  float width = 0.0;
  float length = 0.0;
  float height = 0.0;
  float heading = 0.0;
  FusionObjType type = FusionObjType::FUS_UNDEFINED;
  float confi = 0.0;
  cv::Rect2f box2d;
  int box2d_in_cam_index = -1;
    bool crop_h30 = false;
  bool is3d = false;
  bool blocked = false;
  bool _is_fixed_by_cloud = false;
  bool source_cluster = false;
  bool source_vision = false;
  bool source_lidar = false;
  std::vector<cv::Point3f> seg_ground_pts;
  std::vector<cv::Point2f> seg_img_pts;
    //车头车尾框
  int htflag = -1; // -1:none 0:head 1:tail
  cv::Rect2f htbox2d; // head or tail box2d
  std::vector<cv::Point3f> cloud_index;
  std::vector<OccupyCell> cells;
  PointLabels plabel;
  int filter_method = 0;
  bool is_noise = false;
} SensorObstacle;


typedef struct ClusterPixal {
  int cluster_id = -1;
  std::vector<int> indices;
  float rect_area = 0.0;
  float area_ratio = 0.0;
} ClusterPixal;


typedef struct LidarCamObject {
  int id = 0;
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  float width = 0.0;
  float length = 0.0;
  float height = 0.0;
  float heading = 0.0;
  bool is_noise = false;
  int filter_method = 0;
  std::vector<OccupyCell> cells;
  PointLabels plabel;
  std::vector<int> cloud_indices;
  std::map<int, int> box_indices; // boxIdx, cnt //vision obj
  std::vector<int> lidar_det_indices; // boxIdx, cnt //lidar obj
  int matched_lidar_det = -1;
  int cam_trk_id = -1;
  int cam_trk_age = -1;
  FusionObjType cam_trk_type = FusionObjType::FUS_UNDEFINED;
  cv::Rect2f cam_trk_box2d;
  // tracking params
  double x_min = 0.0;
  double y_min = 0.0;
  double z_min = 0.0;
  double x_utm = 0.0;//最近点
  double y_utm = 0.0;
  double x_utm_center = 0.0;//中心点
  double y_utm_center = 0.0;
  double z_utm_center = 0.0;
  
  float heading_utm = 0.0;
  int match_method = 0;
  bool _is_fixed_by_cloud = false;
  cv::Rect2f box2d;//2d 在图像上的中心点center，或者bottom_center
  cv::Rect2f box2d_crop;
  int box2d_in_cam_index=-1;
} LidarCamObject;

typedef struct ThreadProfile {
  bool valid = true;
  double thread_start_stamp = 0.0;
  double cloud_stamp = 0.0;
  double image_stamp = 0.0;
  std::vector<double> clouds_stamp;
  std::vector<double> images_stamp;
  double location_stamp = 0.0;
  float time_consume_ms = 0.0;
  float remove_ground_ms = 0.0;
  float lidar_cloud_cluster_ms = 0.0;
  void reset() {
    valid = true;
    thread_start_stamp = 0.0;
    cloud_stamp = 0.0;
    image_stamp = 0.0;
    time_consume_ms = 0.0;
    remove_ground_ms = 0.0;
    lidar_cloud_cluster_ms = 0.0;
  }
} ModueleProfile;
typedef struct VisionTrackObject {
  int camid;
  cv::Rect box2d;
  int type;
  int trk_id;
  int trk_age;
  int trk_guess_age;
  bool blocked;
}VisionTrackObject;
typedef struct SensorDataType {
  int source = LIDAR_SOURCE;
  double trigger_stamp = 0.0;
  std::vector<SensorObstacle> objs;
  std::vector<LidarCamObject> _clusters;
  std::vector<PointLabel> labels;
  std::vector<PointLabel> labels_crop;
  // std::vector<VisionTrackObject> vision_3d_det_objs;
  std::vector<VisionTrackObject> vision_2d_det_objs;
  ModueleProfile profile;
  // ModueleProfile sync_data_profile;
  //pcl::PointCloud<pcl::PointXYZ>::Ptr clouds;
  void reset() {
    //clouds = nullptr;
    source = LIDAR_SOURCE;
    trigger_stamp = 0.0;
    objs.clear();
    labels.clear();
    labels_crop.clear();
    _clusters.clear();
  }
} SensorDataType;


const int STATE_NEW = 0;
const int STATE_ESTB = 1;


const int MM_INVALID = 0;
const int MM_LIDAR_ID = 1;
const int MM_CAM_ID = 5;
const int MM_CLUSTER_ID = 6;
const int MM_GNN = 2;
const int MM_NEW_CLUSTER = 3;
const unsigned MM_DOGM = 4;

















enum ErrorCode
{
  CLOUD_GET_ERROR = -1,
  CLOUD_DELAY_ERROR = -2,
  CLOUD_SIZE_ERROR = -3,
  LIDAR_TARGET_GET_ERROR = -4,
  FRONT_VISION_GET_ERROR = -5,
  FRONT_VISION_DELAY_ERROR = -6,
  FRONT_CROP_VISION_GET_ERROR = -7,
  FRONT_CROP_VISION_DELAY_ERROR = -8,
  FRONT_VISIONTRACKER_DATA_ERROR = -9,
  LOCATION_DR_GET_ERROR = -10,
  LOCATION_GLOBAL_GET_ERROR = -11,
  LOCATION_DR_DELAY_ERROR = -12,
  LOCATION_DR_VALID_ERROR = -13,
  LOCATION_GLOBAL_DELAY_ERROR = -14,
  LIDAR2CAMTHREAD_GET_ERROR = -15,
  LIDAR2CAMTHREAD_DELAY_ERROR = -16,
  RADAR_GET_ERROR = -17,
  RADAR_DELAY_ERROR = -18,
  LIDAR_CLUSTER_OBJ_GET_ERROR= -19,
  LIDAR_CLUSTER_OBJ_GET_DELAY_ERROR = -20,
  VISION_TRACK_OBJ_3D_GET_ERROR = -21,
  VISION_TRACK_OBJ_3D_GET_DELAY_ERROR = -22,
  VISION_TRACK_OBJ_2D_GET_ERROR = -23,
  VISION_TRACK_OBJ_2D_GET_DELAY_ERROR = -24,

};
typedef struct FusionError
{
  double time;
  ErrorCode _error;
}FusionError;


inline bool IsVerticalBox(const float &heading_vcs_deg) {
  if ((heading_vcs_deg >= -60 && heading_vcs_deg <= 60)
      || (heading_vcs_deg >= -180 && heading_vcs_deg <= -120)
      || (heading_vcs_deg >= 120 && heading_vcs_deg <= 180)) {
    return true;
  }
  return false;
};

inline std::vector<cv::Point3f> cvtGeo2CornerPts(cv::Point3f pvcs, float length, float width, float height, float heading) {
  std::vector<Eigen::Vector3f> geo_bbox;
  geo_bbox.resize(8);
  Eigen::Vector3f min_pt(-length / 2.0, -width / 2.0, -height/2.0);
  Eigen::Vector3f max_pt(length / 2.0, width / 2.0, height/2.0);
  geo_bbox[0] = Eigen::Vector3f(min_pt[0], min_pt[1], min_pt[2]);
  geo_bbox[1] = Eigen::Vector3f(min_pt[0], min_pt[1], max_pt[2]);
  geo_bbox[2] = Eigen::Vector3f(max_pt[0], min_pt[1], min_pt[2]);
  geo_bbox[3] = Eigen::Vector3f(max_pt[0], min_pt[1], max_pt[2]);
  geo_bbox[4] = Eigen::Vector3f(max_pt[0], max_pt[1], min_pt[2]);
  geo_bbox[5] = Eigen::Vector3f(max_pt[0], max_pt[1], max_pt[2]);
  geo_bbox[6] = Eigen::Vector3f(min_pt[0], max_pt[1], min_pt[2]);
  geo_bbox[7] = Eigen::Vector3f(min_pt[0], max_pt[1], max_pt[2]);
  // transform geometry bbox from body frame to sensor frame
  Eigen::Matrix3f project =
    Eigen::AngleAxis<float>((heading * 3.1415926 / 180.0),
                            Eigen::Matrix<float, 3, 1>::UnitZ()).matrix();
  std::vector<cv::Point3f> output;
  for (size_t i = 0; i < geo_bbox.size(); ++i) {
    geo_bbox[i] = project * geo_bbox[i];
    float x = geo_bbox[i][0] + pvcs.x;
    float y = geo_bbox[i][1] + pvcs.y;
    float z = geo_bbox[i][2] + pvcs.z;
    output.push_back(cv::Point3f(x,y,z));
  }
  return output;
}



inline std::vector<cv::Point2f> cvtGeo2CornerPts1(cv::Point3f pvcs, float length, float width, float height, float heading) {
  std::vector<Eigen::Vector3f> geo_bbox;
  geo_bbox.resize(4);
  Eigen::Vector3f min_pt(-length / 2.0, -width / 2.0, 0);
  Eigen::Vector3f max_pt(length / 2.0, width / 2.0, height);
  geo_bbox[0] = Eigen::Vector3f(min_pt[0], min_pt[1], min_pt[2]);
  geo_bbox[1] = Eigen::Vector3f(max_pt[0], min_pt[1], min_pt[2]);
  geo_bbox[2] = Eigen::Vector3f(max_pt[0], max_pt[1], min_pt[2]);
  geo_bbox[3] = Eigen::Vector3f(min_pt[0], max_pt[1], min_pt[2]);
  // transform geometry bbox from body frame to sensor frame
  Eigen::Matrix3f project =
    Eigen::AngleAxis<float>((heading * 3.1415926 / 180.0),
                            Eigen::Matrix<float, 3, 1>::UnitZ()).matrix();
  std::vector<cv::Point2f> output;
  for (size_t i = 0; i < geo_bbox.size(); ++i) {
    geo_bbox[i] = project * geo_bbox[i];
    float x = geo_bbox[i][0] + pvcs.x;
    float y = geo_bbox[i][1] + pvcs.y;
    output.push_back(cv::Point2f(x,y));
  }
  return output;
}

inline cv::Rect2f get2dBox(std::vector<cv::Point2f> &pims) {
  float xmax=-1e9, xmin=1e9, ymax=-1e9, ymin=1e9;
  for(auto &p: pims) {
    if(p.x > xmax) xmax = p.x;
    if(p.x < xmin) xmin = p.x;
    if(p.y > ymax) ymax = p.y;
    if(p.y < ymin) ymin = p.y;
  }
  if(xmin < 0) xmin = 0;
  if(xmax >= 640) xmax = 640-1;
  if(ymin < 0) ymin = 0;
  if(ymax >= 320) ymax = 320-1;
  return cv::Rect2f(cv::Point2f(xmin,ymin), cv::Point2f(xmax,ymax));
};


inline bool CheckPointInBBOX1(const cv::Point2f & pt,
                      const std::vector<cv::Point2f> &bbox) {
  if (bbox.size() == 0) return false;
  int counter = 0;
  int i;
  double xinters;
  cv::Point2f p1;
  cv::Point2f p2;
  // use top 4 or bottom 4 points
  int N = bbox.size();
  N = std::min(4, N);
  p1 = bbox.at(0);
  for (i = 1; i <= N; i++) {
    p2 = bbox[i % N];
    if (pt.y > std::min<float>(p1.y, p2.y)) {
      if (pt.y <= std::max<float>(p1.y, p2.y)) {
        if (pt.x <= std::max<float>(p1.x, p2.x)) {
          if (p1.y != p2.y) {
            xinters = (pt.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
            if (p1.x == p2.x || pt.x <= xinters) counter++;
          }
        }
      }
    }
    p1 = p2;
  }
  if (counter % 2 == 0)
    return false;
  else
    return true;
}





}  // namespace perception
}  // namespace avos
