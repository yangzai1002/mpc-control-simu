#pragma once
#include <opencv2/core.hpp>
#include "perception_fusion_common/common/imu_message.h"
#include "perception_fusion_common/common/lidarvisioncommon.h"


namespace avos {
namespace perception {


enum class RadarObjectClass {
    CAR = 0,
    TRUCK = 1,
    PEDESTRIAN = 2,
    CYCLIST = 3,
    UNKNOWN = 4,
    UNKNOWN_MOVABLE = 5,
    UNKNOWN_UNMOVABLE = 6
};

enum class RadarMeasState {
    DELETED = 0,
    NEW = 1,
    MEASURED = 2,
    PREDICTED = 3,
    DELETED_FOR = 4,
    NEW_FROM_MERGE = 5,
    NOT_CONFIRMED = 6
};

enum class FusionVRStatus{
  VR_FUSION = 0,
  VISION_ONLY = 1,
  RADAR_ONLY = 2,
  NO_VISION_RADAR = 3
};

enum class RadarDynProp {
    MOVING = 0,
    STATIONARY = 1,
    ONCOMING = 2,
    STATIONARY_CANDICATE = 3,
    UNKNOWN = 4,
    CROSSING_STATIONARY = 5,
    CROSSING_MOVING = 6,
    STOPPED = 7
};

// **********************radar-493***********************
enum class TrackStateType {
	TRACK_STAT_DEFAULT 	= 0,
	TRACK_STAT_CUT_IN 	= 1,
	TRACK_STAT_CUT_OUT 	= 2
};



// **********************radar-mrr***********************







struct RadarObject {
    int source;
    int id = 0;
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    float range = 0.0;
    float rangerate = 0.0;
    float lateralrate = 0.0;
    float vxrel = 0.0;
    float vyrel = 0.0;
    double xabs = 0.0; // 根据dr定位计算
    double yabs = 0.0;
    double zabs = 0.0;
    float vxabs = 0; // 根据dr定位计算
    float vyabs = 0; 
    float vzabs = 0; 
    RadarDynProp dyn_prop = RadarDynProp::MOVING;
    double rcs = 0.0;
    
    RadarMeasState meas_state = RadarMeasState::DELETED;
    double prob_of_exist = 0.0;
    int invalid_state = 0;
    int ambig_state = 0;
    TrackStateType track_state = TrackStateType::TRACK_STAT_DEFAULT;

    double dist_long_rms = 0.0;
    double dist_lat_rms = 0.0;
    double vrel_long_rms = 0.0;
    double vrel_lat_rms = 0.0;
    double arel_long_rms = 0.0;
    double arel_lat_rms = 0.0;
    double orientation_rms = 0.0;

    double arel_long = 0.0;
    RadarObjectClass object_class = RadarObjectClass::UNKNOWN;
    double arel_lat = 0.0;
    double orientation_angle = 0.0;
    double length = 0.0;
    double width = 0.0;
    

    int status = 0;
    int rangemode = 0;
    int bridge = 0;

    double confidence = 0.0;
    bool is_static = true;

    // tracking information
    // @brief anchor point, required
    Eigen::Vector3d anchor_point = Eigen::Vector3d(0, 0, 0);
    // @brief center of the boundingbox (cx, cy, cz), required
    Eigen::Vector3d center = Eigen::Vector3d(0, 0, 0);
    // @brief covariance matrix of the center uncertainty, required
    Eigen::Matrix3f center_uncertainty{};
    // @brief track id, required
    int track_id = -1;
    // @brief velocity of the object, required
    Eigen::Vector3f velocity = Eigen::Vector3f(0, 0, 0);
    // @brief covariance matrix of the velocity uncertainty, required
    Eigen::Matrix3f velocity_uncertainty{};
    // @brief if the velocity estimation is converged, true by default
    bool velocity_converged = true;
    // @brief velocity confidence, required
    float velocity_confidence = 1.0f;
    // @brief acceleration of the object, required
    Eigen::Vector3f acceleration = Eigen::Vector3f(0, 0, 0);
    // @brief covariance matrix of the acceleration uncertainty, required
    Eigen::Matrix3f acceleration_uncertainty{};

    // @brief age of the tracked object, required
    double tracking_time = 0.0;
    // @brief timestamp of latest measurement, required
    double latest_tracked_time = 0.0;
    bool matched_vision=false;
    int visionobject_id=0;
    int visionobject_age=0;
    double time_triggered;       // timestamp  seconds
    double time_received;       // timestamp  seconds
    int track_age=0;
    int vision_type=4;
    int fusion_type=255;
    //华为4D毫米波参数
    uint8_t rangeStdDev{0};//目标点径向距离标准差
    uint8_t rangeRateStdDev{0};//目标点径向速度标准差
    uint8_t azimuthStdDev{0};//目标点方位角标准差
    uint8_t elevationStdDev{0};//目标点俯仰角标准差
    uint8_t existStatus{0};//存在可能性0:高存在概率点 1:低存在概率点
    uint8_t falseAlarmProb{0};//虚警概率
    uint8_t heightConfidence{0};//高度置信度 0:高度高置信度点 1:高度低置信度点
    uint8_t attribState{0};//点云属性状态信息
    uint8_t modeFlag{0};//点云属性状态信息0:行车模式,径向距离不需要进行缩放 1:泊车模式,径向距离需要按缩放系数进行缩放


};


struct RadarDataMsg {
  double trigger_stamp;
  double time;
  int source;
  std::string radar_type = "all";
  std::vector<RadarObject> objs;
  void reset() {
    trigger_stamp = 0.0;
    radar_type = "all";
    objs.clear();
  }
};



struct RadarTrackOutput{
  // input
  double trigger_stamp;

  // output
  int trk_id = -1;
  int trk_age = 0; // 每个tracker内部的age
  int trk_guess_age = 0;


  // vehicle coords
  double x_vcs = 0.0; // 目标中心点（毫米波点）
  double y_vcs = 0.0;
  double vx_vcs = 0.0;
  double vy_vcs = 0.0;
  double vx_ground = 0.0;
  double vy_ground = 0.0;
  double heading_vcs = 0.0;

  // dr coords
  double x_dr = 0.0;
  double y_dr = 0.0;
  double vx_dr = 0.0;
  double vy_dr = 0.0;
  double speed_dr = 0.0;
  double heading_dr = 0.0;
  double ax_dr = 0;
  double ay_dr = 0;

	bool sudden_brake = false;
  int radar_source = 0; //毫米波目标来源
};

struct RadarTrackMsg {
  double trigger_stamp;
  int source;
  std::vector<RadarTrackOutput> objs;
  void reset() {
    trigger_stamp = 0.0;
    objs.clear();
  }
};


//毫米波通用结构体
typedef struct RObject {
  // input
  double trigger_stamp;
  int camid = -1;
  cv::Rect2f box2d;

  cv::RotatedRect match_gate; // 仅用于可视化，前后帧跟踪匹配门限

  // output
  int trk_id = -1;
  int trk_age = 0; // 每个tracker内部的age
  int trk_guess_age = 0;
  FusionObjType type = FusionObjType::FUS_UNDEFINED;
  PosSource pos_source;

  double length = 0.0;
  double width = 0.0;
  double height = 0.0;

  // vehicle coords
  double x_vcs = 0.0; // 目标中心点（毫米波点）
  double y_vcs = 0.0;
  double vx_vcs = 0.0;
  double vy_vcs = 0.0;
  double heading_vcs = 0.0;

  // dr coords
  double x_dr = 0.0;
  double y_dr = 0.0;
  double vx_dr = 0.0;
  double vy_dr = 0.0;

  double det_vx_dr = 0.0;
  double det_vy_dr = 0.0;
  double det_vx_vcs = 0.0;
  double det_vy_vcs = 0.0;
  //卡尔曼滤波加速度
  double ax_dr = 0.0;
  double ay_dr = 0.0;

  double ax_ground = 0.0;
  double ay_ground = 0.0;
  //检测帧差加速度
  double det_ax_dr = 0.0;
  double det_ay_dr = 0.0;
  //最小二乘法计算加速度
  double ax_dr_2 = 0.0;
  double ay_dr_2 = 0.0;

  double speed_dr = 0.0;
  double heading_dr = 0.0;

  //radar trk
  double radarx_vcs = 0;//毫米波用检测点
  double radary_vcs = 0;
  bool matched = false;

  float match_gate_dist = 0;
  bool merged_by_vrobject = false;
	bool sudden_brake = false;//突然刹车标志位

  int radar_source = 0; //毫米波目标来源
} RObject;


}
}

