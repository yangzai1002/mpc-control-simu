#ifndef DATAPOOL_INCLUDE_PERCEPTION_INPUT_H_
#define DATAPOOL_INCLUDE_PERCEPTION_INPUT_H_

#include <iostream>
#include "xkit/planning/datapool/public_typedef.h"
#include "xkit/planning/datapool/perception_struct.h"
#include "xkit/planning/toolbox/geometry/site.h"
// #include "prediction_input.h"
namespace avos {
namespace planning {
// typedef struct TrafficLight {
//   eLightColor color;
//   std::string id;
//   double confidence;
//   double tracking_time;
// } TrafficLight;

struct PerceptionFusionData {
  //语义融合原始数据时间
  double timestamp;
  std::vector<ObjectCell> cells;
  std::vector<Object> objects;
  std::vector<ObjectCell> lane;

  PerceptionFusionData() {
    cells.reserve(MAX_STATIC_CELL_SIZE);
    objects.reserve(MAX_OBJ_SIZE);
    lane.reserve(MAX_STATIC_CELL_SIZE);
    // traffic_lights.reserve(MAX_LIGHTS_SIZE);
    cells.clear();
    objects.clear();
    lane.clear();
  }
  ~PerceptionFusionData() {
    cells.clear();
    objects.clear();
    lane.clear();
  }
};

struct PerceptionSemanticMapData {
  //语义融合原始数据时间
  double timestamp;
  std::vector<SemanticCell> semantic_map;
  std::vector<SemanticCell> semantic_edge;

  PerceptionSemanticMapData() {
    semantic_map.reserve(MAX_STATIC_CELL_SIZE);
    semantic_edge.reserve(MAX_OBJ_SIZE);
    semantic_map.clear();
    semantic_edge.clear();
  }
  ~PerceptionSemanticMapData() {
    semantic_map.clear();
    semantic_edge.clear();
  }
};

struct PerceptionTargetObjData {
  //目标感知
  double timestamp;
  geometry::Site target_obj;
  std::vector<geometry::Site> target_boundary;

  PerceptionTargetObjData() {
    target_boundary.reserve(MAX_STATIC_CELL_SIZE);
    target_boundary.clear();
    target_obj.Reset();
  }
  ~PerceptionTargetObjData() {
    target_obj.Reset();
    target_boundary.clear();
  }
};

typedef struct PerceptionFusion {
  double time;
  int valid_count;
  bool is_valid;
  bool new_input;  /*!< judge the Perception is the new data from callback, default is false*/
  // LocalizationData localization_data;
  PerceptionFusionData perception_fusion_data;
  PerceptionFusion() {
    time = 0;
    valid_count = 0;
    is_valid = false;
    new_input = false;
  }
  bool CheckValid() {
    valid_count++;
    if (valid_count > MAINSTREAM_RESUME_TIMES) {
      valid_count = 0;
      is_valid = true;
      new_input = true;
    }
    return is_valid;
  }
  void ResetValid() {
    valid_count = 0;
    is_valid = false;
    new_input = false;
  }
} PerceptionFusion;

typedef struct PerceptionFusionBuffer
{
  std::queue<PerceptionFusion> buffer_;
  size_t buffer_size_;

  void Reset(){
    buffer_size_ = 10;
    std::queue<PerceptionFusion> buffer_empty;
    buffer_.swap(buffer_empty);
  }

  PerceptionFusionBuffer(){
    Reset();
  }

  void LimitSize(){
    while(buffer_.size() > buffer_size_){
      buffer_.pop();
    }
  }

  void PushBuffer(const PerceptionFusion& per_objs){
    buffer_.push(per_objs);
    LimitSize();
  }

  std::queue<PerceptionFusion> GetBuffer(){
    return buffer_;
  }

  int SetBufferSize(const int& size){
    if(size <= 0){
      return -1;
    }
    buffer_size_ = size;
    return 0;
  }

  int FindNearest(const double& standard_time, const double& time_bias_threshold, PerceptionFusion& per_objs){
    auto copy_buffer = buffer_;
    double time_bias = std::numeric_limits<double>::max();
    double time_bias_threshold_tmp = 0.1;
    if(time_bias_threshold > 10.0 || time_bias_threshold < 0){
      std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" Are u serious with such large time_bias_threshold: "<<time_bias_threshold<<std::endl;
    }else{
      time_bias_threshold_tmp = time_bias_threshold;
    }
    while(!copy_buffer.empty()){
      auto record_per_objs = copy_buffer.front();
      copy_buffer.pop();
      double time_bias_tmp = std::fabs(record_per_objs.perception_fusion_data.timestamp - standard_time);
      // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" time_bias_tmp: " << std::setprecision(20) <<time_bias_tmp<<std::endl;

      if(time_bias_tmp < time_bias){
        time_bias = time_bias_tmp;
        per_objs = record_per_objs;
        if(time_bias < 0.001){//1ms
          // std::cout<<"record_per_objs.timestamp:"<<record_per_objs.perception_fusion_data.timestamp;
          // std::cout<<"standard_time:"<<standard_time;
          // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" early stopping with time_bias: "<<time_bias<<", cut buffer_"<<std::endl;
          buffer_ = copy_buffer;
          break;
        }
      }
    }
    if(time_bias < time_bias_threshold_tmp){
      //tbd
      return 0;
    }
    return -1;
  }
//   int FindNearesttarget(const double& standard_time, const double& time_bias_threshold, PerceptionData& per_target){
//     auto copy_buffer = buffer_;
//     double time_bias = std::numeric_limits<double>::max();
//     double time_bias_threshold_tmp = 0.1;
//     if(time_bias_threshold > 10.0 || time_bias_threshold < 0){
//       std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" Are u serious with such large time_bias_threshold: "<<time_bias_threshold<<std::endl;
//     }else{
//       time_bias_threshold_tmp = time_bias_threshold;
//     }
//     while(!copy_buffer.empty()){
//       auto record_per_target = copy_buffer.front();
//       copy_buffer.pop();
//       double time_bias_tmp = std::fabs(record_per_target.timestamp  - standard_time);
//       // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" time_bias_tmp: " << std::setprecision(20) <<time_bias_tmp<<std::endl;

//       if(time_bias_tmp < time_bias){
//         time_bias = time_bias_tmp;
//         per_target.target_obj = record_per_target.target_obj;
//         per_target.target_boundary = record_per_target.target_boundary;
//         if(time_bias < 0.001){//1ms
//           std::cout<<"record_per_target.timestamp:"<<record_per_target.timestamp;
//           std::cout<<"standard_time:"<<standard_time;
//           // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" early stopping with time_bias: "<<time_bias<<", cut buffer_"<<std::endl;
//           buffer_ = copy_buffer;
//           break;
//         }
//       }
//     }
//     if(time_bias < time_bias_threshold_tmp){
//       //tbd
//       return 0;
//     }
//     return -1;
//   }
// }PerceptionBuffer;
}PerceptionFusionBuffer;

typedef struct PerceptionSemanticMap {
  double time;
  int valid_count;
  bool is_valid;
  bool new_input;  /*!< judge the Perception is the new data from callback, default is false*/
  // LocalizationData localization_data;
  PerceptionSemanticMapData perception_semantic_map_data;
  PerceptionSemanticMap() {
    time = 0;
    valid_count = 0;
    is_valid = false;
    new_input = false;
  }
  bool CheckValid() {
    valid_count++;
    if (valid_count > MAINSTREAM_RESUME_TIMES) {
      valid_count = 0;
      is_valid = true;
      new_input = true;
    }
    return is_valid;
  }
  void ResetValid() {
    valid_count = 0;
    is_valid = false;
    new_input = false;
  }
} PerceptionSemanticMap;

typedef struct PerceptionSemanticMapBuffer
{
  std::queue<PerceptionSemanticMap> buffer_;
  size_t buffer_size_;

  void Reset(){
    buffer_size_ = 10;
    std::queue<PerceptionSemanticMap> buffer_empty;
    buffer_.swap(buffer_empty);
  }

  PerceptionSemanticMapBuffer(){
    Reset();
  }

  void LimitSize(){
    while(buffer_.size() > buffer_size_){
      buffer_.pop();
    }
  }

  void PushBuffer(const PerceptionSemanticMap& per_objs){
    buffer_.push(per_objs);
    LimitSize();
  }

  std::queue<PerceptionSemanticMap> GetBuffer(){
    return buffer_;
  }

  int SetBufferSize(const int& size){
    if(size <= 0){
      return -1;
    }
    buffer_size_ = size;
    return 0;
  }

  int FindNearest(const double& standard_time, const double& time_bias_threshold, PerceptionSemanticMap& per_objs){
    auto copy_buffer = buffer_;
    double time_bias = std::numeric_limits<double>::max();
    double time_bias_threshold_tmp = 0.1;
    if(time_bias_threshold > 10.0 || time_bias_threshold < 0){
      std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" Are u serious with such large time_bias_threshold: "<<time_bias_threshold<<std::endl;
    }else{
      time_bias_threshold_tmp = time_bias_threshold;
    }
    while(!copy_buffer.empty()){
      auto record_per_objs = copy_buffer.front();
      copy_buffer.pop();
      double time_bias_tmp = std::fabs(record_per_objs.perception_semantic_map_data.timestamp - standard_time);
      // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" time_bias_tmp: " << std::setprecision(20) <<time_bias_tmp<<std::endl;

      if(time_bias_tmp < time_bias){
        time_bias = time_bias_tmp;
        per_objs = record_per_objs;
        if(time_bias < 0.001){//1ms
          // std::cout<<"record_per_objs.timestamp:"<<record_per_objs.perception_semantic_map_data.timestamp;
          // std::cout<<"standard_time:"<<standard_time;
          // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" early stopping with time_bias: "<<time_bias<<", cut buffer_"<<std::endl;
          buffer_ = copy_buffer;
          break;
        }
      }
    }
    if(time_bias < time_bias_threshold_tmp){
      //tbd
      return 0;
    }
    return -1;
  }
}PerceptionSemanticMapBuffer;

typedef struct PerceptionTargetObj {
  double time;
  int valid_count;
  bool is_valid;
  bool new_input;  /*!< judge the Perception is the new data from callback, default is false*/
  // LocalizationData localization_data;
  PerceptionTargetObjData perception_target_obj_data;
  PerceptionTargetObj() {
    time = 0;
    valid_count = 0;
    is_valid = false;
    new_input = false;
  }
  bool CheckValid() {
    valid_count++;
    if (valid_count > MAINSTREAM_RESUME_TIMES) {
      valid_count = 0;
      is_valid = true;
      new_input = true;
    }
    return is_valid;
  }
  void ResetValid() {
    valid_count = 0;
    is_valid = false;
    new_input = false;
  }
} PerceptionTargetObj;

typedef struct PerceptionTargetObjBuffer
{
  std::queue<PerceptionTargetObj> buffer_;
  size_t buffer_size_;

  void Reset(){
    buffer_size_ = 10;
    std::queue<PerceptionTargetObj> buffer_empty;
    buffer_.swap(buffer_empty);
  }

  PerceptionTargetObjBuffer(){
    Reset();
  }

  void LimitSize(){
    while(buffer_.size() > buffer_size_){
      buffer_.pop();
    }
  }

  void PushBuffer(const PerceptionTargetObj& per_objs){
    buffer_.push(per_objs);
    LimitSize();
  }

  std::queue<PerceptionTargetObj> GetBuffer(){
    return buffer_;
  }

  int SetBufferSize(const int& size){
    if(size <= 0){
      return -1;
    }
    buffer_size_ = size;
    return 0;
  }

  int FindNearest(const double& standard_time, const double& time_bias_threshold, PerceptionTargetObj& per_objs){
    auto copy_buffer = buffer_;
    double time_bias = std::numeric_limits<double>::max();
    double time_bias_threshold_tmp = 0.1;
    if(time_bias_threshold > 10.0 || time_bias_threshold < 0){
      std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" Are u serious with such large time_bias_threshold: "<<time_bias_threshold<<std::endl;
    }else{
      time_bias_threshold_tmp = time_bias_threshold;
    }
    while(!copy_buffer.empty()){
      auto record_per_objs = copy_buffer.front();
      copy_buffer.pop();
      double time_bias_tmp = std::fabs(record_per_objs.perception_target_obj_data.timestamp - standard_time);
      // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" time_bias_tmp: " << std::setprecision(20) <<time_bias_tmp<<std::endl;

      if(time_bias_tmp < time_bias){
        time_bias = time_bias_tmp;
        per_objs = record_per_objs;
        if(time_bias < 0.001){//1ms
          // std::cout<<"record_per_objs.timestamp:"<<record_per_objs.perception_target_obj_data.timestamp;
          // std::cout<<"standard_time:"<<standard_time;
          // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" early stopping with time_bias: "<<time_bias<<", cut buffer_"<<std::endl;
          buffer_ = copy_buffer;
          break;
        }
      }
    }
    if(time_bias < time_bias_threshold_tmp){
      //tbd
      return 0;
    }
    return -1;
  }
}PerceptionTargetObjBuffer;

struct Perception 
{
  PerceptionFusion perception_fusion;
  PerceptionSemanticMap perception_semantic_map;
  PerceptionTargetObj perception_target_obj;
};

} // namespace planning
} // namespace avos

#endif // DATAPOOL_INCLUDE_LOCPERCEPTION_INPUT_H_
