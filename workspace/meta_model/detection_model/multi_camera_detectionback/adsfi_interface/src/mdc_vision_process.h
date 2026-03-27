#pragma once 

#include <vector>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp> 
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>
#include <map>
// #include "SensorImageList.h"
// #include "PerceptionVPResult.h"
#include "custom_stack.h"//获取gsds配置参数

#include "adsfi_interface/src/protocol/vision_detection2usr_protocol.h"
#include "vision_perception.h"
#include "base_config.h"
#include "fault_diagnosis.h"
#include "perception_fusion_common/common/visionperception.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "PerceptionVPResult.h"
#include "PerceptionCameraResult.h"
#include "SensorImage.h"
namespace avos{
namespace perception{

class mdc_vision_process
{
public:
  int init_status=-1;
  mdc_vision_process(){}
  ~mdc_vision_process(){}
  int Init(std::string& error_info,std::shared_ptr<pertarget::FaultDiagnosis> error_handle_ptr);
  int Process(const ara::adsfi::MsgImageDataList &image_list, 	// 视觉图像输入列表	
              PerceptionVPResult &vpresults,
              ara::adsfi::MsgImageDataList& debug_out,
              std::string& error_info);
private:
  bool IsFrequency10hz(const ara::adsfi::MsgImageDataList &image_list);
  int GetVPResoult(PerceptionVPResult &vpresults);
  BaseConfig base_config_;
  std::string ascend_config_file_;
  std::string ascend_vision_yaml_;
  std::string ascend_vision_so_;
  std::string camera_params_file_;
  std::string inTopic_camnames_;
  bool do_camname_remap_;
  bool append_cameras_;
  bool debug_vis_;
  bool use_offline_imgs_=false;
  std::string offline_imgs_path_;
  int start_file_id_=1;//217 340
  std::map<std::string, std::string> cam_name_remap_;
  std::vector<std::string > camera_yaml_names_;
  std::vector<cv::Point2i> camera_yaml_sizes_;
  VisionPerception* vision_per_ptr=nullptr;
  std::vector<std::vector<uint8_t>> fake_data_;//适配相机，输出假数据
  long seq_=0;
  std::shared_ptr<pertarget::FaultDiagnosis> error_handle_ptr_;
  // PerceptionVPResult record_vpresults_;//记录上一次的vpresults，放置生成空数据
  cv::Mat yuv_image_;//离线加载图片时的缓存

  // 分割字符串函数定义
  std::vector<std::string> SplitString(const std::string &str, char delimiter) {
      std::vector<std::string> tokens;
      std::stringstream ss(str);
      std::string token;

      while (std::getline(ss, token, delimiter)) {
          tokens.push_back(token);
      }
      return tokens;
  }
  cv::Mat convertSensorImageToMat(const ara::adsfi::MsgImageData& simg) {
     // 检查数据大小 (YUV420SP 格式大小应为 width*height*3/2)
    size_t expected_size = simg.width * simg.height * 3 / 2;
    if (simg.dataSize != expected_size) {
        throw std::runtime_error("Invalid YUV420SP data size");
    }

    // 创建 YUV 矩阵 (单通道，高度为原始高度的1.5倍)
    cv::Mat yuv_mat(simg.height * 3 / 2, simg.width, CV_8UC1, (void*)simg.rawData.data());
    
    // 创建目标 RGB 矩阵
    cv::Mat rgb_mat(simg.height, simg.width, CV_8UC3);
    
    // 执行 YUV420SP 转 RGB
    cv::cvtColor(yuv_mat, rgb_mat, cv::COLOR_YUV2RGB_NV21);
    
    return rgb_mat;
  }
  int CV8C1MatToImage(const Mat& source, SensorImage& image) {
    image.height = source.rows;
    image.width = source.cols;
    image.encoding = "mono";
    int data_size = 1;
    image.step = data_size * source.cols; // Full row length in bytes
    if(base_config_.is_segment_compressed)// 结果是否压缩
    {
      std::string str = RLECompress((uint8_t *)(source.data()),source.rows * source.cols);
      image.data.resize(str.length());
      memcpy(image.data.data(), str.data(), str.length());
    }
    else{
      image.data.resize(source.rows * source.cols * data_size);
      memcpy(image.data.data(), source.data(), source.rows * source.cols * data_size);
    }
    return 0;
  }
  int ConverInferResults(NodePerceptionResult& infer, PerceptionVPResult& ret){
    const bool debug=false;
    int obj_ct=0;
    // std::cout<<"infer.node_name:"<<infer.node_name<<std::endl;
    ret.header.time = infer.trigger_stamp;
    if(debug)std::cout<<"ConverInferResults: infer  trigger:"<< infer.trigger_stamp<<" receive:"<<infer.receive_stamp<<" send:"<<infer.send_stamp <<std::endl;
    if(debug)std::cout<<"ConverInferResults: infer.camera_vision_objects:"<<infer.camera_vision_objects.size()<<std::endl;
    for(int i=0; i<infer.camera_vision_objects.size(); ++i){
      auto& cam_result=infer.camera_vision_objects[i];
      if(debug)std::cout<<"camera_vision_objects["<<i<<"] camera_name:"<<cam_result.camera_name<<" valid:"<<cam_result.valid<<std::endl;
      if(cam_result.valid==false){
        continue;
      }
      if(debug)std::cout<<"i:"<<i<<" time:"<<cam_result.time<<" trigger:"<< cam_result.trigger_stamp<<" receive:"<<cam_result.receive_stamp<<" send:"<<cam_result.send_stamp <<std::endl;
      PerceptionCameraResult pcam;
      pcam.valid=true;
      pcam.stamp = cam_result.trigger_stamp;
      pcam.camera_name=cam_result.camera_name;
      pcam.vision_objects.resize(cam_result.vision_objects.size()); // std::vector<PerceptionVisionObject>
      obj_ct += pcam.vision_objects.size();
      for(int j=0; j<cam_result.vision_objects.size(); ++j){
        pcam.vision_objects[j].source_crop = cam_result.vision_objects[j].source_crop;
        pcam.vision_objects[j].object_2d.obj_id = cam_result.vision_objects[j].object_2d.obj_id; //PerceptionObject2D
        pcam.vision_objects[j].object_2d.child_id = cam_result.vision_objects[j].object_2d.child_id; //todo
        pcam.vision_objects[j].object_2d.prob = cam_result.vision_objects[j].object_2d.prob; //todo
        pcam.vision_objects[j].object_2d.prob2d = cam_result.vision_objects[j].object_2d.prob2d; //todo
        pcam.vision_objects[j].object_2d.prob3d = cam_result.vision_objects[j].object_2d.prob3d; //todo
        pcam.vision_objects[j].object_2d.x = cam_result.vision_objects[j].object_2d.x; //todo
        pcam.vision_objects[j].object_2d.y = cam_result.vision_objects[j].object_2d.y; //todo
        pcam.vision_objects[j].object_2d.w = cam_result.vision_objects[j].object_2d.w; //todo
        pcam.vision_objects[j].object_2d.h = cam_result.vision_objects[j].object_2d.h; //todo
        pcam.vision_objects[j].object_2d.orientation_enable = cam_result.vision_objects[j].object_2d.orientation_enable; //todo
        pcam.vision_objects[j].object_2d.orientation_id = cam_result.vision_objects[j].object_2d.orientation_id; //todo
        pcam.vision_objects[j].object_2d.orientation_score = cam_result.vision_objects[j].object_2d.orientation_score; //todo
        pcam.vision_objects[j].object_2d.vrotate_enable = cam_result.vision_objects[j].object_2d.vrotate_enable; //todo
        pcam.vision_objects[j].object_2d.vrotate = cam_result.vision_objects[j].object_2d.vrotate; //todo
        pcam.vision_objects[j].object_2d.vrotate_score = cam_result.vision_objects[j].object_2d.vrotate_score; //todo
        pcam.vision_objects[j].object_2d.key_points_enable = cam_result.vision_objects[j].object_2d.key_points_enable; //todo
        pcam.vision_objects[j].object_2d.key_points.resize(cam_result.vision_objects[j].object_2d.key_points.size());
        for(int k=0; k<cam_result.vision_objects[j].object_2d.key_points.size(); ++k){
          pcam.vision_objects[j].object_2d.key_points[k].x = cam_result.vision_objects[j].object_2d.key_points[k].x;
          pcam.vision_objects[j].object_2d.key_points[k].y = cam_result.vision_objects[j].object_2d.key_points[k].y;
        }
        pcam.vision_objects[j].object_2d.vheight = cam_result.vision_objects[j].object_2d.vheight; //
        pcam.vision_objects[j].object_2d.occlusion = cam_result.vision_objects[j].object_2d.occlusion; //
        pcam.vision_objects[j].object_2d.truncation = cam_result.vision_objects[j].object_2d.truncation; //
        pcam.vision_objects[j].track_state.enable = cam_result.vision_objects[j].track_state.enable; //PerceptionTrackState
        if(pcam.vision_objects[j].track_state.enable){
          pcam.vision_objects[j].track_state.track_id = cam_result.vision_objects[j].track_state.track_id; 
          pcam.vision_objects[j].track_state.fusion_id = cam_result.vision_objects[j].track_state.fusion_id; 
          pcam.vision_objects[j].track_state.track_age = cam_result.vision_objects[j].track_state.track_age; 
          pcam.vision_objects[j].track_state.track_time_since_update = cam_result.vision_objects[j].track_state.track_time_since_update; 
          pcam.vision_objects[j].track_state.track_score = cam_result.vision_objects[j].track_state.track_score; 
        }
        pcam.vision_objects[j].object_3d.enable = cam_result.vision_objects[j].object_3d.enable; //PerceptionObject3D
        if(pcam.vision_objects[j].object_3d.enable){
          pcam.vision_objects[j].object_3d.x = cam_result.vision_objects[j].object_3d.x;
          pcam.vision_objects[j].object_3d.y = cam_result.vision_objects[j].object_3d.y;
          pcam.vision_objects[j].object_3d.z = cam_result.vision_objects[j].object_3d.z;
          pcam.vision_objects[j].object_3d.width = cam_result.vision_objects[j].object_3d.width;
          pcam.vision_objects[j].object_3d.height = cam_result.vision_objects[j].object_3d.height;
          pcam.vision_objects[j].object_3d.length = cam_result.vision_objects[j].object_3d.length;
          pcam.vision_objects[j].object_3d.protate = cam_result.vision_objects[j].object_3d.protate;
          pcam.vision_objects[j].object_3d.cx_score = cam_result.vision_objects[j].object_3d.cx_score;
          pcam.vision_objects[j].object_3d.cy_score = cam_result.vision_objects[j].object_3d.cy_score;
          pcam.vision_objects[j].object_3d.cz_score = cam_result.vision_objects[j].object_3d.cz_score;
          pcam.vision_objects[j].object_3d.width_score = cam_result.vision_objects[j].object_3d.width_score;
          pcam.vision_objects[j].object_3d.height_score = cam_result.vision_objects[j].object_3d.height_score;
          pcam.vision_objects[j].object_3d.length_score = cam_result.vision_objects[j].object_3d.length_score;
          pcam.vision_objects[j].object_3d.protate_score = cam_result.vision_objects[j].object_3d.protate_score;
        }
        pcam.vision_objects[j].instance_mask.enable = cam_result.vision_objects[j].instance_mask.enable; //PerceptionInstanceMask
        if(pcam.vision_objects[j].instance_mask.enable){
          pcam.vision_objects[j].instance_mask.mask_w = cam_result.vision_objects[j].instance_mask.mask_w;
          pcam.vision_objects[j].instance_mask.mask_h = cam_result.vision_objects[j].instance_mask.mask_h;
          pcam.vision_objects[j].instance_mask.instance_mask.resize(cam_result.vision_objects[j].instance_mask.instance_mask.size());
          for(int k=0; k<cam_result.vision_objects[j].instance_mask.instance_mask.size(); ++k){
            pcam.vision_objects[j].instance_mask.instance_mask[k]=cam_result.vision_objects[j].instance_mask.instance_mask[k];
          }
        }
        pcam.vision_objects[j].car_light_state.enable = cam_result.vision_objects[j].car_light_state.enable; //PerceptionCarLightState
        pcam.vision_objects[j].car_light_state.tail_light_state = cam_result.vision_objects[j].car_light_state.tail_light_state; //PerceptionCarLightState
        pcam.vision_objects[j].car_light_state.tail_light_score = cam_result.vision_objects[j].car_light_state.tail_light_score; //PerceptionCarLightState
        pcam.vision_objects[j].pedestrian_state.enable = cam_result.vision_objects[j].pedestrian_state.enable; //PerceptionPedestrianState
        pcam.vision_objects[j].pedestrian_state.pedestrian_pose = cam_result.vision_objects[j].pedestrian_state.pedestrian_pose; //PerceptionPedestrianState
        pcam.vision_objects[j].pedestrian_state.pedestrian_pose_score = cam_result.vision_objects[j].pedestrian_state.pedestrian_pose_score; //PerceptionPedestrianState
        // pcam.vision_objects[j].pedestrian_state = cam_result.vision_objects[j].traffic_sign; //cam_result.vision_objects[j].traffic_sign deleted
      }
      pcam.vision_segment.camera_width = cam_result.vision_segment.camera_width; // PerceptionVisionSegment
      pcam.vision_segment.camera_height = cam_result.vision_segment.camera_height; // PerceptionVisionSegment
      pcam.vision_segment.resa_valid = cam_result.vision_segment.resa_valid; // PerceptionVisionSegment
      if(cam_result.vision_segment.resa_valid){
        pcam.vision_segment.resa_roi = {(uint32_t)cam_result.vision_segment.resa_roi.x, (uint32_t)cam_result.vision_segment.resa_roi.y, (uint32_t)cam_result.vision_segment.resa_roi.width, (uint32_t)cam_result.vision_segment.resa_roi.height};
        CV8C1MatToImage(cam_result.vision_segment.resa, pcam.vision_segment.resa);
        // CV8C1MatToImage(cam_result.vision_segment.resacrop, pcam.vision_segment.resacrop);
      }
      pcam.vision_segment.seg_valid = cam_result.vision_segment.seg_valid; // PerceptionVisionSegment
      if(cam_result.vision_segment.seg_valid){
        pcam.vision_segment.seg_roi = {(uint32_t)cam_result.vision_segment.seg_roi.x, (uint32_t)cam_result.vision_segment.seg_roi.y, (uint32_t)cam_result.vision_segment.seg_roi.width, (uint32_t)cam_result.vision_segment.seg_roi.height};
        CV8C1MatToImage(cam_result.vision_segment.seg, pcam.vision_segment.seg);
        // cv::Mat tmp(cam_result.vision_segment.seg.rows,cam_result.vision_segment.seg.cols, CV_8UC1);
        // memcpy(tmp.data, cam_result.vision_segment.seg.data(), tmp.rows * tmp.cols * 1);
        // cv::imwrite("ggxx"+std::to_string(start_file_id_)+".jpg", tmp);
        
        // CV8C1MatToImage(cam_result.vision_segment.segcrop, pcam.vision_segment.segcrop);
      }
      //todo: pcam.lane_inst // <PerceptionSingleLaneInst>
      //todo: pcam.camera_instseg_result // PerceptionCameraInstSegResult
      //todo: pcam.wheels // <PerceptionPairObject2D>
      //todo: pcam.wheel_points // <PerceptionWheelPoints>
      //todo: pcam.vehicle_hts // <PerceptionPairObject2D>
      ret.camera_vpresults.push_back(pcam);
    }
    std::cout<<"convert result done."<<std::endl;
    return obj_ct;
  }
  void LoadOfflieImags(std::vector<SVisionImageData> &imagedatas, std::vector<std::string> &hit_cam_names, ara::adsfi::MsgImageDataList& debug_out);
};
inline std::map<std::string, std::string> createMapping(const std::string& input) {
    std::map<std::string, std::string> mapping;
    
    std::vector<std::string> pairs;
    std::stringstream ss(input);
    std::string pair;
    
    // 首先按分号';'分割字符串
    while (std::getline(ss, pair, ';')) {
        if (pair.empty()) continue;
        
        // 然后按冒号':'分割每对键值
        size_t colon_pos = pair.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = pair.substr(0, colon_pos);
            std::string value = pair.substr(colon_pos + 1);
            mapping[key] = value;
        }
    }
    std::cout<<"createMapping input:"<<input<<" output map size:"<<mapping.size()<<std::endl;
    return mapping;
}
inline int mdc_vision_process::Init(std::string& error_info,std::shared_ptr<pertarget::FaultDiagnosis> error_handle_ptr){
  init_status=0;
  // std::cout<<"GGGGGl0425"<<std::endl;
  // Read config param:

  if(!base_config_.ReadConfig(error_info)){
    std::cout << "ReadConfig error info: " << error_info << std::endl;
    return -1;
  }
  error_handle_ptr_ = error_handle_ptr;
  ascend_config_file_ = base_config_.ascend_config_file;
  ascend_vision_yaml_ = base_config_.ascend_vision_yaml;
  ascend_vision_so_ = base_config_.ascend_vision_so;
  camera_params_file_ = base_config_.camera_params_file;
  do_camname_remap_ = base_config_.do_camname_remap;

  append_cameras_ = base_config_.append_cameras;
  inTopic_camnames_ = base_config_.inTopic_camnames;

  if(append_cameras_)do_camname_remap_=true;
  if(do_camname_remap_){
    // if (inTopic_camnames_ == ""){
      cam_name_remap_=createMapping(inTopic_camnames_);
    // }
  }

  debug_vis_ = base_config_.debug_vis;
  use_offline_imgs_ = base_config_.use_offline_imgs;
  offline_imgs_path_ = base_config_.offline_imgs_path;

  // Read camera params:
  
  if(!FileExists(camera_params_file_)){
    std::cout<<"Faild to found Camera params file:"<<camera_params_file_<<std::endl;
    error_info="Faild to found Camera params file:"+camera_params_file_;
    return -1;
  }
  YAML::Node node = YAML::LoadFile(camera_params_file_);
  if (node.IsNull()) {
    std::cout << "Faild to read Camera params from file " << camera_params_file_ << " failed! please check!";
    error_info="Faild to read Camera params from file:"+camera_params_file_;
    return -1;
  }
  for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
    if(it->first.as<std::string>()=="camera_names"){
      continue;
    }
    std::string y_c_name=it->first.as<std::string>();
    camera_yaml_names_.push_back(y_c_name);
    std::cout<<"y_c_name:"<<y_c_name<<std::endl;
    const YAML::Node& cam_cf_node=node[y_c_name];
    if(cam_cf_node.IsNull()){
      std::cout<<y_c_name<<" param not exits."<<std::endl;
      error_info=y_c_name+" param not exits.";
      return -1;
    }
    if (!cam_cf_node["Size"].IsSequence()) {
      std::cout<<y_c_name<<" [Size] not exits or is not Sequence."<<std::endl;
      error_info=y_c_name+" [Size] not exits or is not Sequence.";
      return -1;
    }
    int img_w=-1;
    int img_h=-1;
    try{
      img_w = cam_cf_node["Size"][0].as<int>();
      img_h = cam_cf_node["Size"][1].as<int>();
    } catch (YAML::Exception &e) {
      std::cout << "load camera"<<y_c_name<<" Size [0] and [1] error, YAML exception: " << e.what();
      return -1;
    }
    camera_yaml_sizes_.push_back(cv::Point2i(img_w, img_h));
    size_t yuv_size = img_w * img_h * 3 / 2;//yuv编码数据长度计算方式
    fake_data_.push_back(std::vector<uint8_t>(yuv_size, 0));
  }
  std::cout<<"Total camera_yaml_names_ size:"<<camera_yaml_names_.size()<<std::endl;
  if(append_cameras_){
    if(camera_yaml_names_.empty() || camera_yaml_names_.size()!=camera_yaml_sizes_.size()){
      std::cout<<"append_cameras_ but camera_yaml_names_.size():"<<camera_yaml_names_.size()<<" camera_yaml_sizes_.size:"<<camera_yaml_sizes_.size()<<std::endl;
      return -1;
    }
  }
  //Read gsds config camera names:
  std::vector<std::string > camera_gsds_names;
  if(do_camname_remap_){
    camera_gsds_names = SplitString(inTopic_camnames_, ',');
    if (camera_gsds_names.size() != camera_yaml_names_.size()) {
      if(cam_name_remap_.size()<camera_yaml_names_.size()){
        if(append_cameras_){
          std::cout<<"Warnning: camera_gsds_names:"<<camera_gsds_names.size()<<" less to:camera_yaml_names_:"<<camera_yaml_names_.size()<<" part will be appended."<<std::endl;
        }else{
          std::cout<<"Error: camera_gsds_names:"<<camera_gsds_names.size()<<" less to:camera_yaml_names_:"<<camera_yaml_names_.size()<<" part not allowed appending."<<std::endl;
          error_info="Error: camera_gsds_names:"+std::to_string(camera_gsds_names.size())+" not equal to:camera_yaml_names_:"+std::to_string(camera_yaml_names_.size())+" part  not allowed appending.";
          return -1;
        }
      }else{
        std::cout<<"Warnning: camera_gsds_names:"<<camera_gsds_names.size()<<" more than:camera_yaml_names_:"<<camera_yaml_names_.size()<<", use front part."<<std::endl;
      }
    }
    std::cout<<"cam_name_remap_ size:"<<cam_name_remap_.size()<<std::endl;
    for (const auto& pair : cam_name_remap_) {
        std::cout << "from Key: " << pair.first << " => Value: " << pair.second << std::endl;
    }
  }
  // Init dlopen:
  std::cout<<"Try to load:"<<ascend_vision_so_<<std::endl;
  std::cout<<"ascend_config_file_:"<<ascend_config_file_<<std::endl;
  std::cout<<"ascend_vision_yaml_:"<<ascend_vision_yaml_<<std::endl;
  std::cout<<"camera_params_file_:"<<camera_params_file_<<std::endl;
  SVisionPerceptionConfig config;
  config.config_root=ascend_config_file_;
  config.config_file=ascend_vision_yaml_;
  std::cout<<"init config."<<std::endl;
  vision_per_ptr=new VisionPerception(ascend_vision_so_, camera_params_file_, config);
  // SVisionPerceptionConfig config{"/opt/usr/zxz/aarch64-linux-gnu/idp/per_libs/vision_per_config/ascend", 
  //                               "visionperception.yaml"};
  // vision_per_ptr=new VisionPerception("/opt/usr/zxz/aarch64-linux-gnu/idp/per_libs/vision_per_config/libgsds_vision_perception.so", 
  //   "/opt/usr/zxz/aarch64-linux-gnu/idp/per_libs/vision_per_config/camera_params.yaml", config);
  std::cout<<"init to init."<<std::endl;
  if(0!=vision_per_ptr->init()){
    error_info="Err when init vision_per_ptr->init";
    std::cout<<error_info<<std::endl;
    return -1;
  }
  std::cout<<"init ok."<<std::endl;
  init_status=1;
  return 0;
}//end of Init.

inline bool mdc_vision_process::IsFrequency10hz(const ara::adsfi::MsgImageDataList &image_list)
{
  bool sync_state;
  static double last_frame_time{0.0F};
  double time_stamp = image_list.header.timestamp.sec+image_list.header.timestamp.nsec*1e-9;
  if(last_frame_time==time_stamp)
  {
    // AERROR<<"stamp repeated last_frame_stamp==curent_frame_stamp curent_frame_time:"<<std::fixed<<time_stamp;
    return false;
  }
  last_frame_time = time_stamp;
  if(base_config_.frequency_10hz)
  {
    double current_time = time_stamp;
    // double time_diff = current_time - last_process_time;1755619514051.582 520
    uint64_t image_stamp_ms = current_time*1000;
    uint64_t percent_100 = image_stamp_ms%100;

    std::cout<<"current_time:"<<std::fixed<<current_time<<std::endl;
    std::cout<<"befor_time_"<<std::fixed<<image_stamp_ms<<" |"<<percent_100<<std::endl;
    bool is_valid=false;
    static double last_valid_stamp=0.0f;
    //尽可能去靠近整百毫秒数据做推理
    is_valid = ((percent_100>=0 && percent_100<=18) || (percent_100>82 && percent_100<=99));
    std::cout<<"is_valid:"<<is_valid<<std::endl;
    // 检查是否满足10Hz处理条件（约100ms间隔）且时间戳接近整百毫秒
    double curent_stamp = avos::common::NodeTime::Now().ToSecond();
    double frame_gap = curent_stamp-last_valid_stamp;
    std::cout<<"frame_gap:"<<std::fixed<<frame_gap<<" curent_stamp:"<<curent_stamp<<" last_valid_stamp:"<<last_valid_stamp<<std::endl;
    if (is_valid && frame_gap > 0.08) {  // 允许5ms的误差，确保10Hz
      sync_state = true;
      last_valid_stamp = curent_stamp;
      std::cout<<"IsFrequency10hz_valid:"<<std::fixed<<curent_stamp<<std::endl;
    } else {
      sync_state = false;
    }
  }
  else
  {//不执行整百判断
    sync_state = true;
  }
  return sync_state;
}

inline int mdc_vision_process::GetVPResoult(PerceptionVPResult &vpresults)
{
  
  NodePerceptionResult result_gqp;
  // if(!imagedatas.empty()){
  //   vision_per_ptr->process(imagedatas, result_gqp);
    if (0 != vision_per_ptr->getResult(result_gqp)){
      // error_info="model process error.";
      // auto ptr = CustomStack::Instance();
      // ptr->SetDataValid(&vpresults, false);
      return -1;
    }
  // }
  // 推理结果转换:
  int vct=ConverInferResults(result_gqp, vpresults);
  vpresults.receive_stamp=result_gqp.receive_stamp;
  double tend_time = avos::common::NodeTime::Now().ToSecond();
  vpresults.send_stamp=tend_time;
  //推理结果有可能会返回无效结果，无效结果为默认结构体参数
  if(vpresults.header.time<1000000)
  {
    // auto   ptr = CustomStack::Instance();
    // ptr->SetDataValid(&vpresults,false);
    std::cout<<"get vpresult null"<<std::endl;
    return -1;
  }
  
  vpresults.header.seq= seq_++;
  vpresults.header.module_name = "XDLOpenMDCVision_MDC610_zeroCopy";
  {//记录分割结果是否压缩
    std::string data_name="XDLOpenMDCVision_MDC610_zeroCopy_20251223";
    if(base_config_.is_segment_compressed)
    {
      data_name += "-segment_compressed";
    }
    else
    {
      data_name += "-segment_uncompressed";
    }
    vpresults.header.data_name = data_name;
  }
  return 0;
}

// int mdc_vision_process::Process(const SensorImageList& images_in , 	// 视觉图像输入列表	
//               PerceptionVPResult& vpresults,
//               SensorImageList& debug_out,
//               std::string& error_info)
inline int mdc_vision_process::Process(const ara::adsfi::MsgImageDataList &image_list, 	// 视觉图像输入列表	
                                PerceptionVPResult &vpresults,
                                ara::adsfi::MsgImageDataList& debug_out,
                                std::string& error_info)
{
  double start_time = avos::common::NodeTime::Now().ToSecond();
  double time_stamp = image_list.header.timestamp.sec+image_list.header.timestamp.nsec*1e-9;
  std::cout<<"receive_cam dely:"<<(start_time-time_stamp)*1000<<"ms," <<"cur_time:"<<start_time<<std::endl;
  // STATIC_DATA(bool, _init, false); 
  static bool _init = false;
  static std::shared_ptr<NodePerceptionResult> result = nullptr;
  auto ptr = CustomStack::Instance();
  if (!_init) {
		result = std::make_shared<NodePerceptionResult>();
    _init = true;
  }
  // 30Hz到10Hz降采样逻辑：选择整百毫秒的时间戳

  double time2 ,time1,time3;
  std::vector<SVisionImageData> imagedatas;
  std::vector<std::string> hit_cam_names;
  static std::vector<double> stash_stamp(camera_yaml_names_.size(),0.0F);
  if(IsFrequency10hz(image_list)){

  time1 = avos::common::NodeTime::Now().ToSecond();
  // vpresults.header.time=images_in.header.time;
  std::cout<<"mdc_vision_process::Process in img time:"<<time_stamp<<std::endl;

  if(!use_offline_imgs_){
    std::cout<<"Reading online data:"<<std::endl;
    for(const ara::adsfi::MsgImageData& im_img:image_list.images){
      double time11 = avos::common::NodeTime::Now().ToSecond();
      SVisionImageData imagedata;
      std::cout<<"input camera name:"<<im_img.frameID<<std::endl;
      // if(im_img.frameID=="front")
      // {
      double img_stamp = im_img.timestamp.sec+im_img.timestamp.nsec*1e-9;
        std::cout<<"inpus_cam_name:"<<im_img.frameID<<"front_time:"<<std::fixed<<img_stamp<<std::endl;
      // }
      imagedata.trigger_stamp = img_stamp;
      imagedata.height = im_img.height;
      imagedata.width = im_img.width;
      std::string camera_name="";
      int cam_id=-1;
      if(do_camname_remap_){ //重映射时，需要根据配置找到相机的新名字：
        auto it = cam_name_remap_.find(im_img.frameID);
        if (it != cam_name_remap_.end()) {
          camera_name = it->second;
          std::cout<<"camera_name remap from: "<<im_img.frameID<<" to:"<<camera_name<<std::endl;
          //增加单个相机的故障判断，时间戳重复或时间戳为0
          if(error_handle_ptr_->cam_name2index.find(camera_name)!=error_handle_ptr_->cam_name2index.end()){
            int cam_index = error_handle_ptr_->cam_name2index.at(camera_name);
            if(stash_stamp[cam_index]==imagedata.trigger_stamp || (imagedata.trigger_stamp<1.0F))
            {
              AERROR<<"cam_name:"<<camera_name<<error_handle_ptr_->index2string.at(cam_index)<<" stamp:"<<std::fixed<<imagedata.trigger_stamp;
              error_handle_ptr_->SetTopicTimeOut(error_handle_ptr_->index2string.at(cam_index),"");
            }else{
              error_handle_ptr_->ResetError(error_handle_ptr_->index2string.at(cam_index),imagedata.trigger_stamp);
              stash_stamp[cam_index] = imagedata.trigger_stamp;
            }
          }
        } else {
          continue;//跳过名字错误或者无对应的
        }
      }else{
        //不映射时，需要检查输入topic中的name是否和相机标定文件中的名字匹配:
        auto it2 = std::find(camera_yaml_names_.begin(), camera_yaml_names_.end(), im_img.frameID);
        if (it2 == camera_yaml_names_.end()) {
          std::cout << "Error: " << im_img.frameID << "在dlvision_config.yaml中camera_names中不存在." << std::endl;
          continue;
        }else{
          camera_name=im_img.frameID;
          cam_id=it2-camera_yaml_names_.begin();
        }
      }
      if(cam_id<0){
        auto it2 = std::find(camera_yaml_names_.begin(), camera_yaml_names_.end(), camera_name);
        if(it2 == camera_yaml_names_.end()){
          continue;
        }
        cam_id=it2-camera_yaml_names_.begin();
      }
      if(cam_id<0 || cam_id>=camera_yaml_sizes_.size()){
        std::cout<<"error! cam_id:"<<cam_id<<" camera_yaml_sizes_.size:"<<camera_yaml_sizes_.size()<<std::endl;
        continue;
      }
      if(imagedata.height != camera_yaml_sizes_[cam_id].y || imagedata.width != camera_yaml_sizes_[cam_id].x){
        std::cout<<"camera_name:"<<camera_name<<" size w:"<<imagedata.width<<" h:"<<imagedata.height<<" not matched to camera_yaml_sizes_[cam_id]:w:"<<camera_yaml_sizes_[cam_id].x<<" h:"<<camera_yaml_sizes_[cam_id].y<<std::endl;
        continue;
      }
      if(append_cameras_)hit_cam_names.push_back(camera_name);
      imagedata.camera_name=camera_name;
      imagedata.encoding="yuv420";//上游没有修改编码信息，暂时写死
      // zer_mat[imagedatas.size()]=convertSensorImageToMat(im_img);
  
      if(debug_vis_){
        std::cout<<"Saving "<<camera_name<<"_"<<(seq_)<<".png"<<std::endl;
        if(camera_name!="front_h30")
        cv::imwrite("gqp"+camera_name+"_"+std::to_string(seq_)+".png", convertSensorImageToMat(im_img));
      }
      // imagedata.data_ptr = reinterpret_cast<uint64_t>(zer_mat[imagedatas.size()].data);
      imagedata.data_ptr = reinterpret_cast<uint64_t>(im_img.rawData.data());
      imagedatas.push_back(imagedata);
      double time12 = avos::common::NodeTime::Now().ToSecond();
      std::cout<<"cam data convert "<<camera_name<<" time:"<<(time12-time11)*1000<<"ms"<<std::endl;
    }
  }else{
    std::cout<<"Reading offline data:"<<std::endl;
    //use_offline_imgs_
    imagedatas.clear();
    // camera_yaml_names_.resize(1);
    
    LoadOfflieImags(imagedatas, hit_cam_names, debug_out);
  }

  time2 = avos::common::NodeTime::Now().ToSecond();
  if(append_cameras_ && hit_cam_names.size()<camera_yaml_names_.size()){
    for(int k=0;k<camera_yaml_names_.size(); ++k){
      auto& lack_name=camera_yaml_names_[k];
      auto it2 = std::find(hit_cam_names.begin(), hit_cam_names.end(), lack_name);
      if (it2 == hit_cam_names.end()) {
        std::cout << "append camera: " << lack_name << " width:"<<camera_yaml_sizes_[k].x<<" height:"<<camera_yaml_sizes_[k].y<< std::endl;
        SVisionImageData imagedata;
        imagedata.height = camera_yaml_sizes_[k].y;
        imagedata.width = camera_yaml_sizes_[k].x;
        
        imagedata.camera_name=lack_name;
        imagedata.encoding="yuv420";
        imagedata.trigger_stamp = avos::common::NodeTime::Now().ToSecond();

        // zer_mat[imagedatas.size()]=cv::Mat(camera_yaml_sizes_[k].y, camera_yaml_sizes_[k].x, CV_8UC3, cv::Scalar(0, 0, 0));
        // imagedata.data_ptr = reinterpret_cast<uint64_t>(zer_mat[imagedatas.size()].data);
        imagedata.data_ptr = reinterpret_cast<uint64_t>(fake_data_[k].data()/*zer_mat[imagedatas.size()].data*/);
        imagedatas.push_back(imagedata);
      }
    }
  }
  double time22 = avos::common::NodeTime::Now().ToSecond();
  std::cout<<"cam data append "<<imagedatas.size()-hit_cam_names.size()<<" time:"<<(time22-time2)*1000<<"ms"<<std::endl;
  std::cout<<"append_cameras_:"<<append_cameras_<<" hit_cam_names:"<<hit_cam_names.size()<<" imagedatas.size:"<<imagedatas.size()<<std::endl; 
  // 推理：
  time3 = avos::common::NodeTime::Now().ToSecond();
  if(!imagedatas.empty()){
    vision_per_ptr->process(imagedatas);
  }
  imagedatas.clear();
  hit_cam_names.clear();
}

  int vp_sesult_status = GetVPResoult(vpresults);
  // std::cout<<"\n gqp mdc vision end: time cost:"<<(tend_time-start_time)*1000<<"ms," <<std::endl;
  // std::cout<<"mdc vision end: objects:"<<vct <<std::endl;
  std::cout<<"mdc_vision_end: vision time dely:"<<(vpresults.send_stamp-vpresults.header.time)*1000<<"ms," <<std::endl;
  // std::cout<<"time1-start_time:"<<(time1-start_time)*1000<<"ms"<<std::endl;
  std::cout<<"time2-time1:"<<(time2-time1)*1000<<"ms"<<std::endl;
  std::cout<<"time3-time2:"<<(time3-time2)*1000<<"ms"<<std::endl;
  std::cout<<"end2."<<std::endl;
  std::cout<<"end3."<<std::endl;
  std::cout<<"vp_sesult_status:"<<vp_sesult_status<< "stamp:"<<std::fixed<<vpresults.header.time<<std::endl;
  return vp_sesult_status;
}//end of function Process.
static inline std::string gToString(const int& val, int bitL){
  int value=val;
  int max_v=pow(10, bitL)-1;
  if(value>max_v){
    value = value%(max_v+1);
  }else if(value<0){
    value = -value;
  }
  std::string tmp;
  while(--bitL>=0 && value<pow(10, bitL)){
    tmp = tmp +"0";
  }
  if(value==0){
    return tmp;
  }else{
    return tmp+std::to_string(value);
  }
}
inline cv::Mat BGR2YUV420SP(const cv::Mat &image) {
    cv::Mat yuvImage(image.rows * 3 / 2, image.cols, CV_8UC1);
    cv::Mat yuv420p;
    cv::cvtColor(image, yuv420p, cv::COLOR_BGR2YUV_I420);
    uint8_t *y_data = yuv420p.data;                         // Y 数据
    uint8_t *u_data = y_data + image.rows * image.cols;     // U 数据
    uint8_t *v_data = u_data + image.rows * image.cols / 4; // V 数据
    memcpy(yuvImage.data, y_data, image.rows * image.cols);
    uint8_t *u_data_dst = yuvImage.data + image.rows * image.cols;
    for (int i = 0; i < image.rows * image.cols / 4; i++) {
        *u_data_dst = *(u_data++);
        u_data_dst += 2;
    }
    uint8_t *v_data_dst = yuvImage.data + image.rows * image.cols + 1;
    for (int i = 0; i < image.rows * image.cols / 4; i++) {
        *v_data_dst = *(v_data++);
        v_data_dst += 2;
    }
    return yuvImage;
}


inline cv::Mat BGR2YUV420SP_Compatible(const cv::Mat& bgrImage) {
    if (bgrImage.empty()) {
        return cv::Mat();
    }

    int width = bgrImage.cols;
    int height = bgrImage.rows;
    int y_size = width * height;

    // 1. 首先使用OpenCV将BGR转换为YUV_I420
    cv::Mat i420Image;
    cv::cvtColor(bgrImage, i420Image, cv::COLOR_BGR2YUV_I420); // 转换后图像高度为 height * 1.5

    // 2. 创建一个目标Mat用于存储NV12格式数据
    cv::Mat nv12Image(height * 3 / 2, width, CV_8UC1);

    // 3. 复制Y平面 (前 height 行)
    memcpy(nv12Image.data, i420Image.data, y_size);

    // 4. 获取I420中的U平面和V平面的起始指针
    const uint8_t* u_data_i420 = i420Image.data + y_size; // U平面开始位置
    const uint8_t* v_data_i420 = u_data_i420 + (y_size / 4); // V平面开始位置 (在U平面之后)

    // 5. 将I420的U、V平面（连续存储）转换为NV12的UV交错平面
    uint8_t* uv_plane_nv12 = nv12Image.data + y_size; // NV12的UV平面开始位置
    for (int i = 0; i < y_size / 4; ++i) {
        *uv_plane_nv12++ = *u_data_i420++; // 放入U
        *uv_plane_nv12++ = *v_data_i420++; // 紧接着放入V
    }

    return nv12Image;
}

static void BGR2YUV420(const cv::Mat& bgrImg, std::vector<uint8_t>& yuvData) {
    CV_Assert(bgrImg.type() == CV_8UC3); // 确保输入是 BGR 8UC3
    std::cout<<"BGR2YUV420 img cols:"<<bgrImg.cols<<" rows:"<<bgrImg.rows<<std::endl;
    const int width = bgrImg.cols;
    const int height = bgrImg.rows;
    const int ySize = width * height;
    const int uvSize = ySize / 4;

    // 分配内存：Y + U + V
    yuvData.resize(ySize + 2 * uvSize);
    uint8_t* yPlane = yuvData.data();
    uint8_t* uPlane = yuvData.data() + ySize;
    uint8_t* vPlane = yuvData.data() + ySize + uvSize;

    // 临时存储全分辨率的 U/V
    std::vector<uint8_t> uFull(ySize);
    std::vector<uint8_t> vFull(ySize);

    // BGR → YUV444 转换
    for (int y = 0; y < height; ++y) {
        const cv::Vec3b* ptr = bgrImg.ptr<cv::Vec3b>(y);
        for (int x = 0; x < width; ++x) {
            uint8_t b = ptr[x][0];
            uint8_t g = ptr[x][1];
            uint8_t r = ptr[x][2];

            // 计算 YUV (ITU-R BT.601 标准)
            int yy = 0.299 * r + 0.587 * g + 0.114 * b;
            int uu = -0.169 * r - 0.331 * g + 0.5 * b + 128;
            int vv = 0.5 * r - 0.419 * g - 0.081 * b + 128;

            yPlane[y * width + x] = cv::saturate_cast<uint8_t>(yy);
            uFull[y * width + x] = cv::saturate_cast<uint8_t>(uu);
            vFull[y * width + x] = cv::saturate_cast<uint8_t>(vv);
        }
    }

    // U/V 下采样 (4:2:0)
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            int idx = (y / 2) * (width / 2) + (x / 2);
            uPlane[idx] = uFull[y * width + x];
            vPlane[idx] = vFull[y * width + x];
        }
    }
}
inline std::string mat_type2encoding(int mat_type) {
      switch (mat_type) {
          case CV_8UC1:
              return "mono8";
          case CV_8UC3:
              return "bgr8";
          case CV_16SC1:
              return "mono16";
          case CV_8UC4:
              return "rgba8";
          default:
              return "";
      }
  }
  inline int CVMatToSensorImage(const cv::Mat& source, SensorImage& image) {
      image.height = source.rows;
      image.width = source.cols;
      std::string encoding;
      encoding = mat_type2encoding(source.type());
      image.encoding = encoding;
      int data_size;
      if (source.type() == CV_8UC1) {
          data_size = 1;
      } else if (source.type() == CV_8UC3) {
          data_size = 3;
      } else if (source.type() == CV_16SC1) {
          data_size = 2;
      } else if (source.type() == CV_8UC4) {
          data_size = 4;
      } else {
          AINFO << "[CVMatToSensorImage] error source.type() " << source.type();
          return -1;
      }

      image.step = data_size * source.cols; // Full row length in bytes

      image.data.resize(source.rows * source.cols * data_size);
      memcpy(image.data.data(), source.data, source.rows * source.cols * data_size);
      return 0;
  }
inline void mdc_vision_process::LoadOfflieImags(std::vector<SVisionImageData> &imagedatas, std::vector<std::string> &hit_cam_names, ara::adsfi::MsgImageDataList& debug_out){
  // load jpg image to cv::Mat
  // convert cv::Mat to yuv420
  // fill yuv420 data to fake_data_[0]
  
  std::string img_file_name=offline_imgs_path_+"/cam0_"+gToString(start_file_id_, 4)+".jpg";
  start_file_id_++;
  std::cout<<"Loading offline image:"<<img_file_name<<std::endl;
  std::cout<<"read image:"<<img_file_name<<std::endl;
  if(!FileExists(img_file_name)){
    start_file_id_=1;
    img_file_name=offline_imgs_path_+"/cam0_"+gToString(start_file_id_, 4)+".jpg";
  }
  cv::Mat ori_img=cv::imread(img_file_name);
  // cv::imwrite("ori"+std::to_string(start_file_id_)+".jpg", ori_img);
  // BGR2YUV420(ori_img, fake_data_[0]);
  if(ori_img.cols>0){
    debug_out.images.clear();
    SensorImage img;
    CVMatToSensorImage(ori_img, img);
    // debug_out.images.push_back(img);
  }
  // static cv::Mat yuv_image_;
  yuv_image_ = BGR2YUV420SP(ori_img);
  int idx=-1;
  for(int cam_idx=0;cam_idx<camera_yaml_names_.size();++cam_idx)
  {
    std::cout<<"find_name:"<<camera_yaml_names_[cam_idx]<<std::endl;
    if(camera_yaml_names_[cam_idx]=="front_h100")
    {
      std::cout<<"find_name_idx:"<<cam_idx<<std::endl;
      idx = cam_idx;
      break;
    }
  }
  if(idx<0)
  {
    std::cout<<"find cam fail!!\n";
    return;
  }
  std::string lack_name=camera_yaml_names_[idx];
  // std::cout<<"lack_name**:"<<lack_name<<std::endl;
  // camera_yaml_sizes_.push_back(cv::Point2i(img_w, img_h));
  // size_t yuv_size = img_w * img_h * 3 / 2;//yuv编码数据长度计算方式
  // fake_data_.push_back(std::vector<uint8_t>(yuv_size, 0));

  SVisionImageData imagedata;
  imagedata.height = camera_yaml_sizes_[idx].y;
  imagedata.width = camera_yaml_sizes_[idx].x;
  
  imagedata.camera_name=lack_name;
  imagedata.encoding="yuv420";
  hit_cam_names.push_back(lack_name);
  // imagedata.data_ptr = reinterpret_cast<uint64_t>(fake_data_[0].data()/*zer_mat[imagedatas.size()].data*/);
  imagedata.data_ptr = reinterpret_cast<uint64_t>(yuv_image_.data/*zer_mat[imagedatas.size()].data*/);
  imagedata.trigger_stamp = avos::common::NodeTime::Now().ToSecond();
  imagedatas.push_back(imagedata);
  std::cout<<"Loaded offline image:"<<lack_name<<std::endl;
}

}
}
