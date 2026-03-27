#pragma once 

#include "log/log.h"
#include <string>
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <opencv2/core.hpp>

#include "perception_fusion_common/camera_service/camera_service_util.h"
#include "custom_stack.h"
namespace perception {


class PerCameraService {
public:
  PerCameraService(){}
  ~PerCameraService(){}
  bool IsInit(){return is_inited_;}
  void SetCoorHeight(const float& h){
    coor_height = h;
    // for(auto& u: cam_utils){
    //   u.ground_dh = h;
    // }
  }
  //根据配置文件初始化相机服务，输出结果为相机名称vec，其中vec索引为相机后续使用的 cam_id
  bool InitCamera(std::string yaml_path){
    std::map<std::string,CalibrationData> calibration_datas;
    // AINFO<<"to InitCamera:"<<project_config_path;
    if(!LoadFromProjcfg(calibration_datas))
    {
      if(FileExists(yaml_path))
      {
        if(!LoadFromYaml(yaml_path,calibration_datas))return false;
      }else
      {
        return false;
      }
    }
    
    cam_utils.resize(cam_names.size());
    for(int i=0; i<cam_names.size(); ++i){
      AINFO << "to load param for camera:"<<cam_names[i]<<" i:"<<i<<" cam_util size:"<<cam_utils.size();
      if(!cam_utils[i].LoadParams(calibration_datas, cam_names[i])){
        AINFO << "error while load param for camera:"<<cam_names[i];
        // cam_names.resize(i);//只保留前面的成功数据
        // cam_utils.resize(i);
        return false;
      }
    }
    if(!cam_names.empty()){
      AINFO<<"Succeed init cameras:"<<cam_names.size()<<":";
      for(auto& n:cam_names){
        AINFO<<n<<",";
      }
      AINFO;
    }
    is_inited_ = true;
    return true;
  }
  //根据vcs 3D坐标，投影到像素坐标中（不进行畸变矫正，速度稍快，但是可能精度不高）:
  bool ConvertPointVCS2IMG(const int& cam_id, const cv::Point3f &p3fVcs, cv::Point3f &p3fImg){
    if(cam_id<0 || cam_id>=cam_utils.size()){
      return false;
    }
    return cam_utils[cam_id].ConvertPointVCS2IMG(p3fVcs, p3fImg);
  }
  bool PointVcsToImg(const int& cam_id, const cv::Point3f &p3fVcs, cv::Point3f &p3fImg){
    return ConvertPointVCS2IMG(cam_id,p3fVcs,p3fImg);
  }
  //自车坐标系转相机系
  bool ConvertPointVCS2CamCoordinate(const int& cam_id, const cv::Point3f &p3fVcs, cv::Point3f &p3fcam){
    if(cam_id<0 || cam_id>=cam_utils.size()){
      return false;
    }
    return cam_utils[cam_id].ConvertPointVCS2CamCoordinate(p3fVcs, p3fcam);
  }
  //相机系转自车坐标系
  bool ConvertPointCamCoordinate2VCS(const int& cam_id, const cv::Point3f &p3fcam, cv::Point3f &p3fVcs){
    if(cam_id<0 || cam_id>=cam_utils.size()){
      return false;
    }
    return cam_utils[cam_id].ConvertPointCamCoordinate2VCS(p3fcam, p3fVcs);
  }

  //根据vcs 3D坐标，投影到像素坐标中（增加畸变矫正量，计算量稍大）:
  bool ConvertPointVCS2IMG_undistort(const int& cam_id, const cv::Point3f &p3fVcs, cv::Point3f &p3fImg){
    if(cam_id<0 || cam_id>=cam_utils.size()){
      return false;
    }
    return cam_utils[cam_id].ConvertPointVCS2IMG_undistort(p3fVcs, p3fImg);
  }
  //根据像素坐标，计算投影到地面点的3D坐标
  bool ConvertPointIMG2GND_undistort(const int& cam_id, const cv::Point3f &p3fImg, cv::Point3f &p3fVcs){
    if(cam_id<0 || cam_id>=cam_utils.size()){
      return false;
    }
    return cam_utils[cam_id].ConvertPointIMG2GND_undistort(p3fImg, p3fVcs);
  }

  //对原始像素坐标，剔除畸变影响，得到可用于内外参数计算的坐标：
  bool UnDistortImage(const int& cam_id, const cv::Point2f  &pt, cv::Point2f &upoint){
    if(cam_id<0 || cam_id>=cam_utils.size()){
      return false;
    }
    return cam_utils[cam_id].UnDistortImage(pt, upoint);
  }
  //对理想像素坐标，增加畸变影响，找到真实对应的原始像素坐标:
  bool PixelDenormalize(const int& cam_id, cv::Point3f &pt){
    if(cam_id<0 || cam_id>=cam_utils.size()){
      return false;
    }
    cam_utils[cam_id].PixelDenormalize(pt);
    return true;
  }
bool CheckValidity(const int& cam_id) const {
  if (is_inited_ == false) {
    std::cout << "[camera_service] is not inited." << std::endl;
    return false;
  }
  if (cam_id < 0 || cam_id >= cam_utils.size()) {
    std::cout << "request invalid camera index: " << cam_id << std::endl;
    return false;
  }
  return true;
}
  int camera_width(const int &cam_id) const {
    if (!CheckValidity(cam_id))
      return -1;
    return cam_utils[cam_id].camera_width;
  }

  int camera_height(const int &cam_id) const {
    if (!CheckValidity(cam_id))
      return -1;
    return cam_utils[cam_id].camera_height;
  }
private:
  bool LoadFromProjcfg(std::map<std::string,CalibrationData> &calibration_datas)
  {
    std::cout<<"Cam Init LoadFromProjcfg"<<std::endl;
    auto param_ptr = CustomStack::Instance();
    // std::string project_config_path = param_ptr->GetProjectConfigPath();= param_ptr->GetProjectConfigValue("vehicle_interface/body_base_height", body_base_height)
    if(!param_ptr->GetProjectConfigValue<std::vector<std::string>>("camera_names/",cam_names)){
      std::cout<<"get camera_names error"<<std::endl;
      return false;
    }
    //load 
    for(auto n:cam_names)
    {
      CalibrationData calibration_data;
      //Size
      if(!param_ptr->GetProjectConfigValue<std::vector<int>>(n+"/Size/",calibration_data.Size))
      {
        std::cout<<"cam_name:"<<n<<" cannt get Size fail\n";
        return false;
      }
      //new_Size
      if(!param_ptr->GetProjectConfigValue<std::vector<int>>(n+"/new_Size/",calibration_data.new_Size))
      {
        std::cout<<"cam_name:"<<n<<" cannt get new_Size fail\n";
      }
      //scaled_Size
      // if(!param_ptr->GetProjectConfigValue<std::vector<int>>(n+"/scaled_Size/",calibration_data.scaled_Size))
      // {
      //   std::cout<<"cam_name:"<<n<<" cannt get scaled_Size fail\n";
      // }
      //K
      if(!param_ptr->GetProjectConfigValue<std::vector<double>>(n+"/K/",calibration_data.K))
      {
        std::cout<<"cam_name:"<<n<<" cannt get K fail\n";
        return false;
      }
      //D
      if(!param_ptr->GetProjectConfigValue<std::vector<double>>(n+"/D/",calibration_data.D))
      {
        std::cout<<"cam_name:"<<n<<" cannt get D fail\n";
        return false;
      }
      //Epsilon
      if(!param_ptr->GetProjectConfigValue<double>(n+"/Epsilon/",calibration_data.Epsilon))
      {
        std::cout<<"cam_name:"<<n<<" cannt get Epsilon fail\n";
        return false;
      }
      //t
      if(!param_ptr->GetProjectConfigValue<std::vector<double>>(n+"/t/",calibration_data.t))
      {
        std::cout<<"cam_name:"<<n<<" cannt get t fail\n";
        return false;
      }
      //R_euler
      if(!param_ptr->GetProjectConfigValue<std::vector<double>>(n+"/R_euler/",calibration_data.R_euler))
      {
        std::cout<<"cam_name:"<<n<<" cannt get R_euler fail\n";
        return false;
      }
      //new_K
      if(!param_ptr->GetProjectConfigValue<std::vector<double>>(n+"/new_K/",calibration_data.new_K ))
      {
        std::cout<<"cam_name:"<<n<<" cannt get new_K fail\n";
        return false;
      }
      //vcs_to_ground_height
      if(!param_ptr->GetProjectConfigValue<double>(n+"/vcs_to_ground_height/",calibration_data.vcs_to_ground_height))
      {
        std::cout<<"cam_name:"<<n<<" cannt get vcs_to_ground_height fail\n";
        return false;
      }
      calibration_datas[n] = calibration_data;
      std::cout<<"cam_name:"<<n<<" done"<<std::endl;
    }
    return true;
  }
  bool LoadFromYaml(std::string yaml_path,std::map<std::string,CalibrationData> &calibration_datas){
    std::cout<<"Cam Init LoadFromYaml yaml-path:"<<yaml_path<<std::endl;
    if(FileExists(yaml_path))
    {
      YAML::Node node = YAML::LoadFile(yaml_path);
      if (node.IsNull()) {
        std::cout << "Load " << yaml_path << " failed! please check!"<<std::endl;
        return false;
      }
      const YAML::Node& names = node["camera_names"];
      if (!names.IsSequence()) {
        std::cout <<"File content error:"<< yaml_path<<":\ncamera_names is not a sequence"<<std::endl;
        return false;
      }
      for (const auto& name : names) {
        if (name.IsScalar()) {
          cam_names.push_back(name.as<std::string>());
        } else {
          std::cout << "Expected a scalar value for each name in 'camera_names' item."<<std::endl;
          return false;
        }
      }
      for(const auto &name:cam_names)
      {
        CalibrationData calibration_data;
        try {
          if(!node[name]["Size"])std::cout<<"cam:"<<name<<"'s item: 'Size' not exist."<<std::endl;
          calibration_data.Size = node[name]["Size"].as<std::vector<int>>();
          if(!node[name]["new_Size"])std::cout<<"cam:"<<name<<"'s item: 'new_Size' not exist."<<std::endl;
          calibration_data.new_Size = node[name]["new_Size"].as<std::vector<int>>();

          // calibration_data.new_Size = node[name]["new_Size"].as<std::vector<int>>();
          // calibration_data.scaled_Size = node[name]["scaled_Size"].as<std::vector<int>>();
          if(!node[name]["K"])std::cout<<"cam:"<<name<<"'s item: 'K' not exist."<<std::endl;
          calibration_data.K = node[name]["K"].as<std::vector<double>>();
          if(!node[name]["D"])std::cout<<"cam:"<<name<<"'s item: 'D' not exist."<<std::endl;
          calibration_data.D = node[name]["D"].as<std::vector<double>>();
          if(!node[name]["Epsilon"])std::cout<<"cam:"<<name<<"'s item: 'Epsilon' not exist."<<std::endl;
          calibration_data.Epsilon = node[name]["Epsilon"].as<double>();
          if(!node[name]["t"])std::cout<<"cam:"<<name<<"'s item: 't' not exist."<<std::endl;
          calibration_data.t = node[name]["t"].as<std::vector<double>>();
          if(!node[name]["R_euler"])std::cout<<"cam:"<<name<<"'s item: 'R_euler' not exist."<<std::endl;
          calibration_data.R_euler = node[name]["R_euler"].as<std::vector<double>>();
          if(!node[name]["new_K"])std::cout<<"cam:"<<name<<"'s item: 'new_K' not exist."<<std::endl;
          calibration_data.new_K = node[name]["new_K"].as<std::vector<double>>();
          if(!node[name]["vcs_to_ground_height"])std::cout<<"cam:"<<name<<"'s item: 'vcs_to_ground_height' not exist."<<std::endl;
          calibration_data.vcs_to_ground_height = node[name]["vcs_to_ground_height"].as<double>();
          // calibration_data.base_euler = node[name]["base_euler"].as<double>();
          calibration_datas[name] = calibration_data;
        }catch (YAML::Exception &e) {
          std::cout << "load camera"<<name<<"'s intrinsic with error, YAML exception: " << e.what()<<std::endl;
          return false;
        }
      }
    }
    else
    {
      return false;
    }
    return true;
  }
  //判断文件是否存在:  
  bool FileExists(const std::string file_path) {
    std::ifstream ifs(file_path);
    if (!ifs.is_open()) {
      std::cout << "PerCameraService: opening failed. check if " << file_path << " exists"<<std::endl;
      std::cout<<"File not exists:"<<file_path<<std::endl;
      return false;
    } else {
      std::cout << "PerCameraService read file: parsing yaml " << file_path << std::flush<<std::endl;
    }
    ifs.close();
    return true;
  }
 bool is_inited_ = false;
public:
  std::vector<std::string> cam_names;
  std::vector<PerCameraServerUtile> cam_utils;
  float coor_height=0.25;//车体坐标系距离地面典型高度
};

}  // namespace perception

