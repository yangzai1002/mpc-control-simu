/******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * History:
 * xiayang          2019/03/28    1.0.0        build
 *****************************************************************************/
#ifndef DATAPOOL_INCLUDE_DATA_POOL_H_
#define DATAPOOL_INCLUDE_DATA_POOL_H_

#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <vector>
#include "xkit/planning/datapool/common_typedef.h"
// #include "base_log/log.h"
#include "macros.h"

namespace avos {
namespace planning {

class DataPool {
  private:
	DataPool() {
	}

public:
	static DataPool *Instance() {
		static DataPool *in = nullptr;
		if (in == nullptr) {
			in = new DataPool();
		}
		return in;
	}

 public:
  PlanningMainData* GetMainDataPtr() { return &main_data_; }
  PlanningMainData& GetMainDataRef() { return main_data_; }

  PlanningSwapComputeData &GetSwapComputeDateRef() { return compute_data_; }
  // PlanningSwapAdjustData &GetSwapAdjustDateRef() { return adjust_data_; }
  PlanningSwapCognitionData &GetSwapFollowCognitionDateRef() { return follow_cognition_data_; }
  PlanningSwapCognitionData &GetSwapArriveCognitionDateRef() { return arrive_cognition_data_; }
  PlanningSwapCognitionData &GetSwapTrackCognitionDateRef() { return track_cognition_data_; }
  PlanningSwapBehaviorData &GetSwapBehaviorDateRef() { return behavior_data_; }
  PlanningSwapPathplanData &GetSwapPathplanDateRef() { return pathplan_data_; }
  void BakComputeThreadData() {
    // compute_data_.mission_type = main_data_.task_content.mission_type;
    compute_data_.localization_dr_data = main_data_.localization_dr.localization_dr_data;
    compute_data_.localization_data = main_data_.localization.localization_data;
    compute_data_.perception = main_data_.perception;
    compute_data_.paths = main_data_.paths;
    compute_data_.follow_cognition_info = main_data_.follow_cognition_info;
    compute_data_.arrive_cognition_info = main_data_.arrive_cognition_info;
    compute_data_.track_cognition_info = main_data_.track_cognition_info;
    compute_data_.car_model = main_data_.config_info.car_model;
    compute_data_.config_info = main_data_.config_info;
    compute_data_.vehicle_status = main_data_.vehicle_status;
    // compute_data_.is_final_adjust = main_data_.task_fsm_info.is_final_adjust;
    // compute_data_.unstcognition_model.Copy(main_data_.unstcognition_model);
    main_data_.GetSmoothSemanticPath(compute_data_.semantic_path);
    main_data_.GetSemanticPath(compute_data_.smooth_semantic_path);
    compute_data_.current_behavior_status = main_data_.tracking_behavior_fsm_info.behavior_status;
    compute_data_.turning_flag = main_data_.turning_flag;
    compute_data_.remote_steering_angle_ = main_data_.remote_steering_angle_;
    compute_data_.remote_gear = main_data_.remote_gear;
    // if (!main_data_.mapengine_data.map_info_data.currentlanelists.empty()){
    //   if (!main_data_.mapengine_data.map_info_data.currentlanelists.at(0).frontlanelist.empty()) {
    //     compute_data_.mapengine_refpath = main_data_.mapengine_data.map_info_data.
    //                                       currentlanelists.at(0).frontlanelist.front().referenceline;
    //   }
    // }
    // if(!main_data_.task_content.track_inner_paths_data.path.empty()) {
    //   compute_data_.input_refpath = main_data_.task_content.track_inner_paths_data.path;
    // }
  }
  void BakComputeThreadSEHSData(const geometry::SiteVec& goals_vec) {
    compute_data_.goals_status_vec = goals_vec;
  }
  
  // void BakAdjustThreadData() {
  //   adjust_data_.localization_data = main_data_.loc_perception.localization_data;
  //   adjust_data_.paths = main_data_.paths;
  //   adjust_data_.cognition_info = main_data_.cognition_info;
  //   adjust_data_.car_model = main_data_.config_info.car_model;
  // }
  void BakCognitionThreadData() {
    return;
  }
  void BakBehaviorThreadData() {
    return;
  }
  void BakPathplanThreadData() {
    return;
  }

 private:
  PlanningMainData main_data_;
  PlanningSwapComputeData compute_data_;
  // PlanningSwapAdjustData adjust_data_;
  PlanningSwapCognitionData follow_cognition_data_; 
  PlanningSwapCognitionData arrive_cognition_data_; 
  PlanningSwapCognitionData track_cognition_data_; 
  PlanningSwapBehaviorData behavior_data_;
  PlanningSwapPathplanData pathplan_data_;
  // PlanningPathsSwapData swap_data_;
  // DataPool() {}

  // BASE_DECLARE_SINGLETON(DataPool)
};

#define DATAPOOL_REF DataPool::Instance()->GetMainDataRef()
#define COGNITION_CONFIG DATAPOOL_REF.config_info.cognition_config
#define BEHAVIOR_CONFIG DATAPOOL_REF.config_info.behavior_config
#define SPEEDPLAN_CONFIG DATAPOOL_REF.config_info.speedplan_config

}  // namespace ivpathplan
}  // namespace avos

#endif  // DATAPOOL_INCLUDE_DATA_POOL_H_
