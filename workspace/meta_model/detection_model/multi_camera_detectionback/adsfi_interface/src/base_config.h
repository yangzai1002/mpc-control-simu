#pragma once 

#include <vector>
#include <iostream>
#include <string>
#include "log/log.h"
#include "custom_stack.h"//获取配置参数

namespace avos{
namespace perception{

//判断文件是否存在:  
inline bool FileExists(const std::string file_path) {
std::ifstream ifs(file_path);
if (!ifs.is_open()) {
    std::cout << "PerCameraService: opening failed. check if " << file_path << " exists";
    std::cout<<"File not exists:"<<file_path<<std::endl;
    return false;
} else {
    std::cout << "PerCameraService read file: parsing yaml " << file_path << std::flush << std::endl;
}
ifs.close();
return true;
}


struct BaseConfig{
  std::string ascend_config_file; // dlopen库配置参数文件夹, ../config/ascend
  std::string ascend_vision_yaml; // dlopen库函数配置文件名
  std::string ascend_vision_so;   // dlopen库so路径和名字
  std::string camera_params_file; // 相机标定参数文件
  std::string inTopic_camnames;     // 相机名称重映射输入列表，映射为相机标定参数文件中的内容，注意：顺序必须一致，英文分割
  bool frequency_10hz{false}; //相机数据30hz，视觉模型10hz输出，时间戳整百
  bool append_cameras{true};            // 如果输入相机数量不足，自动补全
  bool debug_vis{false};                 // 是否可视化
  bool use_offline_imgs{false};          // 使用离线图片数据进行测试
  std::string offline_imgs_path;  // 离线数据路径：cam0_0001.jpg...
  bool is_segment_compressed{false}; //分割结果是否压缩，注意，压缩后读取结果需要解压缩

  bool do_camname_remap{true};

  bool ReadFromVehicle_param(){
    std::string error_info;//不上传，失败了cout即可，通过gsds继续初始化
    auto param_ptr = CustomStack::Instance();

    std::string project_config_path = param_ptr->GetNodeConfigPath();
    std::cout<<"project_config_path:"<<project_config_path<<std::endl;
    // 从 perception.proj_cfg中读取参数：
    std::string board_name;
    if(param_ptr->GetProjectConfigValue<std::string>("perception/XDLOpenMDCVision_MDC610/board_name", board_name)){ //从 perception.proj_cfg 中读取 XDLOpenMDCVision_MDC610 ascend_config_file
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 board_name:"<<board_name<<std::endl;
      ascend_config_file = project_config_path + "/config/config/" + board_name;
      std::cout<<"ascend_config_file:"<<ascend_config_file<<std::endl;
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/board_name error.";
      std::cout<< error_info<<std::endl;
      return false;
    }

    std::string yaml_file_name;
    if(param_ptr->GetProjectConfigValue<std::string>("perception/XDLOpenMDCVision_MDC610/yaml_file_name", ascend_vision_yaml)){ //从 perception.proj_cfg 中读取 XDLOpenMDCVision_MDC610 ascend_config_file
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 yaml_file_name:"<<ascend_vision_yaml<<std::endl;
      if(!FileExists(ascend_config_file + "/" + ascend_vision_yaml)){
        std::cout<<"ascend_vision_yaml not exists:"<<ascend_config_file + "/" + ascend_vision_yaml<<std::endl;
        return false;//保证可以通过gsds初始化
      }
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/yaml_file_name error.";
      std::cout<< error_info<<std::endl;
      return false;
    }

    std::string so_file_name;
    if(param_ptr->GetProjectConfigValue<std::string>("perception/XDLOpenMDCVision_MDC610/so_file_name", so_file_name)){ //从 perception.proj_cfg 中读取 XDLOpenMDCVision_MDC610 ascend_config_file
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 so_file_name:"<<so_file_name<<std::endl;
      ascend_vision_so = ascend_config_file + "/" + so_file_name;
      if(!FileExists(ascend_vision_so)){
        std::cout<<"ascend_vision_so not exists:"<<ascend_vision_so<<std::endl;
        return false;//保证可以通过gsds初始化
      }
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/so_file_name error.";
      std::cout<< error_info<<std::endl;
      return false;
    }

    std::string camera_params_file_name;
    if(param_ptr->GetProjectConfigValue<std::string>("perception/XDLOpenMDCVision_MDC610/camera_params_file_name", camera_params_file_name)){ //从 perception.proj_cfg 中读取 XDLOpenMDCVision_MDC610 ascend_config_file
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 camera_params_file_name:"<<camera_params_file_name<<std::endl;
      camera_params_file = ascend_config_file + "/" + camera_params_file_name;
      std::cout<<"camera_params_file:"<<camera_params_file<<std::endl;
      if(!FileExists(camera_params_file)){
        std::cout<<"camera_params_file not exists:"<<camera_params_file<<std::endl;
        return false;//保证可以通过gsds初始化
      }
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/camera_params_file_name error.";
      std::cout<< error_info<<std::endl;
      return false;
    }

    if(param_ptr->GetProjectConfigValue<std::string>("perception/XDLOpenMDCVision_MDC610/topic_camnames", inTopic_camnames)){ //从 perception.proj_cfg 中读取 XDLOpenMDCVision_MDC610 ascend_config_file
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 topic_camnames:"<<inTopic_camnames<<std::endl;
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/topic_camnames error.";
      std::cout<< error_info<<std::endl;
      return false;
    }

    if(param_ptr->GetProjectConfigValue<bool>("perception/XDLOpenMDCVision_MDC610/append_cameras", append_cameras)){ //从 perception.proj_cfg 中读取 XDLOpenMDCVision_MDC610 ascend_config_file
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 append_cameras:"<<append_cameras<<std::endl;
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/append_cameras error.";
      std::cout<< error_info<<std::endl;
      return false;
    }
    if(param_ptr->GetProjectConfigValue<bool>("perception/XDLOpenMDCVision_MDC610/frequency_10hz", frequency_10hz)){ //从 perception.proj_cfg 中读取 XDLOpenMDCVision_MDC610 ascend_config_file
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 frequency_10hz:"<<frequency_10hz<<std::endl;
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/frequency_10hz error.";
      std::cout<< error_info<<std::endl;
      return false;
    }
    if(param_ptr->GetProjectConfigValue<bool>("perception/XDLOpenMDCVision_MDC610/debug_vis", debug_vis))
    {
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 debug_vis:"<<debug_vis<<std::endl;
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/debug_vis error.";
      std::cout<< error_info<<std::endl;
      return false;
    }
    if(param_ptr->GetProjectConfigValue<bool>("perception/XDLOpenMDCVision_MDC610/use_offline_imgs", use_offline_imgs))
    {
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 use_offline_imgs:"<<use_offline_imgs<<std::endl;
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/use_offline_imgs error.";
      std::cout<< error_info<<std::endl;
      return false;
    }
    if(param_ptr->GetProjectConfigValue<std::string>("perception/XDLOpenMDCVision_MDC610/offline_imgs_path", offline_imgs_path))
    {
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 offline_imgs_path:"<<offline_imgs_path<<std::endl;
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/offline_imgs_path error.";
      std::cout<< error_info<<std::endl;
      return false;
    }
    if(param_ptr->GetProjectConfigValue<bool>("perception/XDLOpenMDCVision_MDC610/is_segment_compressed", is_segment_compressed))
    {
      std::cout<<"MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610 is_segment_compressed:"<<is_segment_compressed<<std::endl;
    }else{ //否则读取失败，报错，并让后续从gsds中读取配置:
      error_info="MDCVision Read perception.proj_cfg XDLOpenMDCVision_MDC610/is_segment_compressed error.";
      std::cout<< error_info<<std::endl;
      return false;
    }

    
    return true; //读取全部成功
  }

  bool ReadConfig(std::string &error_info){
    std::cout<<"start ReadConfig:"<<std::endl;
    error_info="";//clear error info.
    auto custom_stack = CustomStack::Instance();
    // std::string project_config_path = custom_stack->GetNodeConfigPath();
    // std::cout<<"project_config_path:"<<project_config_path<<std::endl;
    // 优先尝试从配置文件中读取多个雷达的参数，如果读取失败，则使用默认gsds配置参数：
    if(false==ReadFromVehicle_param()){
      // std::cout<<"Read param from gsds global:"<<std::endl;
      // std::string board_name;
      // if(0!=custom_stack->GetConfig("board_name", board_name)){
      //   error_info="Init faild to get board_name param";
      //   return -1;
      // }
      // std::cout<<"board_name:"<<board_name<<std::endl;
      // ascend_config_file = project_config_path+"/"+board_name;

      // if(0!=custom_stack->GetConfig("yaml_file_name", ascend_vision_yaml)){
      //   error_info="Init faild to get yaml_file_name param";
      //   std::cout<<error_info<<std::endl;
      //    return false;
      // }
      // std::cout<<"yaml_file_name:"<<ascend_vision_yaml<<std::endl;
      // if(!FileExists(ascend_config_file + "/" + ascend_vision_yaml))return false;

      // std::string so_file_name;
      // if(0!=custom_stack->GetConfig("so_file_name", so_file_name)){
      //   error_info="Init faild to get so_file_name param";
      //   std::cout<<error_info<<std::endl;
      //    return false;
      // }

      // ascend_vision_so = ascend_config_file + "/" + so_file_name;
      // if(!FileExists(ascend_vision_so))return false;
      
      // std::string camera_params_file_name;
      // if(0!=custom_stack->GetConfig("camera_params_file_name", camera_params_file_name)){
      //   error_info="Init faild to get camera_params_file_name param";
      //   std::cout<<error_info<<std::endl;
      //   return false;
      // }
      // camera_params_file =  ascend_config_file + "/"+ camera_params_file_name;
      // if(!FileExists(camera_params_file))return false;
      
      // if(0!=custom_stack->GetConfig("append_cameras", append_cameras)){
      //   error_info="Init faild to get append_cameras param";
      //   std::cout<<error_info<<std::endl;
      //   return false;
      // }


      // if(append_cameras && 0!=custom_stack->GetConfig("topic_camnames", inTopic_camnames)){
      //   error_info="Init faild to get topic_camnames param";
      //   std::cout<<error_info<<std::endl;
      //   return false;
      // }

      // if(append_cameras && 0!=custom_stack->GetConfig("frequency_10hz", frequency_10hz)){
      //   error_info="Init faild to get frequency_10hz param";
      //   std::cout<<error_info<<std::endl;
      //   return false;
      // }
      // std::cout<<"from gsds param frequency_10hz:"<<frequency_10hz<<std::endl;

      // if(0!=custom_stack->GetConfig("debug_vis", debug_vis)){
      //     error_info="Init faild to get debug_vis param";
      //     std::cout<<error_info<<std::endl;
      //     return false;
      //   }
      // if(0!=custom_stack->GetConfig("use_offline_imgs", use_offline_imgs)){
      //     std::cout<<"Init faild to get use_offline_imgs param, using as false."<<std::endl;
      //   }
      // if(use_offline_imgs && 0!=custom_stack->GetConfig("offline_imgs_path", offline_imgs_path)){
      //     error_info="Init faild to get offline_imgs_path param";
      //     std::cout<<error_info<<std::endl;
      //     return false;
      //   }
      // if(use_offline_imgs && 0!=custom_stack->GetConfig("is_segment_compressed", is_segment_compressed)){
      //     error_info="Init faild to get is_segment_compressed param";
      //     std::cout<<error_info<<std::endl;
      //     return false;
      //   }
      AERROR<<"Init error";
      return false;
    }

  

    std::cout<<"param read okk"<<std::endl;
    return true;
  }

  void Print(){
    std::cout<<"MDCVision BaseConfig:"<<std::endl;
    std::cout<<"ascend_config_file:"<<ascend_config_file<<std::endl;
    std::cout<<"ascend_vision_yaml:"<<ascend_config_file+"/"+ascend_vision_yaml<<std::endl;
    std::cout<<"ascend_vision_so:"<<ascend_vision_so<<std::endl;
    std::cout<<"camera_params_file:"<<camera_params_file<<std::endl;
    std::cout<<"inTopic_camnames:"<<inTopic_camnames<<std::endl;
    std::cout<<"append_cameras:"<<append_cameras<<std::endl;
    std::cout<<"do_camname_remap:"<<do_camname_remap<<std::endl;
    std::cout<<"debug_vis:"<<debug_vis<<std::endl;
    std::cout<<"use_offline_imgs:"<<use_offline_imgs<<std::endl;
    std::cout<<"offline_imgs_path:"<<offline_imgs_path<<std::endl;
  }
};

}
}