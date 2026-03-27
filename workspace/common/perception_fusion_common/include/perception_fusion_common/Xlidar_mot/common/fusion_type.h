#pragma once

#include <queue>
#include <string>
#include <vector>
#include "pcl_types.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core.hpp>

namespace avos {
namespace perception {
namespace xlidarMOT{
#define ME_INDEX 0
#define DENSO_INDEX 1
#define IBEO_RAW_INDEX 2
#define LIDAR_INDEX 3
#define ESR_INDEX 4
#define ARS_INDEX_0 5
#define RSDS_REAR_INDEX 6
#define RSDS_REAR_INDEX_R 7
#define ARS_INDEX_1 8

#define FUSE_POINT_CENTER 0
#define FUSE_POINT_BOTTOM 1
#define FUSE_POINT_TOP 2

#define SOURCE_LIDAR_ONLY 0
#define SOURCE_RADAR_ONLY 1
#define SOURCE_VISION_ONLY 2
#define SOURCE_LIDAR_VISION 3
#define SOURCE_LIDAR_RADAR 4
#define SOURCE_LIDAR_VISION_RADAR 5
#define SOURCE_PANO_VISION_ONLY 6
#define SOURCE_NONE 7
#define SOURCE_RADAR_VISION 8

#define TOP_LEFT 0
#define TOP_CENTER 1
#define TOP_RIGHT 2
#define CENTER_LEFT 3
#define CENTER 4
#define CENTER_RIGHT 5
#define BOTTOM_LEFT 6
#define BOTTOM_CENTER 7
#define BOTTOM_RIGHT 8

#define VELOCITY_STATUS_BAD 0
#define VELOCITY_STATUS_MEDIUM 1
#define VELOCITY_STATUS_GOOD 2
#define VELOCITY_STATUS_INVALID 9

const float DEG2RAD = (3.1415926 / 180.0);
const float RAD2DEG = (180.0 / 3.1415926);

struct GeoPoint {
  double x;
  double y;
  float vxrel;
  float vyrel;
  bool isvalid;
  GeoPoint() {
    x = 0;
    y = 0;
    vxrel = 0;
    vyrel = 0;
    isvalid = false;
  }
};

struct GeoBBox {
  double timestamp;
  std::vector<GeoPoint> geoPoints;
  GeoPoint fusion_point;
  GeoBBox() { geoPoints.resize(9); }
  GeoBBox& operator=(const GeoBBox& other) {
    if (this != &other) {
      timestamp = other.timestamp;
      geoPoints = other.geoPoints;
      fusion_point = other.fusion_point;
    }
    return *this;
  }
};

struct VelocityRMS {
  std::deque<float> buffer;
  float rms = 0.0;
  float mean = 0.0;
};

struct OccupyCell_ {
  uint16_t idc;
  uint16_t type;
  float x;
  float y;
  float z;
  double xg;
  double yg;
  double maxh;
  double minh;
  double meanh;
  double stdh;
  double dh;
  OccupyCell_& operator=(const OccupyCell_& other) {
    if (this != &other) {
      idc = other.idc;
      type = other.type;
      x = other.x;
      y = other.y;
      z = other.z;
      xg = other.xg;
      yg = other.yg;
      maxh = other.maxh;
      minh = other.minh;
      meanh = other.meanh;
      stdh = other.stdh;
      dh = other.dh;
    }
    return *this;
  }
};
struct CameraCluster_ {
  int camIdx = -1; // 0-front_h60, 1-left_h90, 2-right_h90
  uint32_t norm_type = 255;
  int orientation_id = -1;
  float heading = 0.0;
  bool track_enable = false;
  uint32_t track_id = 0;
  uint32_t track_age = 0;
  double x = 0;
  double y = 0;
  float vx = 0;
  float vy = 0;
  bool v_valid = false;
  uint32_t pointsNum = 0;
  std::vector<float> feature;//视觉特征
  
  CameraCluster_& operator=(const CameraCluster_& other) {
    if (this != &other) {
      camIdx = other.camIdx;
      norm_type = other.norm_type;
      orientation_id = other.orientation_id;
      heading = other.heading;
      track_enable = other.track_enable;
      track_id = other.track_id;
      track_age = other.track_age;
      x = other.x;
      y = other.y;
      vx = other.vx;
      vy = other.vy;
      v_valid = other.v_valid;
      pointsNum = other.pointsNum;
      feature = other.feature;
    }
    return *this;
  }
};

struct Lidar2Camera_ {
  bool is_valid = false;
  double time_triggered = 0;
  uint32_t totalCamObjs = 0;
  bool is_cluster_obj = false;
  std::vector<CameraCluster_> objs;
  void reset() {
    is_valid = false;
    time_triggered = 0;
    totalCamObjs = 0;
    is_cluster_obj = false;
    objs.clear();
  }

  Lidar2Camera_& operator=(const Lidar2Camera_& other) {
    if (this != &other) {
      is_valid = other.is_valid;
      time_triggered = other.time_triggered;
      totalCamObjs = other.totalCamObjs;
      is_cluster_obj = other.is_cluster_obj;
      objs = other.objs;
    }
    return *this;
  }

} ;

struct Lidar2Semantic_ {
  bool is_valid = false;
  double time_triggered = 0;
  int total_cloud_pts = 0;
  int background_pts = 0; // background, building, traffic sign
  int road_curb_pts = 0; // road, curb
  int fense_pts = 0;      // fense
  int roadblock_pts = 0; // guideboard(on ground), roadblock
  int roaduser_car_pts = 0; // car, truck
  int roaduser_other_pts = 0; // cyclist, pedestrain
  bool isTrueObj = true;
  void reset() {
    is_valid = false;
    time_triggered = 0;
    total_cloud_pts = 0;
    background_pts = 0;
    road_curb_pts = 0;
    fense_pts = 0;
    roadblock_pts = 0;
    roaduser_car_pts = 0;
    roaduser_other_pts = 0;
    isTrueObj = true;
  }
};

struct LidarDynamicDetection_ {
  bool isvalid = false;
  int object_id = 0;
  int track_id = 0;
  int track_age = 0;
  float track_speed = 0.0;
  int track_status = 0;
  int cell_size = 0;
  std::vector<int> track_sta;
  void reset() {
    isvalid = false;
    object_id = 0;
    track_id = 0;
    track_age = 0;
    track_speed = 0.0;
    track_status = 0;
    cell_size = 0;
    track_sta.clear();
  }

  LidarDynamicDetection_& operator=(const LidarDynamicDetection_& other) {
    if (this != &other) {
      isvalid = other.isvalid;
      object_id = other.object_id;
      track_id = other.track_id;
      track_age = other.track_age;
      track_speed = other.track_speed;
      track_status = other.track_status;
      cell_size = other.cell_size;
      track_sta = other.track_sta;
    }
    return *this;
  }

} ;

struct Radar2CamInfo {
  double time_triggered;       // timestamp  seconds
  double time_received;       // timestamp  seconds
  int radar_id = -1;
  int radar_age = 0;
  bool matched_vision = false;
  int visionobject_id = -1;
  int visionobject_age = 0;
  int visionobject_type = 0;
  std::string camera_name = "";
  double x = 0.0;
  double y = 0.0;
  //int radarobject_age=0;
};

struct FusionObject_ {
  int id = 0;      // obstacle ID.
  float x = 0.0;        // relative distance of x, VCS, unit: m
  float y = 0.0;        // relative distance of y, VCS, unit: m
  float z = 0.0;        // relative distance of z, VCS, unit: m
  float vxrel = 0.0;    // relative speed of x, VCS, unit: m/s
  float vyrel = 0.0;    // relative speed of y, VCS, unit: m/s
  double xabs = 0.0;    // relative distance of x, VCS, unit: m
  double yabs = 0.0;    // relative distance of y, VCS, unit: m
  float vxabs = 0.0;    // relative speed of x, VCS, unit: m/s
  float vyabs = 0.0;    // relative speed of y, VCS, unit: m/s
  float width = 0.0;    // unit: m
  float length = 0.0;   // unit: m
  float height = 0.0;   // unit: m
  float speed = 0.0;    // unit: m/s
  float heading = 0.0;  // unit: du
  float dr_heading = 0.0;
  float confidence = 0.0;
  int type = 4;
  uint32_t age = 0;
  uint16_t velocitystatus = VELOCITY_STATUS_INVALID;
  int source;
  std::vector<OccupyCell_> cells;
  Lidar2Camera_ l2cInfo;
  Lidar2Semantic_ lidar2semantic;
  LidarDynamicDetection_ ld_record;
  Radar2CamInfo  radar2caminfo_;
  std::vector<int> det_in_img={0,0,0}; // add by hhl 顺序代表三个相机front_h60 left_h90 right_h90
  //by lxd for lidarProjectToImg
  float img_box_x = 0.0;
  float img_box_y = 0.0;
  float img_box_w = 0.0;
  float img_box_h = 0.0;
  int img_width = 0;
  int img_height = 0;
  int img_camId = -1;
  // by hhl 
  bool is_bl_cluster = false;
  
  FusionObject_& operator=(const FusionObject_& other) {
    if (this != &other) {
      id = other.id;
      x = other.x;
      y = other.y;
      z = other.z;
      vxrel = other.vxrel;
      vyrel = other.vyrel;
      xabs = other.xabs;
      yabs = other.yabs;
      vxabs = other.vxabs;
      vyabs = other.vyabs;
      width = other.width;
      length = other.length;
      height = other.height;
      speed = other.speed;
      heading = other.heading;
      dr_heading = other.dr_heading;
      confidence = other.confidence;
      type = other.type;
      age = other.age;
      velocitystatus = other.velocitystatus;
      source = other.source;
      cells = other.cells;
      l2cInfo = other.l2cInfo;
      lidar2semantic = other.lidar2semantic;
      ld_record = other.ld_record;
      radar2caminfo_ = other.radar2caminfo_;
      det_in_img = other.det_in_img;
      img_box_x = other.img_box_x;
      img_box_y = other.img_box_y;
      img_box_w = other.img_box_w;
      img_box_h = other.img_box_h;
      img_width = other.img_width;
      img_height = other.img_height;
      img_camId = other.img_camId;
      is_bl_cluster = other.is_bl_cluster;
    }
    return *this;
  }
};


struct TargetObjectDataType_ {
  double time;
  bool is_valid;
  FusionObject_ target;
  int mode; // ped 1 veh 2
  bool is_initing; //
  // float confidence_de;
  TargetObjectDataType_() {
    time = 0;
    is_valid = false;
    is_initing = false;
    mode = 4;
  }
  void Reset() {
    mode = 4;
    time = 0;
    is_valid = false;
    is_initing = false;
    target = FusionObject_();
  }
};


struct GeoInfo {
  bool isvalid = false;
  double distance = 0;
};


struct DorDetection_ {
  double time_sec = 0;
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloudPtr_static;
  void reset() {
    time_sec = 0;
    cloudPtr_static = nullptr;
  }
};



struct ObjectDetection_ {
  double system_time;
  double received_time;
  // vehicle coords
  double x_vcs = 0.0;
  double y_vcs = 0.0;
  double z_vcs = 0.0;
  float vx_vcs = 0.0;
  float vy_vcs = 0.0;
  float heading_vcs = 0.0;
  GeoBBox lidar_bbox_vcs;
  Lidar2Camera_ l2cInfo;
  Lidar2Semantic_ lidar2semantic;
  // dr coords
  double x_dr = 0.0;
  double y_dr = 0.0;
  float vx_dr = 0.0;
  float vy_dr = 0.0;
  float heading_dr = 0.0;
  GeoBBox lidar_bbox_dr;
  bool vision_velo_valid = false;
  // other info
  uint16_t source;
  uint16_t lidar_fake2vision=0;
  int id = 0;
  float length = 0.0;
  float width = 0.0;
  float height = 0.0;
  uint32_t type = 4;
  float lidarConfi = 0.0;
  bool isTrueObj = true;
  GeoInfo geoInfo;
  std::vector<OccupyCell_> cells_vcs;

  bool is_dor_valid = false;
  uint32_t static_points_num = 0;
  bool is_static = false;
  LidarDynamicDetection_ ldd_status;
  std::vector<int> det_in_img={0,0,0}; // add by hhl 顺序代表三个相机front_h60 left_h90
  //by lxd for lidarProjectToImg
  float img_box_x = 0.0;
  float img_box_y = 0.0;
  float img_box_w = 0.0;
  float img_box_h = 0.0;
  int img_width = 0;
  int img_height = 0;
  int img_camId = -1;
    // by hhl 
  bool is_bl_cluster = false;
    // radar info
  Radar2CamInfo radarInfo;

  ObjectDetection_& operator=(const ObjectDetection_& other) {
    if (this != &other) {
      system_time = other.system_time;
      received_time = other.received_time;
      x_vcs = other.x_vcs;
      y_vcs = other.y_vcs;
      z_vcs = other.z_vcs;
      vx_vcs = other.vx_vcs;
      vy_vcs = other.vy_vcs;
      heading_vcs = other.heading_vcs;
      lidar_bbox_vcs = other.lidar_bbox_vcs;
      l2cInfo = other.l2cInfo;
      lidar2semantic = other.lidar2semantic;
      x_dr = other.x_dr;
      y_dr = other.y_dr;
      vx_dr = other.vx_dr;
      vy_dr = other.vy_dr;
      heading_dr = other.heading_dr;
      lidar_bbox_dr = other.lidar_bbox_dr;
      vision_velo_valid = other.vision_velo_valid;
      source = other.source;
      lidar_fake2vision = other.lidar_fake2vision;
      id = other.id;
      length = other.length;
      width = other.width;
      height = other.height;
      type = other.type;
      lidarConfi = other.lidarConfi;
      isTrueObj = other.isTrueObj;
      geoInfo = other.geoInfo;
      cells_vcs = other.cells_vcs;
      is_dor_valid = other.is_dor_valid;
      static_points_num = other.static_points_num;
      is_static = other.is_static;
      ldd_status = other.ldd_status;
      det_in_img = other.det_in_img;
      img_box_x = other.img_box_x;
      img_box_y = other.img_box_y;
      img_box_w = other.img_box_w;
      img_box_h = other.img_box_h;
      img_width = other.img_width;
      img_height = other.img_height;
      img_camId = other.img_camId;
      is_bl_cluster = other.is_bl_cluster;
      radarInfo = other.radarInfo;
    }
    return *this;
  }
};

struct ObjectTrack {
  double system_time;
  double lidar_received_time;
  int id = 0;
  uint32_t age = 0;

  double x = 0;
  double y = 0;
  float vx = 0;
  float vy = 0;
  float heading = 0;
  float length = 0;
  float width = 0;
  float height = 0;

  float length_predict = 0;
  float width_predict = 0;

  int type = 4;
  bool is_type_sure = false;
  int type_confidence = 0;

  float confidence = 0;

  bool is_top_bottom_reversed = false;
  int fuse_point = FUSE_POINT_CENTER;
  int fuse_source = SOURCE_NONE;
  Lidar2Camera_ l2cInfo;
  Lidar2Semantic_ Lidar2Semantic;
  bool is_l2c_updated = false;
  std::vector<OccupyCell_> cells;
  bool updated = false;
  uint16_t coast_count = 0;

  std::deque<GeoBBox> bbox_dr_buffer;
  float vx_bbox_dr = 0;
  float vy_bbox_dr = 0;

  double x_center_vcs = 0;
  double y_center_vcs = 0;
  double z_center_vcs = 0;
  float heading_vcs = 0;
  float vx_vcs = 0;
  float vy_vcs = 0;
  std::string heading_source = "lidar";
  bool heading_has_conflict = false;
  int heading_conflict_cnt = 0;

  VelocityRMS rms_vx;
  VelocityRMS rms_vy;
  VelocityRMS rms_heading;
  uint16_t reset_filter_cnt = 0;
  uint16_t velocitystatus = VELOCITY_STATUS_INVALID;

  int velo_static_cnt = 0;
  int dor_static_cnt = 0;
  int dor_static_points_num = 0;
  bool ldd_static = false;
  bool is_trk_static = false;
  Radar2CamInfo radarInfo;
  std::vector<int> det_in_img={0,0,0}; // add by hhl 顺序代表三个相机front_h60 left_h90
//by lxd for lidarProjectToImg
  float img_box_x = 0.0;
  float img_box_y = 0.0;
  float img_box_w = 0.0;
  float img_box_h = 0.0;
  int img_width = 0;
  int img_height = 0;
  int img_camId = -1;
  uint16_t lidar_fake2vision=0;
    // by hhl 
  bool is_bl_cluster = false;
};


struct ThreadProfile {
  double timestamp_thread_start = 0.0;
  float time_used_lidar = 0.0;
  float time_bak_data = 0.0;
  float time_used_lidar2cam = 0.0;
  bool findSyncImage = false;
  double timestamp_lidar = 0.0;
  double timestamp_vision = 0.0;
  float time_bak_landet = 0.0;
  bool findSyncLandet = false;
  double timestamp_landet = 0.0;
  double timestamp_landet_callback = 0.0;
  bool module_activated = false;
  void reset() {
    timestamp_thread_start = 0.0;
    time_used_lidar = 0.0;
    time_bak_data = 0.0;
    time_used_lidar2cam = 0.0;
    findSyncImage = false;
    timestamp_lidar = 0.0;
    timestamp_vision = 0.0;
    module_activated = false;
    findSyncLandet = false;
    timestamp_landet = 0.0;
    timestamp_landet_callback = 0.0;
  }
};

struct FusionDataType_ {
  double time;
  int source;
  ThreadProfile profile;
  std::vector<FusionObject_> objs;
  std::vector<OccupyCell_> cells;
  DorDetection_ dorDets;
  // Fault Monitor informations
  std::vector<int> lidar_error_codes;
  int vision_error_codes;
  FusionDataType_() { time = 0; }
  void Reset() {
    time = 0;
    profile.reset();
    objs.clear();
    cells.clear();
    dorDets.reset();
  }
};

/// Sensor frame item, for sorting and indexing.
struct DataItem {
  /// Sensor item time stamp, unit:sec.
  double time;
  /// Sensor item's sensor id.
  int sensor_id;
  /// Sensor item's fram id.
  int frame_id;
};

struct follow_task {
  // float time_stamp;
  char follow_type ;  //1-car 2-person 0-error
  // std::vector<float> ahead_distance;  // m  [4,8] 前方4-8m处
  // std::vector<float> LR_distance; // m [-2,5]右侧两米到左侧五米处
  double speed;
  float max_speed;
  // float follow_type;
  char follow_mode;
  float safe_distance;
  int avoid;
  int find_flag;  //1-成功 0-失败
  int id;
  char target_origin;
  double lat;
  double lon;
  FusionObject_ tar_obj;
  // std::vector<float> x_vec ;
  // std::vector<float> y_vec ;  //目标相对坐标
  cv::Mat image;
  bool invalid;
  // std::vector<int> id;
  void reset() {
    id = -1;
    target_origin = 2;
    invalid = true;
    follow_type = 0;
    // ahead_distance = {3,60};
    // LR_distance = {-5,5};
    speed = 1.0;
    max_speed = 2;
    // x_vec.clear();
    // y_vec.clear();
    avoid = 2;
    safe_distance = 0;
    follow_mode = 2;
    lat = 0;
    lon = 0;

  }
};


}
}  // namespace perception
}  // namespace avos
