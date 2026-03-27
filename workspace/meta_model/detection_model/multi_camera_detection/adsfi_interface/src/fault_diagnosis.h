#pragma once

#include <vector>
#include <map>
#include <mutex>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>
// #include "PerceptionFinalObjects.h"
#include "faulthandle_api.hpp"
#include "CommonFaultInfo.h"
// #include "common_simple_semantic_map/semantic_config_define.h"

// namespace avos{
// namespace perception{
namespace pertarget{

static std::string double2String(double value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

class ErrorItem{
public:
  ErrorItem(){}
  ErrorItem(const int& id, const std::string& tx_info){
    error_code_id=id;
    error_info=tx_info;
    last_data_time=0.0;
    lost_ct=0;
  }
  ~ErrorItem(){}
  CommonFaultInfo CreatFault(const std::string& extra_info,std::string &final_error_info){
    CommonFaultInfo finfo;
    if(last_data_time<1.0)
    {
      finfo.t = avos::common::NodeTime::Now().ToSecond();
    }else{
      finfo.t=last_data_time;
    }
    final_error_info = finfo.info;
    finfo.info=error_info+extra_info;
    finfo.error_code=error_code_id;
    return finfo;
  }
  int error_code_id;
  std::string error_info;
  double last_data_time;
  int lost_ct;

};

// template<typename T>
class FaultDiagnosis
{
private:
  std::map<std::string, ErrorItem> error_codes_;
public:
  FaultDiagnosis(){//3032002_SensorImageListTimeOut_error
    error_codes_["3032001_init_error"]=ErrorItem(3032001, "XDLOpenMDCVision_MDC610 init error:");
    error_codes_["3032002_SensorFrontCam_error"]=ErrorItem(3032002, "XDLOpenMDCVision_MDC610 SensorFrontCam error:");
    error_codes_["3032003_SensorLeftFrontCam_error"]=ErrorItem(3032003, "XDLOpenMDCVision_MDC610 SensorLeftFrontCam error:");
    error_codes_["3032004_SensorRightFrontCam_error"]=ErrorItem(3032004, "XDLOpenMDCVision_MDC610 SensorRightFrontCam error:");
    error_codes_["3032005_SensorRearCam_error"]=ErrorItem(3032005, "XDLOpenMDCVision_MDC610 SensorRearCam error:");
    error_codes_["3032006_SensorLeftRearCam_error"]=ErrorItem(3032006, "XDLOpenMDCVision_MDC610 SensorLeftRearCam error:");
    error_codes_["3032007_SensorRightRearCam_error"]=ErrorItem(3032007, "XDLOpenMDCVision_MDC610 SensorRightRearCam error:");
    error_codes_["3032008_SensorImageListTimeOut_error"]=ErrorItem(3032008, "XDLOpenMDCVision_MDC610 SensorImageListTimeOut error:");
  }
  ~FaultDiagnosis(){}
  std::map<int,std::string> index2string{{0,"3032002_SensorFrontCam_error"},{1,"3032003_SensorLeftFrontCam_error"},
                                        {2,"3032004_SensorRightFrontCam_error"},{3,"3032006_SensorLeftRearCam_error"},
                                        {4,"3032007_SensorRightRearCam_error"},{5,"3032005_SensorRearCam_error"}};
  //cam_name2index 和 index2string 序号对应；这里的需要来自"visionperception.yaml" 文件中camera_name ，基本是写死的
  std::map<std::string,int> cam_name2index{{"front_h100",0},{"left_front",1},
                                        {"right_front",2},{"left_back",3},
                                        {"right_back",4},{"back",5}};
  inline void SetInitError( const std::string& ifo);
  inline void SetTopicTimeOut(const std::string error_type, const std::string& ifo);
  inline void ResetError(const std::string error_type,double trigger_stamp);
};

// 初始化失败信息设置：
// template<typename T>
void FaultDiagnosis::SetInitError( const std::string& ifo){
  // if(error_codes_["3052006_init_error"].last_data_time<0.01){
    std::string final_error_info;
  // message_in.header.fault_vec.push_back(error_codes_["3032001_init_error"].CreatFault(ifo,final_error_info));
    error_codes_["3032001_init_error"].last_data_time=avos::common::NodeTime::Now().ToSecond();
    FaultHandle::FaultApi::Instance()->SetFaultCode(std::to_string(error_codes_["3032001_init_error"].error_code_id),final_error_info);
  // }
}
// template<typename T>
void FaultDiagnosis::SetTopicTimeOut(const std::string error_type, 
                                      const std::string& ifo){
  std::string final_error_info;
  if(error_codes_.find(error_type)!=error_codes_.end())
  {
    if(error_codes_[error_type].lost_ct<150)
    {
      error_codes_[error_type].lost_ct++;
    }

    if(error_codes_[error_type].lost_ct>=150)
    {
      // message_in.header.fault_vec.push_back(error_codes_[error_type].CreatFault(ifo,final_error_info));
      FaultHandle::FaultApi::Instance()->SetFaultCode(std::to_string(error_codes_[error_type].error_code_id),final_error_info);
    }

  }
}

// template<typename T>
void FaultDiagnosis::ResetError(const std::string error_type,double trigger_stamp){
  FaultHandle::FaultApi::Instance()->ResetFaultCode(std::to_string(error_codes_[error_type].error_code_id));
  error_codes_[error_type].last_data_time = trigger_stamp;
  error_codes_[error_type].lost_ct=0;
}

}//namespace semaperror
// }//namespace perception
// }//namespace avos
