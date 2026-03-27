#ifndef DATAPOOL_INCLUDE_LOCALIZATION_INPUT_H_
#define DATAPOOL_INCLUDE_LOCALIZATION_INPUT_H_
#include <iostream>
#include "xkit/planning/datapool/public_typedef.h"

namespace avos {
  namespace planning {
    struct LocalizationData {
      double time;
      double xg, yg, zg, roll, pitch, yaw;
      double velocity;
      double vx, vy;
      double yawrate;
      double loc_xg_dr;
      double loc_yg_dr;
      double loc_zg_dr;
      double loc_yaw_dr;
      double dr_time;
      int gps_state;
      int is_heading_valid;

      int valid_count;
      bool is_valid;

      void Reset(){
        valid_count = 0;
        is_valid = true;
        xg = 0.0;
        yg = 0.0;
        zg = 0.0;
        roll = 0.0;
        pitch = 0.0;
        yaw = 0.0;
        velocity = 0.0;
        vx = 0.0;
        vy = 0.0;
        yawrate = 0.0;
        loc_xg_dr = 0.0;
        loc_yg_dr = 0.0;
        loc_zg_dr = 0.0;
        loc_yaw_dr = 0.0;
        dr_time = 0.0;
        gps_state = 0;
        is_heading_valid = 0;
      }

      LocalizationData() {
        Reset();
      }

      geometry::Site ToSite() const{
        geometry::Site loc_p;
        loc_p.xg = xg;
        loc_p.yg = yg;
        loc_p.globalangle = yaw;
        loc_p.x = 0.0;
        loc_p.y = 0.0;
        loc_p.angle = 0.0;
        loc_p.length = 0.0;
        loc_p.velocity = velocity;
        return loc_p;
      }

      bool CheckData() {
        if (xg < 0.01 || yg < 0.01) return false;
        return true;
      }

      void ResetValid() {
        valid_count = 0;
        is_valid = false;
      }

      bool CheckValid(double input_xg, double input_yg) {
        if (fabs(xg) < 0.1 &&  fabs(yg) < 0.1 ) {  //for init
          return true;
        }
        double dis_err = std::hypot(input_xg - xg, input_yg - yg);
        if (dis_err > 0.5) {
          ResetValid();
        }
        if (is_valid == false) {
          valid_count++;
          if (valid_count > LOCPOS_JUMP_RESUME_TIMES) {
            valid_count = 0;
            is_valid = true;
          }
        }

        return is_valid;
      }
    };

    typedef struct Localization {
      double time;
      int valid_count;
      bool is_valid;
      bool new_input;  /*!< judge the Localization is the new data from callback, default is false*/
      // LocalizationData localization_data;
      LocalizationData localization_data;      
      bool bad_quality;
        
      Localization() {
        time = 0;
        valid_count = 0;
        is_valid = false;
        new_input = false;
        bad_quality = false;
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
    } Localization;

    typedef struct LocalizationBuffer
    {
      std::queue<Localization> buffer_;
      size_t buffer_size_;

      void Reset(){
        buffer_size_ = 10;
        std::queue<Localization> buffer_empty;
        buffer_.swap(buffer_empty);
      }

      LocalizationBuffer(){
        Reset();
      }

      void LimitSize(){
        while(buffer_.size() > buffer_size_){
          buffer_.pop();
        }
      }

      void PushBuffer(const Localization& loc){
        buffer_.push(loc);
        LimitSize();
      }

      std::queue<Localization> GetBuffer(){
        return buffer_;
      }

      int SetBufferSize(const int& size){
        if(size <= 0){
          return -1;
        }
        buffer_size_ = size;
        return 0;
      }

      int FindNearest(const double& standard_time, const double& time_bias_threshold, Localization& loc){
        auto copy_buffer = buffer_;
        double time_bias = std::numeric_limits<double>::max();
        double time_bias_threshold_tmp = 0.1;
        if(time_bias_threshold > 10.0 || time_bias_threshold < 0){
          std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" Are u serious with such large time_bias_threshold: "<<time_bias_threshold<<std::endl;
        }else{
          time_bias_threshold_tmp = time_bias_threshold;
        }
        while(!copy_buffer.empty()){
          auto record_loc = copy_buffer.front();
          copy_buffer.pop();
          double time_bias_tmp = std::fabs(record_loc.localization_data.time - standard_time);
          // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" time_bias_tmp: " << std::setprecision(20) <<time_bias_tmp<<std::endl;

          if(time_bias_tmp < time_bias){
            time_bias = time_bias_tmp;
            loc = record_loc;
            if(time_bias < 0.001){//1ms
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
    }LocalizationBuffer;


//---------------------------------------------------------------------------------------------------------------------------------
    struct LocalizationDataDr {
      double time;
      double velocity;
      double loc_xg_dr;
      double loc_yg_dr;
      double loc_zg_dr;
      double loc_yaw_dr;
      int valid_count;
      bool is_valid;

      void Reset(){
        valid_count = 0;
        is_valid = true;
        velocity = 0.0;
        loc_xg_dr = 0.0;
        loc_yg_dr = 0.0;
        loc_zg_dr = 0.0;
        loc_yaw_dr = 0.0;
      }

      LocalizationDataDr() {
        Reset();
      }

      // bool CheckData() {
      //   if (xg < 0.01 || yg < 0.01) return false;
      //   return true;
      // }

      void ResetValid() {
        valid_count = 0;
        is_valid = false;
      }

      // bool CheckValid(double input_xg, double input_yg) {
      //   if (fabs(xg) < 0.1 &&  fabs(yg) < 0.1 ) {  //for init
      //     return true;
      //   }
      //   double dis_err = std::hypot(input_xg - xg, input_yg - yg);
      //   if (dis_err > 0.5) {
      //     ResetValid();
      //   }
      //   if (is_valid == false) {
      //     valid_count++;
      //     if (valid_count > LOCPOS_JUMP_RESUME_TIMES) {
      //       valid_count = 0;
      //       is_valid = true;
      //     }
      //   }

      //   return is_valid;
      // }
    };

    typedef struct LocalizationDr {
      double time;
      int valid_count;
      bool is_valid;
      bool new_input;  /*!< judge the Localization is the new data from callback, default is false*/
      // LocalizationData localization_data;
      LocalizationDataDr localization_dr_data;      
      bool bad_quality;
        
      LocalizationDr() {
        time = 0;
        valid_count = 0;
        is_valid = false;
        new_input = false;
        bad_quality = false;
      }
      // bool CheckValid() {
      //   valid_count++;
      //   if (valid_count > MAINSTREAM_RESUME_TIMES) {
      //     valid_count = 0;
      //     is_valid = true;
      //     new_input = true;
      //   }
      //   return is_valid;
      // }
      void ResetValid() {
        valid_count = 0;
        is_valid = false;
        new_input = false;
      }
    } LocalizationDr;

    typedef struct LocalizationDrBuffer
    {
      std::queue<LocalizationDr> buffer_;
      size_t buffer_size_;

      void Reset(){
        buffer_size_ = 10;
        std::queue<LocalizationDr> buffer_empty;
        buffer_.swap(buffer_empty);
      }

      LocalizationDrBuffer(){
        Reset();
      }

      void LimitSize(){
        while(buffer_.size() > buffer_size_){
          buffer_.pop();
        }
      }

      void PushBuffer(const LocalizationDr& loc){
        buffer_.push(loc);
        LimitSize();
      }

      std::queue<LocalizationDr> GetBuffer(){
        return buffer_;
      }

      int SetBufferSize(const int& size){
        if(size <= 0){
          return -1;
        }
        buffer_size_ = size;
        return 0;
      }

      int FindNearest(const double& standard_time, const double& time_bias_threshold, LocalizationDr& loc){
        auto copy_buffer = buffer_;
        double time_bias = std::numeric_limits<double>::max();
        double time_bias_threshold_tmp = 0.1;
        if(time_bias_threshold > 10.0 || time_bias_threshold < 0){
          std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" Are u serious with such large time_bias_threshold: "<<time_bias_threshold<<std::endl;
        }else{
          time_bias_threshold_tmp = time_bias_threshold;
        }
        while(!copy_buffer.empty()){
          auto record_loc = copy_buffer.front();
          copy_buffer.pop();
          double time_bias_tmp = std::fabs(record_loc.localization_dr_data.time - standard_time);
          // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" time_bias_tmp: " << std::setprecision(20) <<time_bias_tmp<<std::endl;

          if(time_bias_tmp < time_bias){
            time_bias = time_bias_tmp;
            loc = record_loc;
            if(time_bias < 0.001){//1ms
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
    }LocalizationDrBuffer;



  } //planning
} //avos

#endif
