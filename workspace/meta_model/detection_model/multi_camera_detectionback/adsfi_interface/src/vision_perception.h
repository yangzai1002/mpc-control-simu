#ifndef _VISION_PERCEPTION_H_
#define _VISION_PERCEPTION_H_

#include "dvpp_interface/cidpabstractplatforminterface.h"
#include "adsfi_interface/src/protocol/vision_detection2usr_protocol.h"
#include "adsfi_interface/src/protocol/visionperception_struct.h"
using namespace avos::perception;
using namespace avos::idpware;


class VisionPerception {
public:
    /*
     * so: type(std::string), so动态库路径
     * camera_pamera_file: type(std::string), 相机参数路径
     * config：type(SVisionPerceptionConfig), 模型配置参数
     */
    VisionPerception(const std::string &so,
                     const std::string &camera_pamera_file,
                     const SVisionPerceptionConfig &config)
        : so_(so), camera_pamera_file_(camera_pamera_file), config_(config) {}

    ~VisionPerception() = default;

    int init() {
        // 加载so文件
        std::cout<<"loadd so_:"<<so_<<std::endl;
        std::cout<<"c250amera_pamera_file_:"<<camera_pamera_file_<<std::endl;
        handle_.open(so_, RTLD_LAZY|RTLD_DEEPBIND);
        // 初始化相机参数
        std::cout<<"1so_:"<<so_<<std::endl;
        SStringConfig cam_pam_path_conf;
        cam_pam_path_conf.str=camera_pamera_file_;
        DataTrans<SStringConfig> path_conf_transor;
        PerDataInterface path_to_finale= path_conf_transor.Serialize(cam_pam_path_conf,m_process_path_to_);
        auto ret = handle_.ioctl(
            EIdpVisionDectectionIOType::IOCTL_INIT_INTRINSICS_PATH,
            &path_to_finale);
        if (0 != ret) {
            // 释放so
            std::cout<<"\n2so_:"<<so_<<std::endl;
            std::cout<<"c22amera_pamera_file_:"<<camera_pamera_file_<<std::endl;
            return -1;
        }
        
        DataTrans<SVisionPerceptionConfig> init_conf_transor;
        PerDataInterface init_to_finale= init_conf_transor.Serialize(config_,m_process_config_to_);
        
        // 初始化模型配置
        ret = handle_.ioctl(EIdpVisionDectectionIOType::IOCTL_INIT_CONFIG,
                            &init_to_finale);
        if (0 != ret) {
            // 释放so
            return -1;
        }

        return 0;
    }
    int getResult(NodePerceptionResult &result)
    {
        PerDataInterface data_from;
        DataTrans<NodePerceptionResult> from_data_transor;
        int ret = handle_.ioctl(
            EIdpVisionDectectionIOType::IOCTL_GET_VISION_DETECT_DATA,
            &data_from);
        // if(data_from.m_ptr_data_len<=0) 
        // {
        //     std::cout<<"m_ptr_data_len_error\n"; 
        //     return -1;
        // }
        if(data_from.m_ptr==nullptr) 
        {
            std::cout<<"m_ptr_null\n"; 
            return -1;
        }
        result = from_data_transor.Deserialize(data_from);
        std::cout<<"\ngqp vec result:"<<result.camera_vision_objects.size()<<std::endl;
        return 0;
    }
    int process(const std::vector<SVisionImageData> &imagedatas) {
        //在某些板卡上如310P,初始化推理的前几帧耗时较高，导致推理结果队列里的结果没有及时pop出来，造成队列里有堆积，输出的结果会有延迟
        //临时处理办法，timmer触发，设置一个高频处理流程，对最终的推理结果判断是否有效，只publish有效数据。
        //
        if (fabs(imagedatas[0].trigger_stamp - last_trigger_time) > 0.02&&imagedatas[0].trigger_stamp>1000000) {
            SVisionImageDataList data_struct_to;
            data_struct_to.camera_images = imagedatas;
            for(auto &img:imagedatas)
                std::cout<<"in_camera_name:"<<img.camera_name<<"height:"<<img.height<<"width:"<<img.width<<"encoding:"<<img.encoding<<"trigger_stamp:"<<std::fixed<<img.trigger_stamp<<std::endl;
            DataTrans<SVisionImageDataList> to_data_transor;
            std::string                     m_process_str_to;
            PerDataInterface data_to_finale = to_data_transor.Serialize(data_struct_to, m_process_str_to);
            auto ret = handle_.ioctl(EIdpVisionDectectionIOType::IOCTL_SET_IMAGE_LIST_DATA, &data_to_finale);
            // std::cout<<"send_img:"<<ret<<"\n";
            //   reinterpret_cast<void *>(&imagedatas));
            // if (0 != ret)
            //     return -1;
            last_trigger_time = imagedatas[0].trigger_stamp;
        }
        std::cout<<"vision process time2222 "<<std::fixed<<imagedatas[0].trigger_stamp<<std::endl;
        // PerDataInterface data_from;
        // DataTrans<NodePerceptionResult> from_data_transor;
        // int ret = handle_.ioctl(
        //     EIdpVisionDectectionIOType::IOCTL_GET_VISION_DETECT_DATA,
        //     &data_from);
        // if(data_from.m_ptr_data_len<=0) 
        // {
        //     std::cout<<"m_ptr_data_len_error\n"; 
        //     return -1;
        // }
        // if(data_from.m_ptr==nullptr) 
        // {
        //     std::cout<<"m_ptr_null\n"; 
        //     return -1;
        // }
        // result = from_data_transor.Deserialize(data_from);
        // std::cout<<"\ngqp vec result:"<<result.camera_vision_objects.size()<<std::endl;

        return 0;
    }

private:
    std::string so_;
    std::string camera_pamera_file_;
    SVisionPerceptionConfig config_;
    CIdpAbstractPlatformInterface handle_;
    std::string m_process_config_to_;
    std::string m_process_path_to_;
    double last_trigger_time=0;
    

    // SafeQueue<SVisionImageDataList> input_queue;       // 原始帧队列
    // SafeQueue<PipelineData> mid_queue;    // 预处理后数据队列
    // SafeQueue<NodePerceptionResult> output_queue_; // 结果队列
};

#endif
