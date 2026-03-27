#pragma once

#include "log/log.h"
#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>   // cv::line
#include <opencv2/highgui.hpp>   // cv::imshow, cv::waitKey
#include <opencv2/calib3d.hpp> 
#include <string>

namespace perception {

#define MODEL_PINHOLE 0
#define MODEL_FISHEYE 1
 struct DistortionParam {
  int   camera_model = 0;
  float params_d[5] = {0, 0, 0, 0, 0};
  float epsilon = 0;
} ;

struct CalibrationData{
  std::vector<int> Size;
  std::vector<int> new_Size;
  std::vector<int> scaled_Size; // 相机去畸变后的分辨率（非GDC方案，数据平台中用）
  std::vector<double> K;// 相机内参矩阵
  std::vector<double> D;//相机畸变系数
  double Epsilon;//: 0 # 全相机模型校正系数，采用针孔相机时本系数为0
  std::vector<double> t;//: [0.093, 1.305, -1.836] # 相机位于车体坐标系(前左上)坐标为(x,y,z) 则本处填写[y, z, -x]
  std::vector<double> R_euler;//: [4.0, 0.0, 0.0] # pitch, yaw, roll  单位:度 pitch>0,点云上升；yaw>0 点云右移； roll>0 点云左升右降
  std::vector<double> new_K;//: [1920, 0, 1920, 0, 1920, 960, 0, 0, 1] # 标准相机K
  double base_euler;//: 0.0 # 理想坐标系相对前向基础相机的理想yaw角的偏差
  double vcs_to_ground_height{0.0};//后轴中心到地面高度。
};

class PerCameraServerUtile {
public:
  PerCameraServerUtile(){}
  ~PerCameraServerUtile(){}
  //读取当前相机的标定参数:
  bool LoadParams(std::map<std::string,CalibrationData> calibration_data, const std::string &cam_name){
    std::cout<<"start InitCamera:"<<cam_name<<std::endl;
    camera_name = cam_name;
    std::cout<<"LoadCameraIntrinsics :"<<cam_name<<std::endl;
    try {
      camera_width = calibration_data[cam_name].Size[0]; // 表示标定时图像的大小
      camera_height = calibration_data[cam_name].Size[1];// 表示标定时图像的大小
      model_width = calibration_data[cam_name].new_Size[0]; // 表示模型输出时图像的大小
      model_height = calibration_data[cam_name].new_Size[1];// 表示模型输出时图像的大小
      vcs_to_ground_height = calibration_data[cam_name].vcs_to_ground_height;
      Eigen::VectorXf params_k(9);
      for (size_t i = 0; i < 9; ++i) {
        params_k(i) = calibration_data[cam_name].K[i];
      }
      // K&D 值均为在Size: original_width_ & original_height_ 尺寸下标定的参数
      intr_K_.row(0) << params_k(0), params_k(1), params_k(2);
      intr_K_.row(1) << params_k(3), params_k(4), params_k(5);
      intr_K_.row(2) << params_k(6), params_k(7), params_k(8);

      // new_width_ = calibration_data[cam_name]["new_Size"][0].as<int>();// 表示去完畸变后图像大小，经过GDC去畸变，resize之后输出的图像大小
      // new_height_ = calibration_data[cam_name]["new_Size"][1].as<int>();// 表示去完畸变后图像大小，经过GDC去畸变，resize之后输出的图像大小
      // // 下面代码只在图像经过GDC去畸变后的工程中是有效的，在带畸变的功能中所有全局变量无效
      // Eigen::VectorXf params_new_k(9);
      // for (size_t i = 0; i < 9; ++i) {
      //   params_new_k(i) = calibration_data[cam_name]["new_K"][i].as<float>();
      // }
      // new_intr_K_.row(0) << params_new_k(0), params_new_k(1), params_new_k(2);
      // new_intr_K_.row(1) << params_new_k(3), params_new_k(4), params_new_k(5);
      // new_intr_K_.row(2) << params_new_k(6), params_new_k(7), params_new_k(8);
      // 需要区分pinhole和fisheye
      float this_epilon=calibration_data[cam_name].Epsilon;
      // if (calibration_data[cam_name]["Epsilon"]) {
      //   this_epilon = calibration_data[cam_name]["Epsilon"].as<float>();
      // }
      if (this_epilon>0.1) {
        intr_D_.camera_model = MODEL_FISHEYE;
        intr_D_.epsilon = this_epilon;
        Eigen::VectorXf params_d(4);
        for (size_t i = 0; i < 4; ++i) {
          intr_D_.params_d[i] = calibration_data[cam_name].D[i];
        }
        intr_D_.params_d[4] =0;
      } else {
        intr_D_.camera_model = MODEL_PINHOLE;
        Eigen::VectorXf params_d(5);
        for (size_t i = 0; i < 5; ++i) {
          intr_D_.params_d[i] = calibration_data[cam_name].D[i];
        }
      }
    } catch (YAML::Exception &e) {
      std::cout << "load camera"<<cam_name<<"'s intrinsic with error, YAML exception: " << e.what()<<std::endl;
      return false;
    }
    std::cout<<"LoadCameraIntrinsics ok:"<<cam_name<<std::endl;
    if(camera_width==1920 && camera_height==1080){
      img_to_model_cof = 1.0/3;
      img_to_model_cut_y = 120;
      std::cout<<"model_width:"<<model_width<<std::endl;
      std::cout<<"camera:"<<cam_name<<" size match to [1920x1080]"<<std::endl;
    }else if(camera_width==1280 && camera_height==720){
      img_to_model_cof = 1.0/2;
      img_to_model_cut_y = 80;
      std::cout<<"camera:"<<cam_name<<" size match to [1280x720]"<<std::endl;
    }else if(camera_width==3840 && camera_height==2160){
      img_to_model_cof = 1.0/6;
      img_to_model_cut_y = 240;
      std::cout<<"camera:"<<cam_name<<" size match to [3840x2160]"<<std::endl;
    }else{
      img_to_model_cof=1.0;
      img_to_model_cut_y=0;
      std::cout<<"camera:"<<cam_name<<" size not match to known [1920x1080 or 1280x720]"<<std::endl;
    }
    //load extrins
    float x, y, z;
    float exter_roll,exter_pitch,exter_yaw;
    std::cout<<"LoadCameraExtrinsics:"<<cam_name<<std::endl;
    Eigen::VectorXf params_t(3), params_euler(3);
    try {
      for (int i = 0; i < 3; ++i) {
        params_t(i) = calibration_data[cam_name].t[i];
        params_euler(i) = calibration_data[cam_name].R_euler[i];
      }
      params_euler = params_euler * degree2arc_;
    } catch (const YAML::Exception &e) {
      std::cout << "load camera"<<cam_name<<"'s extrinsic with error, YAML exception: " << e.what()<<std::endl;
      return false;
    }
    std::cout<<"LoadCameraExtrinsics ok:"<<cam_name<<std::endl;
    // 初始旋转矩阵，需要根据相机进行坐标系调整：
    float roll = 90 * M_PI / 180; // 3-x-roll
    float pitch = -90 * M_PI / 180; // 2-y-pitch
    float yaw = 0 * M_PI / 180; // 1-z-yaw
    extr_R_init_ = Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitZ())
                  * Eigen::AngleAxisf(pitch,  Eigen::Vector3f::UnitY())
                  * Eigen::AngleAxisf(roll, Eigen::Vector3f::UnitX());
    Eigen::Quaternionf rotation =
      (Eigen::AngleAxisf(params_euler.z(), // pitch
                        Eigen::Vector3f::UnitZ()) *
      Eigen::AngleAxisf(params_euler.y(), // 
                        Eigen::Vector3f::UnitY()) *
      Eigen::AngleAxisf(params_euler.x(), // 
                        Eigen::Vector3f::UnitX()));
    extr_R_ = rotation.matrix();
    
    extr_t_.col(0) << params_t(0), params_t(1), params_t(2);
    std::cout<<"Succeed InitCamera."<<cam_name<<std::endl;
    //计算中间变量，从而避免每次调用都重复计算：

    M_Mat_ = intr_K_ * extr_R_ * extr_R_init_ ;
    M_Mat_inv_ = M_Mat_.inverse();
    Eigen::Matrix<float, 3, 1>  p_4 = intr_K_ * extr_R_ * (extr_t_+Eigen::Vector3f(0, vcs_to_ground_height, 0)) ;//vcs_to_ground_height
    C_tilde_ = M_Mat_inv_ * p_4;
    return true;
  }
private:
    Eigen::Matrix<float, 3, 3> M_Mat_;
    Eigen::Matrix<float, 3, 3> M_Mat_inv_ ;
    Eigen::Matrix<float, 3, 1> C_tilde_ ;
public:    
  // 3D坐标（vcs）转图像坐标:
  bool ConvertPointVCS2IMG(const cv::Point3f &p3fVcs, cv::Point3f &p3fImg, const bool debug=false){
    Eigen::Matrix<float, 3, 1> point_vcs;
    Eigen::Matrix<float, 3, 1> point_camera;
    if(debug){
      std::cout<<"vcs pt:("<<p3fVcs.x<<","<<p3fVcs.y<<","<<p3fVcs.z<<std::endl;
      std::cout<<"extr_t_:"<<std::endl;
      std::cout<<extr_t_<<std::endl;
      std::cout<<"extr_R_:"<<std::endl;
      std::cout<<extr_R_<<std::endl;
      std::cout<<"extr_R_init_:"<<std::endl;
      std::cout<<extr_R_init_<<std::endl;
      std::cout<<"intr_K_:"<<std::endl;
      std::cout<<intr_K_<<std::endl;
    }

    point_vcs << p3fVcs.x, p3fVcs.y, p3fVcs.z;
    point_camera = extr_R_ * ( extr_R_init_ * point_vcs + extr_t_);
    if(debug)std::cout<<"point_camera:"<<point_camera<<std::endl;
    if (point_camera[2] <= 0) {
      if(debug)std::cout<<"point_camera[2]:"<<point_camera[2]<<std::endl;
      p3fImg.z = -1;
      return false;
    }
    float x = point_camera[0] / point_camera[2];
    float y = point_camera[1] / point_camera[2];
    float depth = point_camera[2];
    if(fabs(x)>1.4){p3fImg.z = -2;return false;}//tan(x/z)<110,目前没有用到视场角大于120度的相机
    p3fImg = cv::Point3f(x, y, depth);
    if(debug)std::cout<<"x y depth:"<<x<<","<<y<<","<<depth<<std::endl;
    PixelDenormalize(p3fImg); //从相机系转换到图像系，并计算添加畸变
    if(debug)std::cout<<"x y:"<<p3fImg.x<<","<<p3fImg.y<<","<<p3fImg.z<<std::endl;

    if (p3fImg.x >= 0 && p3fImg.x < camera_width &&
        p3fImg.y >= 0 && p3fImg.y < camera_height) {
      p3fImg.z = -3;
      return true;
    }else{
      return false;
    }
  }

  //自车系转相机系
  bool ConvertPointVCS2CamCoordinate(const cv::Point3f &p3fVcs, cv::Point3f &p3fcam){
    Eigen::Matrix<float, 3, 1> point_vcs;
    Eigen::Matrix<float, 3, 1> point_camera;

    point_vcs << p3fVcs.x, p3fVcs.y, p3fVcs.z;
    point_camera = extr_R_ * ( extr_R_init_ * point_vcs + extr_t_);
    // if(debug)AINFO<<"point_camera:"<<point_camera;
    if (point_camera[2] <= 0) {
      p3fcam.z = -1;
      return false;
    }
    p3fcam.x = point_camera[0];
    p3fcam.y = point_camera[1];
    p3fcam.z = point_camera[2];
    return true;

  }
  //相机系转自车系
  bool ConvertPointCamCoordinate2VCS(const cv::Point3f &p3fcam, cv::Point3f &p3fVcs){
    Eigen::Matrix<float, 3, 1> point_vcs;
    Eigen::Matrix<float, 3, 1> point_camera;
    point_camera << p3fcam.x,p3fcam.y,p3fcam.z;
    // point_vcs << p3fVcs.x, p3fVcs.y, p3fVcs.z;
    // point_camera = extr_R_ * ( extr_R_init_ * point_vcs + extr_t_);
    point_vcs = extr_R_init_.inverse()*(extr_R_.inverse()*point_camera-extr_t_);
    // if(debug)AINFO<<"point_camera:"<<point_camera;
    // if (point_camera[2] <= 0) {
    //   //AINFO<<"point_camera[2]:"<<point_camera[2];
    //   p3fcam.z = -1;
    //   return false;
    // }
    p3fVcs.x = point_vcs[0];
    p3fVcs.y = point_vcs[1];
    p3fVcs.z = point_vcs[2];
    return true;
  }

  // 3D坐标（vcs）转图像坐标（考虑畸变，输出图像标准坐标并补充畸变）:
  bool ConvertPointVCS2IMG_undistort(const cv::Point3f &p3fVcs, cv::Point3f &p3fImg){
    Eigen::Matrix<float, 3, 1> point_vcs;
    Eigen::Matrix<float, 3, 1> point_camera;
    // bool debug=(fabs(p3fVcs.x-2.84)<0.12 && fabs(p3fVcs.y-1.81)<0.12);
    point_vcs << p3fVcs.x, p3fVcs.y, p3fVcs.z;
    point_camera = extr_R_ * ( extr_R_init_ * point_vcs + extr_t_);
    if (point_camera[2] <= 0) {
      // if(debug)AINFO<<"point_camera[2]:"<<point_camera[2];
      //AINFO<<"point_camera[2]:"<<point_camera[2];
      p3fImg.z = -1;
      return false;
    }
    float x = point_camera[0] / point_camera[2];
    float y = point_camera[1] / point_camera[2];
    float depth = point_camera[2];
    if(fabs(x)>1.4){p3fImg.z = -2;return false;}//tan(x/z)<110,目前没有用到视场角大于120度的相机
    p3fImg = cv::Point3f(x, y, depth);
    // if(debug)AINFO<<"x y depth:"<<x<<","<<y<<","<<depth;
    PixelDenormalize(p3fImg); // 从相机系转换到图像系，并添加畸变
    // if(debug)AINFO<<"x y:"<<p3fImg.x<<","<<p3fImg.y<<","<<p3fImg.z;

    if (p3fImg.x >= 0 && p3fImg.x < camera_width &&
        p3fImg.y >= 0 && p3fImg.y < camera_height) {
      p3fImg.z = -3;
      return true;
    }else{
      return false;
    }
  }
  //图像坐标转3D坐标（vcs）地面，（不考虑畸变，精度可能不足）:
  bool ConvertPointIMG2GND(const cv::Point3f &p3fImg, cv::Point3f &p3fVcs){
    // Eigen::Matrix<float, 3, 3> M_Mat_;
    // M_Mat_ = intr_K_ * extr_R_ * extr_R_init_ ;
    // Eigen::Matrix<float, 3, 1> p_4_;
    // p_4_ = intr_K_ * extr_R_ * extr_t_ ;
    // auto M_Mat_inv_ = M_Mat_.inverse();
    // auto C_tilde_ = M_Mat_inv_ * p_4_;

    Eigen::Matrix<float, 3, 1> p_in_camera;
    p_in_camera(0, 0) = p3fImg.x;
    p_in_camera(1, 0) = p3fImg.y;
    p_in_camera(2, 0) = 1;
    Eigen::Matrix<float, 3, 1> X_Tilde = M_Mat_inv_ * p_in_camera;

    // 地平线以上的点会投影到相机后面,统一把这些点输出000
    if(X_Tilde(2) > 0) {
      p3fVcs = cv::Point3f(0,0,0);
      return false;
    }

    float mue_N = C_tilde_(2, 0) / X_Tilde(2, 0);
    // float mue_N = (z_offset_ + C_tilde_(2, 0)) / X_Tilde(2, 0);
    Eigen::Matrix<float, 3, 1> point = mue_N * X_Tilde - C_tilde_;

    p3fVcs.x = point[0];
    p3fVcs.y = point[1];
    p3fVcs.z = point[2];
    return true;
  }
  //图像坐标转3D坐标（vcs）地面，（考虑畸变）:


  bool ConvertPointIMG2GND_undistort(const cv::Point3f &p3fImg, cv::Point3f &p3fVcs){
    cv::Point2f  pt, upoint;
    pt.x = p3fImg.x;
    pt.y = p3fImg.y;
    UnDistortImage(pt, upoint);
    //if (upoint.x <= 0 || upoint.x >= scaled_width_ || upoint.y <= 0 || upoint.y >= scaled_height_) {
    //  return false;
    //}
    // Eigen::Matrix<float, 3, 3> M_Mat_;
    // M_Mat_ = intr_K_ * extr_R_ * extr_R_init_;
    // Eigen::Matrix<float, 3, 1> p_4_;
    // p_4_ = intr_K_ * extr_R_ * extr_t_ ;
    // auto M_Mat_inv_ = M_Mat_.inverse();
    // auto C_tilde_ = M_Mat_inv_ * p_4_;

    Eigen::Matrix<float, 3, 1> p_in_camera;
    p_in_camera(0, 0) = upoint.x;
    p_in_camera(1, 0) = upoint.y;
    p_in_camera(2, 0) = 1;
    Eigen::Matrix<float, 3, 1> X_Tilde = M_Mat_inv_ * p_in_camera;
    
    // 地平线以上的点会投影到相机后面,统一把这些点输出000
    if(X_Tilde(2) > 0) {
      p3fVcs = cv::Point3f(0,0,0);
      // std::cout<<"ConvertPointIMG2GND_undistort return X_Tilde(2):"<<X_Tilde(2)<<std::endl;
      return false;
    }

    float mue_N = C_tilde_(2, 0) / X_Tilde(2, 0);
    // float mue_N = (z_offset_ + C_tilde_(2, 0)) / X_Tilde(2, 0);
    Eigen::Matrix<float, 3, 1> point = mue_N * X_Tilde - C_tilde_;

    p3fVcs.x = point[0];
    p3fVcs.y = point[1];
    p3fVcs.z = point[2];
    
    return true;
  }
    
  std::vector<bool> ConvertPointsIMG2GND_undistort(const std::vector<cv::Point2f> &p3fImgs, 
                                                std::vector<cv::Point3f> &p3fVcs) {
    size_t num_points = p3fImgs.size();
    p3fVcs.resize(num_points);
    std::vector<bool> valid_flags(num_points, false);
    
    if (num_points == 0) {
        return valid_flags;
    }
    
    const bool debug_ = false;
    
    if (debug_) {
        std::cout << "ConvertPointsIMG2GND_undistort processing " << num_points << " points" << std::endl;
    }
    
    // 提前计算固定矩阵和向量（只需计算一次）
    // Eigen::Matrix<float, 3, 3> M_Mat_ = intr_K_ * extr_R_ * extr_R_init_;
    // Eigen::Matrix<float, 3, 1> p_4_ = intr_K_ * extr_R_ * extr_t_;
    // Eigen::Matrix<float, 3, 3> M_Mat_inv_ = M_Mat_.inverse();
    // Eigen::Vector3f C_tilde_ = M_Mat_inv_ * p_4_;
    
    if (debug_) {
        // std::cout << "M_Mat_:" << std::endl << M_Mat_ << std::endl;
        std::cout << "M_Mat_inv_:" << std::endl << M_Mat_inv_ << std::endl;
        std::cout << "C_tilde_: " << C_tilde_.transpose() << std::endl;
    }
    
    // 处理每个图像点
    for (size_t i = 0; i < num_points; ++i) {
        cv::Point2f pt, upoint;
        pt.x = p3fImgs[i].x;
        pt.y = p3fImgs[i].y;
        
        // 去畸变处理
        UnDistortImage(pt, upoint);
        
        if (debug_ && i < 3) {
            std::cout << "Point " << i << " - original: (" << pt.x << "," << pt.y 
                      << "), undistorted: (" << upoint.x << "," << upoint.y << ")" << std::endl;
        }
        
        // 检查去畸变后的点是否在图像范围内（可选）
        // if (upoint.x <= 0 || upoint.x >= scaled_width_ || upoint.y <= 0 || upoint.y >= scaled_height_) {
        //   continue;
        // }
        
        // 构造图像齐次坐标
        Eigen::Matrix<float, 3, 1> p_in_camera;
        p_in_camera(0, 0) = upoint.x;
        p_in_camera(1, 0) = upoint.y;
        p_in_camera(2, 0) = 1.0f;
        
        // 计算 X_Tilde = M_Mat_inv_ * p_in_camera
        Eigen::Matrix<float, 3, 1> X_Tilde = M_Mat_inv_ * p_in_camera;
        
        // 地平线以上的点会投影到相机后面
        if (X_Tilde(2) > 0) {
            if (debug_ && i < 5) {
                std::cout << "Point " << i << " - Behind horizon, X_Tilde(2): " << X_Tilde(2) << std::endl;
            }
            p3fVcs[i] = cv::Point3f(0, 0, 0);
            valid_flags[i] = false;
            continue;
        }
        
        // 计算 mue_N = C_tilde_(2) / X_Tilde(2)
        float mue_N = C_tilde_(2) / X_Tilde(2);
        
        // 计算3D点坐标
        Eigen::Matrix<float, 3, 1> point = mue_N * X_Tilde - C_tilde_;
        
        p3fVcs[i].x = point[0];
        p3fVcs[i].y = point[1];
        p3fVcs[i].z = point[2];
        valid_flags[i] = true;
        
        if (debug_ && i < 3) {
            std::cout << "Point " << i << " - X_Tilde: " << X_Tilde.transpose() << std::endl;
            std::cout << "Point " << i << " - mue_N: " << mue_N << std::endl;
            std::cout << "Point " << i << " - 3D point: " << point.transpose() << std::endl;
            std::cout << "Success: Point " << i << ": img=(" << p3fImgs[i].x << "," << p3fImgs[i].y 
                      << "), 3D=(" << p3fVcs[i].x << "," << p3fVcs[i].y << "," << p3fVcs[i].z << ")" << std::endl;
        }
    }
    
    // 统计有效点数量并分析结果
    // int valid_count = 0;
    // float min_x = std::numeric_limits<float>::max();
    // float max_x = std::numeric_limits<float>::lowest();
    // float min_y = std::numeric_limits<float>::max();
    // float max_y = std::numeric_limits<float>::lowest();
    // float min_z = std::numeric_limits<float>::max();
    // float max_z = std::numeric_limits<float>::lowest();
    
    // for (size_t i = 0; i < num_points; ++i) {
    //     if (valid_flags[i]) {
    //         valid_count++;
            // min_x = std::min(min_x, p3fVcs[i].x);
            // max_x = std::max(max_x, p3fVcs[i].x);
            // min_y = std::min(min_y, p3fVcs[i].y);
            // max_y = std::max(max_y, p3fVcs[i].y);
            // min_z = std::min(min_z, p3fVcs[i].z);
            // max_z = std::max(max_z, p3fVcs[i].z);
        // }
    // }
    
    // std::cout << "ConvertPointsIMG2GND_undistort results:" << std::endl;
    // std::cout << "Valid points: " << valid_count << "/" << num_points << std::endl;
    // if (valid_count > 0) {
        // std::cout << "X range: [" << min_x << ", " << max_x << "] meters" << std::endl;
        // std::cout << "Y range: [" << min_y << ", " << max_y << "] meters" << std::endl;
        // std::cout << "Z range: [" << min_z << ", " << max_z << "] meters" << std::endl;
        
        // // 输出前几个有效点的详细信息
        // int count = 0;
        // for (size_t i = 0; i < num_points && count < 3; ++i) {
        //     if (valid_flags[i]) {
        //         std::cout << "Valid point " << count << ": img=(" << p3fImgs[i].x << "," << p3fImgs[i].y 
        //                   << "), 3D=(" << p3fVcs[i].x << "," << p3fVcs[i].y << "," << p3fVcs[i].z << ")" << std::endl;
        //         count++;
        //     }
        // }
    // }
    
    return valid_flags;
}
  //3D坐标（vcs）转模型对应的图像坐标（一般比原始图片要小很多）（不考虑畸变，输出图像标准坐标，精度不足）:
  bool ConvertPointVCS2ModelIMG(const cv::Point3f &p3fVcs, cv::Point3f &p3fImg){
    if(ConvertPointVCS2IMG(p3fVcs, p3fImg)){
      //先转化到原始图像坐标
      //再缩放到模型对应的图像坐标:
      TransOrinImgPosToModel(p3fImg);
      // AINFO<<"p3fVcs("<<p3fVcs.x<<","<<p3fVcs.y<<","<<p3fVcs.z<<") get img pos("<<p3fImg.x<<","<<p3fImg.y<< ").";
      return true;
    }else{
      // AINFO<<"p3fVcs("<<p3fVcs.x<<","<<p3fVcs.y<<","<<p3fVcs.z<<") faild to get img pos.";
      return false;
    }
  }

  //对原始像素坐标，剔除畸变影响，得到可用于内外参数计算的坐标：
  bool UnDistortImage(const cv::Point2f &pt, cv::Point2f &upoint) {
      if (intr_D_.camera_model == MODEL_PINHOLE) {
          return UndistortPointPinhole(pt, upoint);
      } else if (intr_D_.camera_model == MODEL_FISHEYE) {
          return UndistortPointFisheye(pt, upoint);
      }
      return false;
  }
  //将相机坐标转换到像素坐标，增加畸变影响，找到真实对应的原始像素坐标:
  void PixelDenormalize(cv::Point3f &pt) {
      if (intr_D_.camera_model == MODEL_PINHOLE) {
          ProjectInImagePinhole(pt);
      } else if (intr_D_.camera_model == MODEL_FISHEYE) {
          ProjectInImageFisheye(pt);
      }
  }
  void TransOrinImgPosToModel(cv::Point &pt){
    pt.x = pt.x*img_to_model_cof;
    pt.y = (pt.y-img_to_model_cut_y)*img_to_model_cof;
  }
  void TransOrinImgPosToModel(cv::Point2f &pt){
    pt.x = pt.x*img_to_model_cof;
    pt.y = (pt.y-img_to_model_cut_y)*img_to_model_cof;
  }
  void TransOrinImgPosToModel(cv::Point3f &pt){
    pt.x = pt.x*img_to_model_cof;
    pt.y = (pt.y-img_to_model_cut_y)*img_to_model_cof;
  }
private:
  //剔除畸变影响
  bool UndistortPointFisheye(const cv::Point2f &pt, cv::Point2f &upoint){
    Eigen::Vector2f pp((pt.x * intr_K_(1, 1) - intr_K_(0, 2)*intr_K_(1, 1) - intr_K_(0, 1) * (pt.y - intr_K_(1, 2))) / (intr_K_(0, 0)*intr_K_(1, 1)), (pt.y - intr_K_(1, 2)) / intr_K_(1, 1)); //plane
    Eigen::Vector2f pu = pp;
    for (int j = 0; j < 4; j++) {
      float r2 = pu[0] * pu[0] + pu[1] * pu[1];
      float r4 = r2 * r2;
      pu[0] = (pp[0] - 2 * intr_D_.params_d[2] * pu[0] * pu[1] - intr_D_.params_d[3] * (r2 + 2 * pu[0] * pu[0])) / (1 + intr_D_.params_d[0] * r2 + intr_D_.params_d[1] * r4);
      pu[1] = (pp[1] - 2 * intr_D_.params_d[3] * pu[0] * pu[1] - intr_D_.params_d[2] * (r2 + 2 * pu[1] * pu[1])) / (1 + intr_D_.params_d[0] * r2 + intr_D_.params_d[1] * r4);
    }

    auto epsilon = intr_D_.epsilon;
    // project to unit sphere
    float r2 = pu[0] * pu[0] + pu[1] * pu[1];
    float a = r2 + 1;
    float b = 2 * epsilon * r2;
    float c = r2 * epsilon * epsilon - 1;

    if (b * b - 4 * a * c < 0) {
      upoint.x = 1e5;
      upoint.y = 1e5;
      return false;
    }

    float Zs = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
    Eigen::Vector3f Xw(pu[0] * (Zs + epsilon), pu[1] * (Zs + epsilon), Zs);

    if (Xw[2] < 0) {
      upoint.x = 1e5;
      upoint.y = 1e5;
      return false;
    }
    upoint.x = Xw[0] / Xw[2] * intr_K_(0, 0) + intr_K_(0, 2);
    upoint.y = Xw[1] / Xw[2] * intr_K_(1, 1) + intr_K_(1, 2);
    // std::cout<<"UndistortPointFisheye in:"<<pt.x<<","<<pt.y<<", out:"<<upoint.x<<","<<upoint.y<<std::endl;
    return true;
  }
  bool UndistortPointPinhole(const cv::Point2f &pt, cv::Point2f &upoint) {
    const cv::Mat K = ( cv::Mat_<double> ( 3, 3 ) << intr_K_(0, 0), intr_K_(0, 1), intr_K_(0, 2),
                        intr_K_(1, 0), intr_K_(1, 1), intr_K_(1, 2),
                        intr_K_(2, 0), intr_K_(2, 1), intr_K_(2, 2) );

    const cv::Mat D = ( cv::Mat_<double> ( 5, 1 ) << intr_D_.params_d[0], intr_D_.params_d[1], intr_D_.params_d[2], intr_D_.params_d[3], intr_D_.params_d[4] );

    cv::Mat mat(1, 2, CV_32F);
    mat.at<float>(0, 0) = pt.x;
    mat.at<float>(0, 1) = pt.y;
    mat = mat.reshape(2);
    cv::undistortPoints(mat, mat, K, D, cv::Mat(), K);
    mat = mat.reshape(1);
    upoint.x = mat.at<float>(0, 0);
    upoint.y = mat.at<float>(0, 1);
    return true;
  }
  //增加畸变影响
  void ProjectInImagePinhole(cv::Point3f &pt){
    Eigen::Matrix<double, 2, 1> pt2d(pt.x, pt.y);
    double r_sq = pt2d[0] * pt2d[0] + pt2d[1] * pt2d[1];

    auto params_d = intr_D_.params_d;
    Eigen::Matrix<double, 2, 1> pt2d_radial =
      pt2d * (1 + params_d[0] * r_sq + params_d[1] * r_sq * r_sq +
              params_d[4] * r_sq * r_sq * r_sq);
    Eigen::Matrix<double, 2, 1> dpt2d;
    dpt2d[0] = 2 * params_d[2] * pt2d[0] * pt2d[1] +
              params_d[3] * (r_sq + 2 * pt2d[0] * pt2d[0]);
    dpt2d[1] = params_d[2] * (r_sq + 2 * pt2d[1] * pt2d[1]) +
              2 * params_d[3] * pt2d[0] * pt2d[1];

    Eigen::Matrix<double, 2, 1> pt2d_distort;
    pt2d_distort[0] = pt2d_radial[0] + dpt2d[0];
    pt2d_distort[1] = pt2d_radial[1] + dpt2d[1];

    // to image pixel
    // transform to image coordinates
    const double fx = intr_K_(0, 0);
    const double fy = intr_K_(1, 1);
    const double cx = intr_K_(0, 2);
    const double cy = intr_K_(1, 2);
    // std::cout<<"ProjectInImagePinhole:"<<pt.x<<","<<pt.y;
    pt.x = pt2d_distort[0] * fx + cx;
    pt.y = pt2d_distort[1] * fy + cy;
    // std::cout<<" after:"<<pt.x<<","<<pt.y<<std::endl;
  }
  void ProjectInImageFisheye(cv::Point3f &pt){
    
    // float x[3] = {pt.x, pt.y, pt.z};
    // const float norm = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    // Eigen::Vector3f norm_dir(pt.x, pt.y, pt.z);
    Eigen::Vector3f norm_dir(pt.x, pt.y, 1.0);
    norm_dir.normalize();

    auto params_d = intr_D_.params_d;
    auto epsilon = intr_D_.epsilon;
    // project to image plane
    float xu = norm_dir(0) / (norm_dir(2) + epsilon);
    float yu = norm_dir(1) / (norm_dir(2) + epsilon);
    // add distortion
    float r2 = xu * xu + yu * yu;
    float r4 = r2 * r2;

    Eigen::Vector2f pt2d_img;
    pt2d_img[0] = (1 + params_d[0] * r2 + params_d[1] * r4) * xu + 2 * params_d[2] * xu * yu + params_d[3] * (r2 + 2 * xu * xu);
    pt2d_img[1] = (1 + params_d[0] * r2 + params_d[1] * r4) * yu + params_d[2] * (r2 + 2 * yu * yu) + 2 * params_d[3] * xu * yu;

    // to image pixel
    // transform to image coordinates
    const float fx = intr_K_(0, 0);
    const float fy = intr_K_(1, 1);
    const float cx = intr_K_(0, 2);
    const float cy = intr_K_(1, 2);
    const float s = intr_K_(0, 1);
    // std::cout<<"ProjectInImageFisheye:"<<pt.x<<","<<pt.y;
    pt.x = fx * pt2d_img[0] + s * pt2d_img[1] + cx;
    pt.y = fy * pt2d_img[1] + cy;
    // std::cout<<" after:"<<pt.x<<","<<pt.y<<std::endl;
  }

public:
  int   camera_width = 0.0;  // 相机分辨率（相机原分辨率）
  int   camera_height = 0.0; // 相机分辨率（相机原分辨率）
  int model_width = 0.0; // 模型分辨率（视觉模型输出图像分辨率）
  int model_height = 0.0;// 模型分辨率（视觉模型输出图像分辨率）
  // float ground_dh = 0.3;
  std::string camera_name;

public:
  const double mypi_ = 3.141592653589793238;
  const double degree2arc_ = mypi_ / 180.0;
  int          scaled_width_ = 0.0;  // 相机去畸变后的分辨率（非GDC方案，数据平台中用）
  int          scaled_height_ = 0.0; // 相机去畸变后的分辨率（非GDC方案，数据平台中用）
  Eigen::Matrix<float, 3, 3> intr_K_;
  DistortionParam            intr_D_;
  Eigen::Matrix<float, 3, 3> extr_R_init_;
  Eigen::Matrix<float, 3, 3> extr_R_;
  Eigen::Matrix<float, 3, 1> extr_t_; // to ground
  float vcs_to_ground_height{0.0};

  float img_to_model_cof;
  int img_to_model_cut_y;
  // int new_width_ = 0.0; //相机被模型使用的分辨率，本模块一般不用
  // int new_height_ = 0.0;//相机被模型使用的分辨率，本模块一般不用
  // Eigen::Matrix<float, 3, 3> new_intr_K_;
};

} // namespace perception
