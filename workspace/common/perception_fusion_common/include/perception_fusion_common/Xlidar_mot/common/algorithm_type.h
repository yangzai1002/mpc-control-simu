#pragma once

#include "log/log.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <math.h>
#include <yaml-cpp/yaml.h>
#include "custom_stack.h"
namespace avos {
namespace perception {
namespace xlidarMOT{
 struct AreaType
{
  double left;
  double right;
  double front;
  double back;
  inline bool InArea(double x, double y) {
    return (x > back && x < front && y > right && y < left);
  }
} ;

struct VehicleConfigtype{
  std::string config_file_path;
  float vcs_to_ground_z;
  float car_front;//todo
  std::string InitParam(const std::string& vehile_full_path){
    auto param_ptr = CustomStack::Instance();
    //尝试在proj_cfg中获取
    if(!param_ptr->GetProjectConfigValue<float>("vehicle_interface/body_base_height/",vcs_to_ground_z))
    {
      AINFO<<"Fail to load vcs_to_ground_z from vehicle_interface.proj_cfg now load from yaml:"<<vehile_full_path;
      AINFO<<"To load vehicle param:"<<vehile_full_path;
      //检测文件是否存在
      std::ifstream ifs(vehile_full_path);
      if (!ifs.is_open()) {
        AERROR << "XLidarMultiObjectTracking: opening failed. check if " << vehile_full_path << " exists";
        AERROR<<"File not exists:"<<vehile_full_path;
        vcs_to_ground_z=0.3;
        return "File not exists:"+vehile_full_path;
      }
      ifs.close();
      //-------------------------------
      YAML::Node node2 = YAML::LoadFile(vehile_full_path);
      if (node2.IsNull()) {
        AINFO << "Load vehicle param failed! please check!";
        return "Faild to loadfile:"+vehile_full_path;
      }
      if(node2["vehicle_interface"]){
        if(node2["vehicle_interface"]["body_base_height"]){
          vcs_to_ground_z = node2["vehicle_interface"]["body_base_height"].as<float>();
          AINFO << "vehicle param read  [body_base_height]:"<<vcs_to_ground_z ;  
        }else{
          vcs_to_ground_z=0.3;
          AINFO << "vehicle param fail to read  [vehicle_interface] ,use default 0.3";  
        }
      }else{
        AINFO << "vehicle param fail to read  [vehicle_interface] , please check! ";
        return "Faild to read param  [vehicle_interface] in file:"+vehile_full_path;
      }
      return "";
    }else{
      AINFO<<"Load vcs_to_ground_z from vehicle_interface.proj_cfg value:"<<vcs_to_ground_z;
      return "";
    }
  }
};

 struct TrackerConfigType
{
  double P0_0 ;
  double P0_1 ;
  double P0_2 ;
  double P0_3 ;
  double P0_4 ;
  double P0_5 ;

}   ;


 struct FusionTrackConfigType
{
  bool enable_fusion = true;
  bool enable_logging = false;
  bool use_basemap = false;
  int debug_target_follow=0; //0: not debug; 1: follow; 
  bool debug_target_vision=false; //
  bool debug_smog_road=false; //
  std::string mapinfo_topic_name="/null"; //
  bool steady_size=false;
  bool vision_person_filter =false;
  bool only_simple_cluster =false;
  //target_finder 使用 add by lxd
  int perception_mode = -1; 
  // add by hhl reid feature
  bool enable_reid_dis_cal = false;
  bool enable_reid_dynamic_update = false;
  bool force_reid_filter = false;
  int reid_match_mode = 0;
  bool reid_range_search = false;
  bool multi_cam_reid = false;
  bool lidar_proj_reid = false;
  double lidar_cost_max_PedestrianTargetMode = 0.0;
  double lidar_cost_max_VehicleTargetMode = 0.0;
  double vision_cost_max_PedestrianTargetMode = 0.0;
  double vision_cost_max_VehicleTargetMode = 0.0;
  double matched_cost = 0.0;
  double pedestrain_range = 0.0;
  double vehicle_range = 0.0;
  double target_matching_gate_pedestrain = 0.0;
  double target_matching_gate_vehicle = 0.0;

  double fusion_frq = 0.0;
  double R_x_lidar = 0.49; //lidar messure noise, m;
  double R_y_lidar = 0.49; //lidar messure noise, m;
  double R_x_radar = 1; //radar messure noise, m;
  double R_y_radar = 1; //radar messure noise, m;
  double R_vx_radar = 0.64; //radar messure noise, m/s;
  double R_vy_radar = 1; //radar messure noise, m/s;
  double R_x_vision_man = 200; //vision messure noise, m;
  double R_y_vision_man = 0.25; //vision messure noise, m;
  double R_x_vision_car = 1.0; //vision messure noise, m;
  double R_y_vision_car = 0.25; //vision messure noise, m;
  double lidar_confi = 0.3;
  double radar_confi = 0.2;
  double vision_confi = 0.2;
  double decay_confi = 0.1;
  double delete_time ;
  double record_length = 25;
  TrackerConfigType base_config;
} ;


 struct CostParamType
{
  double cost_x;
  double cost_y;
  double cost_z;
  double cost_vx;
  double cost_vy;
  double cost_width;
  double cost_length;
  double cost_height;
} ;

 struct GateParamType
{
  double gate_x;
  double gate_y;
  double gate_z;
  double gate_vx;
  double gate_vy;
  double gate_width;
  double gate_length;
  double gate_height;
} ;


 struct CostConfigType {
  CostParamType cost_lidar;
  CostParamType cost_radar;
  CostParamType cost_vision;
  GateParamType gate_lidar;
  GateParamType gate_radar;
  GateParamType gate_vision;
} ;

 struct MatchingConfigType
{
  int matching_methord;
  int cost_option; //0:dummy cost
  CostConfigType cost_param;
} ;

 struct FakeTrackerConfig
{
  double radar_create_x_min;
  double radar_create_x_max;
  double radar_create_y_min;
  double radar_create_y_max;
  double confi_threshold;
  double decay_confi;
  double lidar_confi;
  double radar_confi;
  double vision_confi;
  MatchingConfigType fake_matching;//use matching for fake matching.
  
} ;

//camconfig path
struct CameraConfig{
  std::string config_path{""};
  std::string camera_yaml_name{""};       //相机yaml文件名字

};

struct FuseConfigType
{
  AreaType car_area;
  AreaType lidar_area;
  MatchingConfigType matching_config;
  FusionTrackConfigType fusion_track_config;
  FakeTrackerConfig fake_track_config;
  CameraConfig camera_config;
  VehicleConfigtype vehicle_config;
  bool track_vision_must=false; //必须有视觉方能跟踪,纯激光目标将会快速消失
};

}
}
}

