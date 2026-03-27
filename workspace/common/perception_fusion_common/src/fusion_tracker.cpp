#include "log/log.h"
#include "perception_fusion_common/Xlidar_mot/tracker/fusion_tracker.h"
#include <iostream>

namespace avos {
namespace perception {
namespace xlidarMOT{

FuseConfigType FusionTracker::config_;
bool FusionTracker::inited_;
bool FusionTracker::vision_person_filter_;


FusionTracker::FusionTracker(const ObjectTrack& det): BaseTracker(det) {
  if (!inited_) {
    AINFO  << "Fatal Error! This Init() must called before!";
    exit(-1);
  }
  BaseTracker::Init(config_.fusion_track_config.base_config);
}
FusionTracker::~FusionTracker() {
}

int FusionTracker::Init(FuseConfigType config) {
  config_ = config;
  inited_ = true;
  //add by hhl: vision_person_filter
  vision_person_filter_ = config.fusion_track_config.vision_person_filter;
  return 0;
}

void FusionTracker::PredictFusionTrack() {
  // kalman filter time predict step
  const float time_step = 0.1; //s
  Predict(time_step);
}
void FusionTracker::UpdataImg2DBox(const ObjectDetection_& mtd_det){
  // trk.img_box_x = mtd_det.img_box_x;
  // trk.img_box_y = mtd_det.img_box_y;
  // trk.img_box_w = mtd_det.img_box_w;
  // trk.img_box_h = mtd_det.img_box_h;
  // return;
  //针对行人目标做优化：行人裤子颜色和地面接近时，经常出现只检出上半身的情况，导致输出结果目标框突变
  float wh_r_tk= trk.img_box_w/trk.img_box_h;
  float wh_r_det= mtd_det.img_box_w/mtd_det.img_box_h;
  float dxR = fabs((trk.img_box_x - mtd_det.img_box_x)/trk.img_box_w);
  float dyR = fabs((trk.img_box_y - mtd_det.img_box_y)/trk.img_box_h);
  bool use_upper_anchor=false;
  if(wh_r_tk<0.6 && wh_r_tk>0.25 && dxR<1.5 && dyR<0.5){//跟踪目标比例正常
    if(wh_r_det>=0.6){//新检测目标胖短，如果头部高度接近，说明目标腿部漏检，需要用新锚点进行弥补：
      use_upper_anchor = true;
    }
  }
  // AINFO<<"UpdataImg2DBox: trk type:"<<(trk.type)
  //   <<" x:"<<std::fixed<< std::setprecision(3)<<trk.img_box_x<<" y:"<<trk.img_box_y
  //   <<" w:"<<trk.img_box_w<<" h:"<<trk.img_box_h
  //   <<" det:x:"<<mtd_det.img_box_x<<" y:"<<mtd_det.img_box_y
  //   <<" w:"<<mtd_det.img_box_w<<" h:"<<mtd_det.img_box_h
  //   <<" wh_r_tk:"<<wh_r_tk<<" wh_r_det:"<<wh_r_det
  //   <<" dxR:"<<dxR<<" dyR:"<<dyR<<" use_upper_anchor:"<<use_upper_anchor
  //   ;
  if(use_upper_anchor){
    //上边作为锚点
    trk.img_box_w = trk.img_box_w*0.3 + 0.7*mtd_det.img_box_w; //缓慢更新宽度
    trk.img_box_h =  trk.img_box_w/wh_r_tk;
    trk.img_box_x = mtd_det.img_box_x + 0.5*(mtd_det.img_box_w-trk.img_box_w);
    trk.img_box_y = mtd_det.img_box_y ;
  }else if( fabs(wh_r_tk-wh_r_det)<0.2 && dxR<0.2 && dyR<0.1){
    //中心作为锚点
    trk.img_box_w = trk.img_box_w*0.3 + 0.7*mtd_det.img_box_w;
    trk.img_box_h = trk.img_box_h*0.3 + 0.7*mtd_det.img_box_h;
    trk.img_box_x = mtd_det.img_box_x + 0.5*(mtd_det.img_box_w-trk.img_box_w);
    trk.img_box_y = mtd_det.img_box_y + 0.5*(mtd_det.img_box_h-trk.img_box_h);
  }else{
    trk.img_box_x = mtd_det.img_box_x;
    trk.img_box_y = mtd_det.img_box_y;
    trk.img_box_w = mtd_det.img_box_w;
    trk.img_box_h = mtd_det.img_box_h;
  }

  trk.img_width = mtd_det.img_width;
  trk.img_height = mtd_det.img_height;
  trk.img_camId = mtd_det.img_camId;
  // AINFO<<"UpdataImg2DBox end: trk:"
  //   <<" x:"<<std::fixed<< std::setprecision(3)<<trk.img_box_x<<" y:"<<trk.img_box_y
  //   <<" w:"<<trk.img_box_w<<" h:"<<trk.img_box_h;
}
void FusionTracker::UpdateFusionTrack(const double timestamp,
                                      const PerceptionLocType &locpos) {
  trk.system_time = timestamp;
  if (matched_lidar_dets_.size() > 0) {
    bool isTrueObj = true;
    for (const auto& det : matched_lidar_dets_) {
      float track_heading = NormalizeHeading(trk.heading_vcs);
      float det_heading = NormalizeHeading(det.heading_vcs);
      float d = fabs(track_heading - det_heading);
      if (d > 180) {d -= 180;}
      bool invalid_heading = (d >= 30 && d <= 150);
      bool invalid_width = (fabs(trk.width - det.width) >= 1.0);
      if ((IsCarOrTruck(trk.type) || IsCarOrTruck(det.type))
          && (invalid_width == true || invalid_heading == true)) {
        isTrueObj = false;
      }
    }
    if (isTrueObj == false) {
      matched_lidar_dets_.clear();
    }
  }

  // update track with lidar detection
  for (auto &det : matched_lidar_dets_) {
    // add by hhl 对于is_bl_cluster的进行长宽高比较 中心距离比较
    if( trk.is_bl_cluster && det.is_bl_cluster &&  trk.type == UNKNOWN_TYPE && det.type == UNKNOWN_TYPE ){
        float width_check =  trk.width / det.width ;
        float height_check =  trk.height / det.height ;
        float length_check =  trk.length / det.length ;
        float center_check =  sqrt(pow(( trk.x_center_vcs - det.x_vcs), 2) + pow(( trk.y_center_vcs - det.y_vcs), 2));
        if( width_check<= 0.5 || width_check >=2 || height_check<=0.5 || height_check >= 2 ||length_check<=0.5 || length_check >=2 || center_check >2 ){
            AINFO << "[MOT MANAGER] BL_CLUSTER BOUNCING " <<  trk.id;
            break;
        }
    }
    // add by hhl 对于匹配上的det再次进行距离匹配检验，以防止GNN误匹配
    // float double_center_check =  sqrt(pow(( trk.x_center_vcs - det.x_vcs), 2) + pow(( trk.y_center_vcs - det.y_vcs), 2));
    //     if(IsCarOrTruck(trk.type) && double_center_check > 3){
    //         AINFO << "[FUSION UPDATE] matched dets center dis too large ";
    //         break;
    //     }
    //     if(IsPedestrainOrCyclist(trk.type) && double_center_check > 1.5){
    //         AINFO << "[FUSION UPDATE] matched dets center dis too large ";
    //         break;    
    //     }
    //     if((trk.type == WATERFENCE_TYPE || trk.type == SUBULATE_TYPE || trk.type == ROADBLOCK_TYPE ) && double_center_check > 1.0){
    //         AINFO << "[FUSION UPDATE] matched dets center dis too large ";
    //         break;    
    //     }

    auto prev_fuse_point = trk.fuse_point;
    auto trk_type = trk.type;
    auto trk_heading_vcs = trk.heading_vcs;
    double x = det.x_dr;
    double y = det.y_dr;
    // flip the flag
    bool reset_filter = false;
    bool is_reversed = IsTopBottomReversed(trk_heading_vcs, det.heading_vcs);
    if (is_reversed == true) {
      trk.is_top_bottom_reversed = (trk.is_top_bottom_reversed == true) ? false : true;
    } else {
      if (trk.is_top_bottom_reversed == true &&
          ((trk_heading_vcs >= -60 && trk_heading_vcs <= 60)
           || (trk_heading_vcs >= -180 && trk_heading_vcs <= -120)
           || (trk_heading_vcs >= 120 && trk_heading_vcs <= 180))) {
        trk.is_top_bottom_reversed = false;
        reset_filter = true;
      }
    }
    bbox_.GetAnchorPoint(prev_fuse_point, trk_type, trk_heading_vcs, det, trk.is_top_bottom_reversed,
                         trk.fuse_point, x, y);

    bool is_static_obj = false;
    is_static_obj = ((det.type == SUBULATE_TYPE || trk_type == SUBULATE_TYPE) ||
                     (det.type == SUBULATE_TYPE || trk_type == ROADBLOCK_TYPE) ||
                     (det.type == SUBULATE_TYPE || trk_type == ANTITANK_TYPE) ||
                     (det.type == ROADBLOCK_TYPE || trk_type == SUBULATE_TYPE) ||
                     (det.type == ROADBLOCK_TYPE || trk_type == ROADBLOCK_TYPE) ||
                     (det.type == ROADBLOCK_TYPE || trk_type == ANTITANK_TYPE) ||
                     (det.type == ANTITANK_TYPE || trk_type == SUBULATE_TYPE) ||
                     (det.type == ANTITANK_TYPE || trk_type == ROADBLOCK_TYPE) ||
                     (det.type == ANTITANK_TYPE || trk_type == ANTITANK_TYPE));//modify by lxd
    if (prev_fuse_point != trk.fuse_point
        || (trk.velocitystatus == VELOCITY_STATUS_BAD)
        || is_static_obj == true
        || reset_filter == true
        || trk.is_bl_cluster == true) { //add by hhl 盲区聚类结果采用位置估计速度
      trk.x = x;
      trk.y = y;
      trk.vx = trk.vx_bbox_dr;
      trk.vy = trk.vy_bbox_dr;
      if (is_static_obj == true) {
        trk.vx = 0;
        trk.vy = 0;
      }
      InitFilterParams();
      // clear fusion point buffer
      for (auto &it : trk.bbox_dr_buffer) {
        it.fusion_point.isvalid = false;
      }
      if (trk.velocitystatus == VELOCITY_STATUS_BAD) {
        trk.reset_filter_cnt = 0;
      }
    } else {
      //kalman update:
      MyVector R(Num_Measure, MAX_R);
      MyVector Z(Num_Measure, 0.0);
      Z(4) = x;
      Z(5) = y;
      R(4) = config_.fusion_track_config.R_x_lidar;
      R(5) = config_.fusion_track_config.R_y_lidar;
      MessureUpdate(R, Z);
    }
    trk.x_center_vcs = det.x_vcs;
    trk.y_center_vcs = det.y_vcs;
    trk.heading_vcs = det.heading_vcs;
    trk.height = det.height;
    trk.cells = det.cells_vcs;
    // 2021-06-22: process multi corrlation
    UpdateLidar2CameraInfo(det);
    UpdateObjectType(det);
    if(trk.is_bl_cluster){
        UpdateObjectDimension_SPC(det,locpos);
    }else{
        UpdateObjectDimension(det, locpos);
    }
    bbox_.UpdateBBoxBuffer(trk, det);
    trk.updated = true;
    trk.coast_count = 0;
    //trk.cells = det.cells_vcs;
    if (det.det_in_img[0] ==1 ||det.det_in_img[1] ==1 ||det.det_in_img[2] ==1 ){
        trk.det_in_img = det.det_in_img;
    }
    //updata img 2d box:
    UpdataImg2DBox(det);
    if(trk.lidar_fake2vision){
      if(det.lidar_fake2vision){
        trk.lidar_fake2vision += det.lidar_fake2vision;
      }else{
        trk.lidar_fake2vision = 1;//清理为1
      }
    }

    // check the correlated dynamic/static status
    if (det.is_dor_valid == true) {
      if (det.is_static == true) {
        trk.dor_static_cnt++;
        trk.dor_static_cnt = trk.dor_static_cnt <= 5 ? trk.dor_static_cnt : 5;
      } else {
        trk.dor_static_cnt--;
        trk.dor_static_cnt = trk.dor_static_cnt > 0 ? trk.dor_static_cnt : 0;
      }
      trk.dor_static_points_num = det.static_points_num;
    } else {
      trk.dor_static_points_num = 0;
    }

    // check lidar dynamic-static detection status
    // 0:unknown, 1:static, 2:movable, 3:moving
    if (det.ldd_status.isvalid == true) {
      if (det.ldd_status.track_status == 1 || trk.ldd_static == true && det.ldd_status.track_status == 2) {
        trk.ldd_static = true;
      } else {
        trk.ldd_static = false;
      }
      // check status buffer
      if (det.ldd_status.track_sta.size() >= 1) {
        int static_status_cnt = std::count(det.ldd_status.track_sta.begin(),
                                           det.ldd_status.track_sta.end(), 1);
        if (static_status_cnt >= det.ldd_status.track_sta.size() - 1) {
          trk.ldd_static = true;
        } else {
          trk.ldd_static = false;
        }
      }
    } else {
      trk.ldd_static = false;
    }

    if (IsCarOrTruck(trk.type)) {
      bbox_.GetPredictBBoxCenter(trk);
    }
    // AINFO << " +++++++ IN update fusion tracks lidar "
    //           << " | trk.x_center_vcs " << trk.x_center_vcs
    //           << " | trk.y_center_vcs " << trk.y_center_vcs
    //           << " | trk.radarInfo.radar_id " << trk.radarInfo.radar_id
    //          ;
  }
  if (matched_lidar_dets_.size() == 0 && trk.coast_count < 5) {
    trk.coast_count++;
    if (trk.coast_count > 5) {trk.coast_count = 5;}
  }
  // // update track with radar detection  modified by hhl
    for (const auto& det : matched_radar_dets_) {
        trk.radarInfo = det.radarInfo;
        // update position
        if (matched_lidar_dets_.size()<1 &&  fabs(det.y_vcs <= 20 )) { // 当雷达检测没有输出时，用毫米波雷达的结果  && IsCarOrTruck(trk.type)
            trk.lidar_received_time = det.received_time;
            trk.x = det.x_dr;
            trk.y = det.y_dr;
            trk.x_center_vcs = det.radarInfo.x;
            trk.y_center_vcs = det.radarInfo.y;
            trk.vx = det.vx_dr;
            trk.vy = det.vy_dr;
            trk.type = det.type;
            trk.heading = det.heading_dr;
            trk.heading_vcs = det.heading_vcs;
            trk.width = det.width > 0 ? det.width : trk.width;
            trk.length = det.length > 0 ? det.length : trk.length;
            trk.height = det.height;
            trk.length_predict = trk.length;
            trk.width_predict = trk.width;
            // bbox_.GetBBoxCenter(trk, det.radarInfo.x, det.radarInfo.y);
            bbox_.UpdateBBoxBuffer(trk, det);
            trk.heading_source = "radar";
            //get bounding box
            // trk.cells.clear();
            // auto geoPoints = bbox_.GetLidarBoundingBox(det.x_vcs, det.y_vcs,
            //                 trk.length, trk.width, trk.heading_vcs);
            // for (const auto& pt : geoPoints) {
            //     OccupyCell cell;
            //     cell.x = pt.x;
            //     cell.y = pt.y;
            //     trk.cells.push_back(cell);
            // }
            trk.updated = true;
            // reset filter
            InitFilterParams();
            // AINFO << " +++++++ IN update fusion tracks radar "
            //   << " | trk.x_center_vcs " << trk.x_center_vcs
            //   << " | trk.y_center_vcs " << trk.y_center_vcs
            //   << " | trk.radarInfo.radar_id " << trk.radarInfo.radar_id
            //  ;
        }
    }
  // // update track with radar detection
  // for (const auto& det : matched_radar_dets_) {
  //   if (trk.updated == false) {
  //     // switch to radar track
  //     // if (trk.coast_count >= 3 || IsRadarValid(det.x_vcs, det.y_vcs)) {
  //     if (IsRadarValid(det.x_vcs, det.y_vcs)) {
  //       trk.lidar_received_time = 0;
  //       trk.x = det.x_dr;
  //       trk.y = det.y_dr;
  //       trk.vx = det.vx_dr;
  //       trk.vy = det.vy_dr;
  //       bbox_.GetBBoxCenter(trk, det.x_vcs, det.y_vcs);
  //       trk.heading = det.heading_dr;
  //       trk.heading_vcs = det.heading_vcs;
  //       trk.heading_source = "radar";
  //       trk.updated = true;
  //       bbox_.UpdateBBoxBuffer(trk, det);
  //       InitFilterParams();
  //     }
  //   }
  // }

  // update track with vision detections
  for (const auto& det : matched_vision_dets_) {
    // AINFO << " IN matching MVS  ------ 1";
    if (matched_lidar_dets_.size() || matched_radar_dets_.size() ) {
      break;
    }
    trk.x = det.x_dr;
    trk.y = det.y_dr;
    if (det.vision_velo_valid) {
      trk.vx = det.vx_dr;
      trk.vy = det.vy_dr;
      trk.heading = det.heading_dr;
      trk.heading_vcs = det.heading_vcs;
      trk.heading_source = "vision";
    }
    // AINFO << " IN matching MVS  ------ 2";
    InitFilterParams();
    // update other info
    trk.cells = det.cells_vcs;
    // AINFO << " IN matching MVS  ------ 3";
    bbox_.GetBBoxCenter(trk, det.x_vcs, det.y_vcs);
    // AINFO << " IN matching MVS  ------ 4";
    trk.updated = true;
    // AINFO << " IN matching MVS  ------ 5";
    UpdateLidar2CameraInfo(det);
    // AINFO << " IN matching MVS  ------ 6";
  }
  UpdateVelocityStatus();
  UpdateFuseSource(trk.fuse_source);
  bbox_.CalculateBBoxVelocity(trk);
}


void FusionTracker::UpdateFakeTrack(const double timestamp,
                                    const PerceptionLocType &locpos) {
  trk.system_time = timestamp;
  // update fake track with lidar detection
  for (const auto& det : matched_lidar_dets_) {
    // add by hhl 对于is_bl_cluster的unknown type的类别进行长宽高比较 中心距离比较
    if( trk.is_bl_cluster &&  trk.type == UNKNOWN_TYPE && det.is_bl_cluster && det.type == UNKNOWN_TYPE ){
        float width_check =  trk.width / det.width ;
        float height_check =  trk.height / det.height ;
        float length_check =  trk.length / det.length ;
        float center_check =  sqrt(pow(( trk.x_center_vcs - det.x_vcs), 2) + pow(( trk.y_center_vcs - det.y_vcs), 2));
        if( width_check<= 0.5 || width_check >=2 || height_check<=0.5 || height_check >= 2 ||length_check<=0.5 || length_check >=2 || center_check >2 ){
            AINFO << "[MOT MANAGER] BL_CLUSTER BOUNCING " <<  trk.id;
            break;
        }
    }  
    trk.lidar_received_time = det.received_time;
    auto prev_fuse_point = trk.fuse_point;
    auto trk_type = trk.type;
    auto trk_heading_vcs = trk.heading_vcs;
    // flip the flag
    bool is_reversed = IsTopBottomReversed(trk_heading_vcs, det.heading_vcs);
    if (is_reversed == true) {
      trk.is_top_bottom_reversed = (trk.is_top_bottom_reversed == true) ? false : true;
    }
    bbox_.GetAnchorPoint(prev_fuse_point, trk_type, trk_heading_vcs, det, trk.is_top_bottom_reversed,
                         trk.fuse_point, trk.x, trk.y);
    trk.vx = 0;
    trk.vy = 0;
    trk.x_center_vcs = det.x_vcs;
    trk.y_center_vcs = det.y_vcs;
    trk.z_center_vcs = det.z_vcs;
    // AINFO << " IN update fake tracks lidar "
    //           << " | trk.x_center_vcs " << trk.x_center_vcs
    //           << " | trk.y_center_vcs " << trk.y_center_vcs
    //           << " | trk.radarInfo.radar_id " << trk.radarInfo.radar_id
    //          ;
    trk.heading_vcs = det.heading_vcs;
    trk.cells = det.cells_vcs;
    // 2021-06-22: process multi corrlation
    UpdateLidar2CameraInfo(det);
    UpdateObjectType(det);
    if(trk.is_bl_cluster){
        UpdateObjectDimension_SPC(det,locpos);
    }else{
        UpdateObjectDimension(det, locpos);
    }
    bbox_.UpdateBBoxBuffer(trk, det);
    trk.updated = true;
    //trk曾经出现在图像中
    if (det.det_in_img[0] ==1 ||det.det_in_img[1] ==1 ||det.det_in_img[2] ==1 ){
        trk.det_in_img = det.det_in_img;
    }
    //add by lxd
    // trk.img_box_x = det.img_box_x;
    // trk.img_box_y = det.img_box_y;
    // trk.img_box_w = det.img_box_w;
    // trk.img_box_h = det.img_box_h;
    // trk.img_width = det.img_width;
    // trk.img_height = det.img_height;
    // trk.img_camId = det.img_camId;
  }

  // update fake track with radar detection
  for (const auto& det : matched_radar_dets_) {
    trk.radarInfo = det.radarInfo;
    if (matched_lidar_dets_.size()<1 && fabs(det.y_vcs <= 20 )) {
        trk.lidar_received_time = det.received_time;
        trk.x = det.x_dr;
        trk.y = det.y_dr;
        trk.x_center_vcs = det.radarInfo.x;
        trk.y_center_vcs = det.radarInfo.y;
        trk.vx = det.vx_dr;
        trk.vy = det.vy_dr;
        trk.type = det.type;
        AINFO << "new fake tracks type: " <<  det.type ;
        trk.heading = det.heading_dr;
        trk.heading_vcs = det.heading_vcs;
        trk.width = det.width > 0 ? det.width : trk.width;
        trk.length = det.length > 0 ? det.length : trk.length;
        trk.height = det.height;
        trk.length_predict = trk.length;
        trk.width_predict = trk.width;
        // bbox_.GetBBoxCenter(trk, det.radarInfo.x, det.radarInfo.y);
        bbox_.UpdateBBoxBuffer(trk, det);
        trk.heading_source = "radar";
        trk.updated = true;
        // reset filter
        InitFilterParams();
    //   trk.lidar_received_time = 0;
    //   trk.x = det.x_dr;
    //   trk.y = det.y_dr;
    //   trk.vx = det.vx_dr;
    //   trk.vy = det.vy_dr;
    //   trk.heading = det.heading_dr;
    //   bbox_.GetBBoxCenter(trk, det.x_vcs, det.y_vcs);
    //   trk.heading_vcs = det.heading_vcs;
    //   trk.heading_source = "radar";
    //   trk.updated = true;
    //   bbox_.UpdateBBoxBuffer(trk, det);
        AINFO << " +++++++ IN update fake tracks radar "
            << " | trk.x_center_vcs " << trk.x_center_vcs
            << " | trk.y_center_vcs " << trk.y_center_vcs
            << " | trk.radarInfo.radar_id " << trk.radarInfo.radar_id
            << " | trk.type " << trk.type
           ;
    }
  }

  UpdateFuseSource(trk.fuse_source);
  bbox_.CalculateBBoxVelocity(trk);
}

void FusionTracker::UpdateLidar2CameraInfo(const ObjectDetection_ &det) {
  trk.is_l2c_updated = false;
  if (det.l2cInfo.is_valid == true && det.l2cInfo.totalCamObjs == 1) {
    trk.l2cInfo = det.l2cInfo;
    trk.is_l2c_updated = true;
  } else if (det.l2cInfo.is_valid == true && det.l2cInfo.totalCamObjs > 1) {
    if (trk.l2cInfo.is_valid == true && trk.l2cInfo.totalCamObjs == 1
        && (det.l2cInfo.time_triggered - trk.l2cInfo.time_triggered) <= 0.3) {
      // find the same image track id
      auto img_track_id = trk.l2cInfo.objs[0].track_id;
      for (const auto &obj : det.l2cInfo.objs) {
        if (img_track_id == obj.track_id) {
          trk.l2cInfo = det.l2cInfo;
          trk.l2cInfo.totalCamObjs = 1;
          trk.l2cInfo.objs.clear();
          trk.l2cInfo.objs.push_back(obj);
          trk.is_l2c_updated = true;
          break;
        }
      }
    }
    if (trk.is_l2c_updated == false) {
      int max_pts = 0;
      for (const auto &obj : det.l2cInfo.objs) {
        auto pts_num = obj.pointsNum;
        if (pts_num > max_pts) {
          trk.l2cInfo = det.l2cInfo;
          trk.l2cInfo.totalCamObjs = 1;
          trk.l2cInfo.objs.clear();
          trk.l2cInfo.objs.push_back(obj);
          max_pts = pts_num;
        }
      }
      trk.is_l2c_updated = true;
    }
  }
}

void FusionTracker::UpdateObjectDimension_SPC(const ObjectDetection_ &det,
    const PerceptionLocType &locpos) {
  // ----------------------------------------------------------------
  // calculate object heading under dr
  // heading source: observed lidar, observed vision, previous tracked
  float heading_lidar_dr = det.heading_vcs + locpos.dr_yaw;
  float heading_prev_dr = trk.heading;
  // adjust heading
  float heading_dr = heading_lidar_dr;
  std::string heading_source = "lidar"; // for debug

  trk.heading = heading_dr;
  trk.heading_source = heading_source;
  trk.length = det.length;
  trk.width = det.width;
  trk.height = det.height;
  trk.length_predict = det.length;
  trk.width_predict = det.width;
}


void FusionTracker::UpdateObjectDimension(const ObjectDetection_ &det,
    const PerceptionLocType &locpos) {
  // ----------------------------------------------------------------
  // calculate object heading under dr
  // heading source: observed lidar, observed vision, previous tracked
  float heading_lidar_dr = det.heading_vcs + locpos.dr_yaw;
  float heading_prev_dr = trk.heading;
  if (det.type == UNKNOWN_TYPE) {
    heading_lidar_dr = heading_prev_dr;
  }

  // adjust heading
  float heading_dr = heading_lidar_dr;
  std::string heading_source = "lidar"; // for debug

  float speed = sqrt(pow(trk.vx, 2) + pow(trk.vy, 2));
  if (speed >= 1.5) {
    bool crosscheck = false;
    if (trk.velocitystatus == VELOCITY_STATUS_GOOD || trk.velocitystatus == VELOCITY_STATUS_MEDIUM) {
      crosscheck = true;
    } else {
      if (speed >= 5 && (trk.type == CAR_TYPE || trk.type == TRUCK_TYPE)) {
        crosscheck = true;
      }
      if (speed >= 3 && trk.type == CYCLIST_TYPE) {
        crosscheck = true;
      }
    }
    if (crosscheck == true) {
      float theta = heading_lidar_dr / 180.0 * 3.1415926;
      float obj_vlon = trk.vx * cos(theta) + trk.vy * sin(theta);
      float obj_vlat = -trk.vx * sin(theta) + trk.vy * cos(theta);
      if (obj_vlon < 0 && (speed < 5 && fabs(obj_vlon) > 2.0 * fabs(obj_vlat) || speed >= 5)) {
        heading_source = "lidar-vel-180";
      }
      if (obj_vlon > 0 && (speed < 5 && fabs(obj_vlon) > 2.0 * fabs(obj_vlat) || speed >= 5)) {
        heading_source = "lidar-vel";
      }
    }
  }

  bool has_conflict = false;
  if ((trk.type == CAR_TYPE || trk.type == TRUCK_TYPE) && trk.is_l2c_updated == true
      && trk.l2cInfo.totalCamObjs == 1 && trk.l2cInfo.objs[0].camIdx == 0) {
    float heading_vision_dr = NormalizeHeading(trk.l2cInfo.objs[0].heading + locpos.dr_yaw);
    float d = fabs(heading_vision_dr - heading_lidar_dr);
    if (d >= 360) {d -= 360;}
    if (d > 179 && d < 181) {
      bool vel_checked = false;
      if (heading_source == "lidar-vel-180") {
        heading_source = "lidar-vel-vision-180";
        vel_checked = true;
      }
      if (heading_source == "lidar-vel") {
        heading_source = "lidar-vision-180-conflict-vel";
        if (speed >= 5) {
          heading_source = "lidar-vel-conflict-vision";
        }
        vel_checked = true;
        has_conflict = true;
      }
      if (vel_checked == false) {
        heading_source = "lidar-vision-180";
      }
    }
    if (d < 1) {
      bool vel_checked = false;
      if (heading_source == "lidar-vel") {
        heading_source = "lidar-vel-vision";
        vel_checked = true;
      }
      if (heading_source == "lidar-vel-180") {
        heading_source = "lidar-vision-conflict-vel";
        if (speed >= 5) {
          heading_source = "lidar-vel-180-conflict-vision";
        }
        vel_checked = true;
        has_conflict = true;
      }
      if (vel_checked == false) {
        heading_source = "lidar-vision";
      }
    }
  }
  // check previous heading
  if (has_conflict == false) {
    float d = fabs(heading_prev_dr - heading_lidar_dr);
    bool is_reversed = (heading_source.find("180") != string::npos);
    if (d >= 360) {d -= 360;}
    if (d >= 165 && d <= 195 && is_reversed == false) {
      has_conflict = true;
    }
    if (d < 15 && is_reversed == true) {
      has_conflict = true;
    }
    if (heading_source == "lidar" && has_conflict == true) {
      heading_source = "lidar-prev-180";
    }
  }

  if (has_conflict == false || trk.heading_conflict_cnt >= 2) {
    if (heading_source.find("180") == string::npos) {
      heading_dr = heading_lidar_dr;
    } else {
      heading_dr = heading_lidar_dr > 0 ? (heading_lidar_dr - 180) : (heading_lidar_dr + 180);
    }
    if (has_conflict == false) {
      trk.heading_conflict_cnt = 0;
    }
  } else {
    float d = fabs(heading_prev_dr - heading_lidar_dr);
    if (d >= 360) {d -= 360;}
    if (d >= 165 && d <= 195) {
      heading_dr = heading_lidar_dr > 0 ? (heading_lidar_dr - 180) : (heading_lidar_dr + 180);
      heading_source = "lidar-prev-180";
    }
    if (d < 15) {
      heading_dr = heading_lidar_dr;
      heading_source = "lidar-prev";
    }
    trk.heading_conflict_cnt++;
  }
  trk.heading = heading_dr;
  trk.heading_source = heading_source;
  trk.heading_has_conflict = has_conflict;

  // ----------------------------------------------------------------
  // update object shapes
  trk.length = det.length;
  trk.width = det.width;
  trk.height = det.height;

#if 0 //20240701：当前是聚类障碍物，不使用此策略，长宽不一定对
  if (max_length_observed_ == false && fabs(det.x_vcs) <= 5) {
    max_length_observed_ = true;
  }
  auto obj_type = trk.type;
  if (IsCarOrTruck(obj_type)) {
    float MAX_WIDTH = 3.5;
    trk.width_predict = det.width <= MAX_WIDTH ? det.width : MAX_WIDTH;
    if (fabs(det.x_vcs) > 5) {
      if (max_length_observed_ == false) {
        trk.length_predict = std::max(trk.width_predict * 2.5, (double)det.length);
      }
    } else {
      // do not process wired length
      if ((obj_type == CAR_TYPE && det.length <= 8.0) || (obj_type == TRUCK_TYPE && det.length <= 28.0)) {
        trk.length_predict = det.length > trk.length_predict ? det.length : trk.length_predict;
      }
    }
  } else if (IsCyclist(obj_type)) {
    // rule out wrong length
    if (det.length <= 2.8) {
      trk.length_predict = det.length;
      trk.width_predict = det.width;
    }
  } else {
    trk.length_predict = det.length;
    trk.width_predict = det.width;
  }
#else
  trk.length_predict = det.length;
  trk.width_predict = det.width;
#endif
}


void FusionTracker::UpdateObjectType(const ObjectDetection_ & det) {
  auto det_type = det.type;
  if (trk.is_l2c_updated == true && trk.l2cInfo.totalCamObjs == 1) {
    det_type = trk.l2cInfo.objs[0].norm_type;
  }
  // 利用trk结果维持匹配上的det结果，只要trk曾经出现在视野范围内则相信其类别
  if (vision_person_filter_){
        //trk曾经出现在视野范围内且目前trk.is_l2c_updated = false
    if(trk.is_l2c_updated == false && trk.type == PEDESTRIAN_TYPE){
        if ( (trk.det_in_img[0]==1 || trk.det_in_img[1]==1 || trk.det_in_img[2]==1 )){
            AINFO<< "[fusion tracker] ：trk once in vision ,use trk! ";
            det_type = trk.type;
        }
    }
    // 水马路障柱类别保持
    if(trk.is_l2c_updated == false ){
        if (trk.type == SUBULATE_TYPE || trk.type == ROADBLOCK_TYPE || trk.type == ANTITANK_TYPE || trk.type == WATERFENCE_TYPE) {
        if ( (trk.det_in_img[0]==1 || trk.det_in_img[1]==1 || trk.det_in_img[2]==1 )){
            AINFO<< "[fusion tracker] ：trk once in vision ,use trk! ";
            det_type = trk.type;
        }
        }
    }
  }
  // 利用主雷达的检测类别维持聚类结果的类别
  if(det.is_bl_cluster == true && trk.type != UNKNOWN_TYPE ){
        det_type = trk.type;
  }
  if (IsSameType(trk.type, det_type) && det_type != UNKNOWN_TYPE) {
    trk.type_confidence += 1;
    trk.type_confidence = trk.type_confidence >= 10 ? 10 : trk.type_confidence;
  } else {
    if (!trk.is_type_sure) {
      trk.type = det_type;
    }
    trk.type_confidence -= 1;
    trk.type_confidence = trk.type_confidence <= 0 ? 0 : trk.type_confidence;
  }
  if (trk.type_confidence >= 5) {
    trk.is_type_sure = true;
  } else {
    trk.is_type_sure = false;
  }
  // 处理检测出的路障(水马障碍物）、锥形桶、反坦克锥
  // TODO:unknown是否加入 ？ lxd
  if (det_type == SUBULATE_TYPE || det_type == ROADBLOCK_TYPE || det_type == ANTITANK_TYPE || det_type == UNKNOWN_UNMOVABLE_TYPE) { //modift by lxd
    trk.is_type_sure = true;
  }
}

void FusionTracker::UpdateFuseSource(int &fuse_source) {
  if (matched_lidar_dets_.size() > 0 && matched_radar_dets_.size() == 0) {
    fuse_source = SOURCE_LIDAR_ONLY;
    if (matched_lidar_dets_[0].l2cInfo.totalCamObjs >= 1) {
      fuse_source = SOURCE_LIDAR_VISION;
    }
    if (matched_vision_dets_.size() > 0) {
      fuse_source = SOURCE_LIDAR_VISION;
    }
  } else if (matched_lidar_dets_.size() == 0 && matched_radar_dets_.size() > 0) {
    fuse_source = SOURCE_RADAR_ONLY;
  } else if (matched_lidar_dets_.size() > 0 && matched_radar_dets_.size() > 0) {
    fuse_source = SOURCE_LIDAR_RADAR;
    if (matched_lidar_dets_[0].l2cInfo.totalCamObjs >= 1) {
      fuse_source = SOURCE_LIDAR_VISION_RADAR;
    }
  } else if (matched_lidar_dets_.size() == 0 && matched_radar_dets_.size() == 0
             && matched_vision_dets_.size() > 0) {
    fuse_source = SOURCE_VISION_ONLY;
  } else if (matched_lidar_dets_.size() == 0 && matched_radar_dets_.size() > 0
             && matched_vision_dets_.size() > 0) {
    fuse_source = SOURCE_RADAR_VISION;
  } else {
    fuse_source = SOURCE_NONE;
  }
}

void FusionTracker::UpdateVelocityStatus() {
  CalcVelocityRMS(trk.rms_vx, trk.vx);
  CalcVelocityRMS(trk.rms_vy, trk.vy);
  CalcVelocityHeadingRMS(trk.rms_heading, trk.vx, trk.vy);
  trk.reset_filter_cnt++;
  trk.reset_filter_cnt = trk.reset_filter_cnt <= VELOCITY_BUFFER_SIZE ? trk.reset_filter_cnt : VELOCITY_BUFFER_SIZE;
  // new tracks
  if (trk.age < 5 || trk.reset_filter_cnt < VELOCITY_BUFFER_SIZE) {
    trk.velocitystatus = VELOCITY_STATUS_INVALID;
  } else {
    if ((trk.rms_vx.rms >= 1.5 || trk.rms_vy.rms >= 1.5) && trk.rms_heading.rms >= 15) {
      trk.velocitystatus = VELOCITY_STATUS_BAD;
      trk.rms_vx.buffer.clear();
      trk.rms_vy.buffer.clear();
      trk.rms_heading.buffer.clear();
    } else if (trk.rms_vx.rms <= 0.5 && trk.rms_vy.rms <= 0.5) {
      trk.velocitystatus = VELOCITY_STATUS_GOOD;
    } else {
      trk.velocitystatus = VELOCITY_STATUS_MEDIUM;
    }
  }
  // checkout if the track is static
  float speed = sqrt(pow(trk.vx, 2) + pow(trk.vy, 2));
  if (speed < 0.5 && trk.velocitystatus != VELOCITY_STATUS_BAD) {
    trk.velo_static_cnt++;
    trk.velo_static_cnt = trk.velo_static_cnt <= 5 ? trk.velo_static_cnt : 5;
  } else {
    trk.velo_static_cnt--;
    trk.velo_static_cnt = trk.velo_static_cnt > 0 ? trk.velo_static_cnt : 0;
  }
  if ((speed >= 1.5 && trk.velocitystatus == VELOCITY_STATUS_GOOD)
      || (speed >= 2.0 && (trk.velocitystatus == VELOCITY_STATUS_MEDIUM
                           || trk.velocitystatus == VELOCITY_STATUS_GOOD))) {
    // deal with static trucks
    const int MAX_POINTS = 290;
    if (trk.dor_static_points_num <= MAX_POINTS) {
      trk.velo_static_cnt = 0;
      trk.dor_static_cnt = 0;
    }
  }

  bool previous_status = trk.is_trk_static;
  bool current_status = trk.is_trk_static;
  if (trk.dor_static_cnt >= 4 || trk.velo_static_cnt >= 3) {
    current_status = true;
  }
  if (trk.velo_static_cnt == 0 && trk.dor_static_cnt == 0) {
    current_status = false;
  }
  // detect the switch from true to false
  if (previous_status == true && current_status == false && trk.ldd_static == true) {
    current_status = true;
  }
  trk.is_trk_static = current_status;
  if (IsPedestrainOrCyclist(trk.type)) {
    if (trk.ldd_static == true) {
      trk.is_trk_static = true;
    } else {
      trk.is_trk_static = current_status;
    }
  }
}

void FusionTracker::CalcVelocityRMS(VelocityRMS & trk_vel, const float v) {
  trk_vel.buffer.push_back(v);
  while (trk_vel.buffer.size() > VELOCITY_BUFFER_SIZE) {
    trk_vel.buffer.pop_front();
  }
  if (trk_vel.buffer.size() > 0) {
    double sum = std::accumulate(std::begin(trk_vel.buffer), std::end(trk_vel.buffer), 0.0);
    double mean = sum / trk_vel.buffer.size();
    double accum = 0.0;
    std::for_each(std::begin(trk_vel.buffer), std::end(trk_vel.buffer), [&](const float d) {
      accum += (d - mean) * (d - mean);
    });
    trk_vel.rms = sqrt(accum / trk_vel.buffer.size());
  }
}
//TODO :RMS or RMSE ？ LXD
void FusionTracker::CalcVelocityHeadingRMS(VelocityRMS & trk_heading, const float & vx, const float & vy) {
  const int BUFFER_SIZE = 10;
  const float DEG2RAD = 3.1415926 / 180.0;
  const float RAD2DEG = 1 / DEG2RAD;
  float heading_deg = atan2(vy, vx) * RAD2DEG;
  heading_deg = heading_deg < 0 ? (heading_deg + 360) : heading_deg;
  trk_heading.buffer.push_back(heading_deg);
  while (trk_heading.buffer.size() > BUFFER_SIZE) {
    trk_heading.buffer.pop_front();
  }
  if (trk_heading.buffer.size() >= 2) {
    std::vector<float> delta_vec;
    int MAX_NUM = trk_heading.buffer.size() <= 5 ? trk_heading.buffer.size() : 5;
    for (int i = 0; i < MAX_NUM - 1; i++) {
      float angle1 = trk_heading.buffer[i];
      float angle2 = trk_heading.buffer[i + 1];
      float delta = NormalizeHeading(angle2 - angle1);
      delta_vec.push_back(delta);
    }
    double sum = std::accumulate(std::begin(delta_vec), std::end(delta_vec), 0.0);
    double mean = sum / delta_vec.size();
    double accum = 0.0;
    std::for_each(std::begin(delta_vec), std::end(delta_vec), [&](const float d) {
      accum += (d - mean) * (d - mean);
    });
    trk_heading.rms = sqrt(accum / delta_vec.size());

    // heading mean
    float sum_sin_angle = 0;
    float sum_cos_angle = 0;
    for (const auto& angle : trk_heading.buffer) {
      sum_sin_angle += sin(angle * DEG2RAD);
      sum_cos_angle += cos(angle * DEG2RAD);
    }
    float mean_sin_angle = sum_sin_angle / trk_heading.buffer.size();
    float mean_cos_angle = sum_cos_angle / trk_heading.buffer.size();
    trk_heading.mean = atan2(mean_sin_angle, mean_cos_angle) * RAD2DEG;
  } else {
    trk_heading.rms = 0;
    if (trk_heading.buffer.size() == 1) {
      trk_heading.mean = NormalizeHeading(trk_heading.buffer[0]);
    }
  }
}


void FusionTracker::ClearMatchedDetections() {
  matched_lidar_dets_.clear();
  matched_radar_dets_.clear();
  matched_vision_dets_.clear();
}

void FusionTracker::InitFilterParams() {
  BaseTracker::Init(config_.fusion_track_config.base_config);
}

bool FusionTracker::IsRadarValid(const double x_vcs, const double y_vcs) {
  auto x_min = config_.fake_track_config.radar_create_x_min;
  auto x_max = config_.fake_track_config.radar_create_x_max;
  auto y_min = config_.fake_track_config.radar_create_y_min;
  auto y_max = config_.fake_track_config.radar_create_y_max;
  if (fabs(x_vcs) <= x_max && fabs(x_vcs) >= x_min
      && fabs(y_vcs) <= y_max && fabs(y_vcs) >= y_min) {
    return true;
  } else {
    return false;
  }
}

}
}//namespace avos
}//namespace perception
