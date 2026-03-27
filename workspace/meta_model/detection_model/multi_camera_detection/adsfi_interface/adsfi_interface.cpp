#include "adsfi_interface.h"
#include "log/avos_node.h"
#include "ara/adsfi/impl_type_msgcameradetectionresult.h"
#include "ara/adsfi/impl_type_msghaf2ddetectionout.h"
#include "adsfi_manager/base_adsfi_interface.h"
void AdsfiInterface::Init()  {
    SetScheduleInfo("message");
    std::string init_err;
    // error_handle_ptr = std::make_shared<pertarget::FaultDiagnosis>();
    // if(handle.Init(init_err,error_handle_ptr)<0)
    // {
    //     AERROR<<"multi_camera_detection_Init:"<<init_err;
    // }

    det_seq = 0;
    yolo_ = std::make_unique<YoloDetection>();
    yolo_->Init();
    std::cout << " AdsfiInterfaceinit" << std::endl;

    thread_put_image_in_shm_ = std::thread([this]() {
        // pthread_setname_np(pthread_self(), "put_image_in_shm");
        // compatible::VideoParameter video_parameter;
        std::cout<<"[############## INFO] start thread \n";
        static bool init = false;
        yolo_->InitDetModelResource();

        int count = 0;
        while (!_stopped.load()) {
            ara::adsfi::MsgImageDataList image_list;
            if (!que_image_list_for_put_image_in_shm_.timeout_pop_latest(image_list, 10000)) {
                continue;
            }

            // for (const auto& img : image_list.images) {
            //     AINFO << "XImageLayout input image name: " << img.frameID << ", time:" << std::setprecision(20)<<"\n";
            // }

            std::cout<<"[############## INFO] before sync \n";


            dvpp::sync(image_list);
            std::cout<<"[############## INFO] after sync \n";

            // ara::adsfi::MsgImageData image;
            // safe_release(image_list);
            // std::shared_ptr<ara::adsfi::MsgImageDataList> ptr;

            // GetImageList(ptr);
            for(const ara::adsfi::MsgImageData& im_img:image_list.images){
                std::cout<<im_img.frameID<<" im_img.height "<<im_img.height<<" im_img.width "<<im_img.width<<" "<<im_img.rawData.size()<<" \n";
            }   
            timeval tm;
            gettimeofday(&tm, NULL);
            std::cout<<"########################## detection time "<<tm.tv_sec * 1e3 + tm.tv_usec * 1e-3<<"\n";
            if(image_list.images[0].rawData.size() == 3110400){
                yolo_->Detection(image_list.images[0]);
                std::cout<<"end detection \n";
            }
            // if(yolot == nullptr)
            //   std::cout<<"end detection \n";

            // ReleaseData(*image_list);
            safe_release(image_list);

            std::cout<<"end while \n";

        }
    });

}

AdsfiInterface::~AdsfiInterface() {
    _stopped = true;

    if (thread_put_image_in_shm_.joinable()) {
        thread_put_image_in_shm_.join();
    }
}

void AdsfiInterface::DeInit(){
    yolo_->Deinit();
    std::cout << "deinit" << std::endl;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageDataList> &image_list_ptr) {
    if (__glibc_unlikely(nullptr == image_list_ptr)) {
        // AERROR << "image_list is nullptr";
        return;
    }  
  
    double time_stamp = image_list_ptr->header.timestamp.sec+image_list_ptr->header.timestamp.nsec/1e9;
    que_image_list_for_put_image_in_shm_.push(*image_list_ptr);
    AINFO << "queue size of que_image_list_for_put_image_in_shm_: " << que_image_list_for_put_image_in_shm_.size();
    std::cout << "  IdpSensorImageList:" << image_list_ptr->header.seq <<"time_stamp:"<<std::fixed<<time_stamp<< std::endl;

    std::shared_ptr<ara::adsfi::MsgImageDataList> ptr;

    // ptr = image_list_ptr;
    // dvpp::sync(*ptr);
    // std::cout<<"[############## INFO] after sync \n";

    // if(ptr->images[0].rawData.size() == 3110400){
    //     yolo_->Detection(ptr->images[0]);
    //     std::cout<<"end detection \n";
    // }

    // safe_release(*ptr);

    // static double last_frame_stamp=0.0;
    // const std::string error_type = "3032008_SensorImageListTimeOut_error";
    // // double tar_stamp=image_list_ptr->header
    // if(last_frame_stamp == time_stamp || time_stamp<1.0F)
    // {
    //   //1.时间戳重复故障
    //   double error_stamp = time_stamp;
    //   if(time_stamp<1.0)
    //     error_stamp = avos::common::NodeTime::Now().ToSecond();
    //   bool is_trigger_error=true;
    //   std::string error_info = "plese check camera data!!";
    //   // error_handle_ptr->SetTopicTimeOut(error_type,error_info);
    // }else{
    //   // error_handle_ptr->ResetError(error_type,time_stamp);
    //   // image_deque.push(image_list_ptr);
    //   SetImageList(image_list_ptr);
    //   // camera_image_lists_.push(image_list_ptr);
    // }
    // SetImageList(image_list_ptr);
    // last_frame_stamp = time_stamp;
}

void AdsfiInterface::ConvertToMsgCameraVPResult(const PerceptionVPResult &out_results,ara::adsfi::MsgCameraVPResult &vpresults)
{

double time_sec = out_results.header.time;
// long sec = static_cast<long>(time_sec);  // 秒部分（整数部分）
// long nsec = static_cast<long>((time_sec - sec) * 1e9);  // 纳秒部分（小数部分乘以1e9）
vpresults.header.timestamp.sec = ADSFI_GET_SEC(time_sec);
vpresults.header.timestamp.nsec = ADSFI_GET_NSEC(time_sec);
vpresults.header.frameID = out_results.header.data_name;
vpresults.header.time_chain_vec.resize(1);
vpresults.header.seq = ++seq_;
vpresults.header.time_chain_vec[0].t1 = out_results.receive_stamp;
// vpresults.header.time_chain_vec.from_module = out_results.start_stamp;
vpresults.header.time_chain_vec[0].t2 = out_results.send_stamp;
for(const auto &vp:out_results.camera_vpresults)
{
  ara::adsfi::MsgCameraDetectionResult cam_res;
  time_sec = vp.stamp;
  // sec = static_cast<long>(time_sec);  // 秒部分（整数部分）
  // nsec = static_cast<long>((time_sec - sec) * 1e9);  // 纳秒部分（小数部分乘以1e9）
  cam_res.header.timestamp.sec = ADSFI_GET_SEC(time_sec);
  cam_res.header.timestamp.nsec = ADSFI_GET_NSEC(time_sec);
  cam_res.header.frameID = vp.camera_name;
  cam_res.image_objects.detectionOut2d.reserve(vp.vision_objects.size());
  
  int obj_3d_number=0;
  for(const auto &obj:vp.vision_objects)
  {
    if(obj.object_3d.enable)
    {
      obj_3d_number++;
    }
  }
  cam_res.image_objects.detectionOut3d.reserve(obj_3d_number);

  ara::adsfi::MsgHaf2dDetectionOut object2d;
  ara::adsfi::MsgHaf3dDetectionOut object3d;
  for(int idx=0;idx<vp.vision_objects.size();idx++)
  {
    auto& obj = vp.vision_objects[idx];
    object2d.cls = obj.object_2d.obj_id;          // 类别
    object2d.confidence = obj.object_2d.prob;      // 置信度

    // 从中心点+宽高 转换为 四个顶点坐标
    const double x_min = obj.object_2d.x;          // 左上角x (原始坐标)
    const double y_min = obj.object_2d.y;          // 左上角y
    const double x_max = x_min + obj.object_2d.w;  // 右下角x
    const double y_max = y_min + obj.object_2d.h;  // 右下角y

    // 填充四个顶点（顺时针或逆时针顺序，需与您的框架约定一致）
    object2d.rect.corners.clear();  // 清空旧数据（如果重用对象）
    object2d.rect.corners.push_back({x_min, y_min});  // 左上
    object2d.rect.corners.push_back({x_max, y_min});  // 右上
    object2d.rect.corners.push_back({x_max, y_max});  // 右下
    object2d.rect.corners.push_back({x_min, y_max});  // 左下

    // 若需保留center/size/location（可选）
    object2d.rect.size.x = obj.object_2d.w;
    object2d.rect.size.y = obj.object_2d.h;
    object2d.rect.center.x = x_min + obj.object_2d.w * 0.5;
    object2d.rect.center.y = y_min + obj.object_2d.h * 0.5;
    object2d.rect.location.x = x_min;
    object2d.rect.location.y = y_min;
    if(obj.object_3d.enable){
      // 从3D检测结果中提取参数
      // const auto& obj = vp.vision_objects[i];

      // 1. 基础参数（明确坐标系：X前, Y左, Z上）
      double center_x = obj.object_3d.x;             // X坐标（前向）
      double center_y = obj.object_3d.y;             // Y坐标（左向）
      double center_z = obj.object_3d.z;             // Z坐标（上向）
      double length = obj.object_3d.length;          // 沿X轴的尺寸（前后方向）
      double width = obj.object_3d.width;            // 沿Y轴的尺寸（左右方向）
      double height = obj.object_3d.height;          // 沿Z轴的尺寸（上下方向）
      double yaw_deg = obj.object_3d.protate;        // 绕Z轴的旋转角度（度）

      // 2. 转换为弧度
      double yaw = yaw_deg * M_PI / 180.0;

      // 3. 计算旋转参数（绕Z轴旋转）
      double cos_yaw = cos(yaw);
      double sin_yaw = sin(yaw);
      double half_length = length / 2.0;  // X方向半边长
      double half_width = width / 2.0;    // Y方向半边宽
      double half_height = height / 2.0;  // Z方向半边高

      // 4. 定义局部坐标系下的8个顶点（未旋转时）
      // 顶点顺序：底面（Z=-h/2）→ 顶面（Z=+h/2）
      // 每个顶点表示：[X(前±), Y(左±), Z(上±)]
      std::vector<std::array<double, 3>> local_corners = {
          { half_length, -half_width, -half_height}, // 前右下
          { half_length,  half_width, -half_height}, // 前左下
          {-half_length,  half_width, -half_height}, // 后左下
          {-half_length, -half_width, -half_height}, // 后右下
          { half_length, -half_width,  half_height}, // 前右上
          { half_length,  half_width,  half_height}, // 前左上
          {-half_length,  half_width,  half_height}, // 后左上
          {-half_length, -half_width,  half_height}  // 后右上
      };

      // 5. 旋转并平移到世界坐标系
      object3d.rect.corners.clear();
      for (const auto& local_point : local_corners) {
          // Step1: 绕局部坐标系原点旋转（绕Z轴）
          double rotated_x = local_point[0] * cos_yaw - local_point[1] * sin_yaw;
          double rotated_y = local_point[0] * sin_yaw + local_point[1] * cos_yaw;
          double rotated_z = local_point[2]; // Z轴不受Yaw旋转影响

          // Step2: 将旋转后的顶点平移到世界坐标系
          object3d.rect.corners.push_back({
              center_x + rotated_x,  // 世界X坐标
              center_y + rotated_y,  // 世界Y坐标
              center_z + rotated_z   // 世界Z坐标
          });
      }

      // 6. 设置其他属性
      object3d.rect.center.x = center_x;
      object3d.rect.center.y = center_y;
      object3d.rect.center.z = center_z;
      object3d.rect.size.x = length;
      object3d.rect.size.y = width;
      object3d.rect.size.z = height;
      object3d.rect.orientation = yaw_deg;
      object3d.types = idx;
      cam_res.image_objects.detectionOut3d.push_back(object3d);
    }
    // else{
    //   obj.object_3d.enable=false;
    // }
    cam_res.image_objects.detectionOut2d.push_back(object2d);
  }
  //分割语义
  ara::adsfi::MsgImageData image_data;
  //相机长宽
  image_data.timestamp.sec = vp.vision_segment.camera_width;
  image_data.timestamp.nsec = vp.vision_segment.camera_height;
  if(vp.vision_segment.seg_valid)
  {
    
    image_data.width = vp.vision_segment.seg.width;
    image_data.height = vp.vision_segment.seg.height;
    // image_data.rawData = 0;//unsigned char 数组todo
    unsigned int length = vp.vision_segment.seg.data.size();
    image_data.dataSize = length;//image_data.width*image_data.height;
    image_data.rawData.resize(length);
    memcpy(image_data.rawData.data(), vp.vision_segment.seg.data.data(), length);
    image_data.frameID = "seg";
    cam_res.image_segmentation_vec.push_back(image_data);
  }
  //车道线分割
  if(vp.vision_segment.resa_valid)
  {
    image_data.width = vp.vision_segment.resa.width;
    image_data.height = vp.vision_segment.resa.height;
    unsigned int length = vp.vision_segment.resa.data.size();
    image_data.rawData.resize(length);
    image_data.dataSize = length;
    memcpy(image_data.rawData.data(), vp.vision_segment.resa.data.data(), length);
    image_data.frameID = "resa";
    cam_res.image_segmentation_vec.push_back(image_data);
  
    vpresults.vp_result_vec.push_back(cam_res);
  }
}
}

int AdsfiInterface::Process(const std::string &name,std::shared_ptr<ara::adsfi::MsgCameraVPResult> &vpresults_ptr) {
  // std::shared_ptr<ara::adsfi::MsgImageDataList> ptr;
  // image_deque.waitAndPop(ptr);
  // std::shared_ptr<ara::adsfi::MsgImageDataList> image_list;
  // GetImageList(ptr);
  // for(const ara::adsfi::MsgImageData& im_img:ptr->images){
  //     std::cout<<im_img.frameID<<" im_img.height "<<im_img.height<<" im_img.width "<<im_img.width<<" "<<im_img.rawData.size()<<" \n";
  // }   
  // std::cout<<"########################## start detection \n";
  // if(ptr->images[0].rawData.size() == 3110400){
  //   if(yolo_->Detection(ptr->images[0])){

  //   }
  // }

  // //获取yolo数据，里面用了conditional_variable wait
  // if(yolo_->GetData(detections)){
  //   std::cout << "[CUSTOM INFO ] ############### INTERFACE GetData YOLO detection size "<<detections.size()<<"\n";

  //   timeval tm;
  //   gettimeofday(&tm, NULL);

  //   camera_dets_ptr->header.seq = det_seq++;
  //   if(det_seq >= std::numeric_limits<int>::max())
  //     det_seq = 0;
  //   // camera_dets_ptr->header.module_name = "yolo_detection";
  //   // camera_dets_ptr->header.data_name = "camera2d_det";
  //   camera_dets_ptr->header.frameId = "/camera";
  //   camera_dets_ptr->header.stamp.sec = tm.tv_sec;
  //   camera_dets_ptr->header.stamp.nsec = tm.tv_usec * 1e3;

  //   for(auto det:detections){
  //     adsfi::Object2d objecttemp;
  //     objecttemp.clsConfidence = det.confidence;
  //     objecttemp.cls = det.classId;
  //     objecttemp.clsDescription = ara::core::String(det.className);
  //           std::cout<<"[PUBLISH INFO ] objecttemp.cls id "<<(int)objecttemp.cls<<" objecttemp.clsDescription  "<<objecttemp.clsDescription<<"\n";

  //           objecttemp.rect2d.center.x = (det.x1 - det.x0)/2 + det.x0;
  //     objecttemp.rect2d.center.y = (det.y1 - det.y0)/2 + det.y0;
  //     objecttemp.rect2d.size.x = (det.x1 - det.x0);
  //     objecttemp.rect2d.size.y = (det.y1 - det.y0);
  //     camera_dets_ptr->object.push_back(objecttemp);
  //   }
  // }else{
  //   timeval tm;
  //   gettimeofday(&tm, NULL);
  //   // camera_dets_ptr->header.module_name = "yolo_detection";
  //   // camera_dets_ptr->header.data_name = "camera2d_det";
  //   camera_dets_ptr->header.frameId = "/camera";
  //   camera_dets_ptr->header.stamp.sec = tm.tv_sec;
  //   camera_dets_ptr->header.stamp.nsec = tm.tv_usec * 1e3;
  // }


  //
  // std::vector<ara::adsfi::Object2dArray> result2d_objs;
  // std::vector<adsfi::Object3dArray> result3d_objs;
  // std::vector<PerceptionVPResult> segment_result;
  // if(handle.init_status<=0)
  // {
  //   std::string init_err = "init error";
  //   error_handle_ptr->SetInitError(init_err);
  //   //2.初始化失败故障
  //   std::cout<<"XDLOpenMDCVision init error:"<<handle.init_status<<" init_err:"<<init_err<<std::endl;
  // }else{
  //   std::string error_info="";
  //   ara::adsfi::MsgImageDataList debug_out;
  //   std::cout<< " clear 0"<< std::endl;
  //   std::shared_ptr<PerceptionVPResult> temp_vpres(new PerceptionVPResult);
  //   int vp_sesult_status = handle.Process(*ptr, *temp_vpres, debug_out,error_info);
  //   // std::cout<< " clear 1"<< std::endl;

  //   if(vp_sesult_status==0)
  //     ConvertToMsgCameraVPResult(*temp_vpres,*vpresults_ptr);
  //   ReleaseData(ptr);

  //   return vp_sesult_status;
  // }
  // ReleaseData(ptr);

  // camera_image_lists_.clear();
  return -1;
}


void AdsfiInterface::SetImageList(const std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
    if (is_imagelist_use.load() == true) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(image_list_mtx_);
        m_cond.notify_one();
        image_list_ptr = ptr;
    }
}

void AdsfiInterface::GetImageList(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
    std::unique_lock<std::mutex> lock(image_list_mtx_);
    m_cond.wait(lock);
    ptr = image_list_ptr;
    FillData(ptr);
}

void AdsfiInterface::FillData(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
    dvpp::sync(*ptr);
    is_imagelist_use.store(true);
}

void AdsfiInterface::ReleaseData(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
    safe_release(*ptr);
    is_imagelist_use.store(false);
}

