
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <iostream>
#include <memory>
#include <string>

#include "DatetimeUtil.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include "image_data_interface/image_data_interface.hpp"
#include "protocol/imgdvppplatform2usrprotocol.h"
#include "zero_copy_shm_mem.h"
#include "transparent_chassis.h"
#include "png_image.h"
#include "yuv_udp.h"

#define STITCH_IMAGE_HIGHT 1920
#define STITCH_IMAGE_WIDTH 1280
#define STITCH_IMAGE_SIZE (STITCH_IMAGE_WIDTH * STITCH_IMAGE_HIGHT * 3 / 2)

typedef u_int8_t *Pbuffer;



inline std::string GetStitchConfigPath(const std::string &path, int num){
    std::string bin_path = path + "/camera_stitch_" + std::to_string(num) + ".bin";
    switch (num) {
        case 0:
            bin_path = path + "/data_front_fish_eye.bin";
            break;
        case 1:
            bin_path = path + "/data_back_fish_eye.bin";
            break;
        case 2:
            bin_path = path + "/data_left_fish_eye.bin";
            break;
        case 3:
            bin_path = path + "/data_right_fish_eye.bin";
            break;
    }
    return bin_path;
}


inline void DeserializeBin(const std::string& filename, int& a, int& b, int& c, int& d) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "无法打开文件 " << filename << std::endl;
        return;
    }

    // 读取四个int值
    file.read(reinterpret_cast<char*>(&a), sizeof(a));
    file.read(reinterpret_cast<char*>(&b), sizeof(b));
    file.read(reinterpret_cast<char*>(&c), sizeof(c));
    file.read(reinterpret_cast<char*>(&d), sizeof(d));


    file.close();
}


class CMdcCmaeraStitchInterfaceV2 {
public:
    CMdcCmaeraStitchInterfaceV2(const std::string &_so_path, const std::string &_config_path) {
        so_path = _so_path;
        config_path = _config_path;
    }

    ~CMdcCmaeraStitchInterfaceV2() {
    }

    int Init() {
        // ApLogInterface::Instance()->Init("MdcCameraStitch");
        mdc_stitch_ptr = std::make_shared<avos::idpware::CIdpAbstractPlatformInterface>();
        mdc_stitch_ptr->open(so_path.c_str(), RTLD_LAZY);

        dvpp_cfg_list.m_dstheight = STITCH_IMAGE_HIGHT;
        dvpp_cfg_list.m_dstwidth = STITCH_IMAGE_WIDTH;
        for (int var = 0; var < STITCH_NUM; ++var) {
            dvpp_cfg_list.data[var].m_pos = var; // 0:前，1:后，2：左， 3：右
            dvpp_cfg_list.data[var].m_channelid = 90 + var;
            dvpp_cfg_list.data[var].m_inwidth = STITCH_IMAGE_HIGHT;
            dvpp_cfg_list.data[var].m_inheight = STITCH_IMAGE_WIDTH;
            dvpp_cfg_list.data[var].m_outwidth = STITCH_IMAGE_WIDTH;
            dvpp_cfg_list.data[var].m_outheight = STITCH_IMAGE_HIGHT;

            //拼接时的有效roi区域
            dvpp_cfg_list.data[var].m_roileft = 0 + 4; // dvpp 要求宽度必须是16的整数倍 1280显示到1280左右留4个像素
            dvpp_cfg_list.data[var].m_roitop = 0;
            dvpp_cfg_list.data[var].m_roiwidth = 0;
            dvpp_cfg_list.data[var].m_roiheight = 0;

            std::string data_path = GetStitchConfigPath(config_path, var);
            std::cout << "stitch_config_path:" << data_path << std::endl;

            memcpy(dvpp_cfg_list.data[var].m_datapath, data_path.c_str(), data_path.size());
            dvpp_cfg_list.data[var].m_datapathlen = data_path.size();
        }

        mdc_stitch_ptr->ioctl(avos::idpware::imgstitchdvpp::IOCTL_INIT_DVPP, &(dvpp_cfg_list));

        write_seq.store(0);

        return 0;
    }

    Pbuffer FillShmBuffer(std::shared_ptr<ara::adsfi::MsgImageData> &image) {
        // while (1) {
        write_seq.store(write_seq.load() + 1);
        int index = write_seq.load();
        image->frameID = "camera_stitch_info";
        image->dataSize = STITCH_IMAGE_SIZE;
        image->seq = index;

        Pbuffer shm_ptr = (Pbuffer)(zero_copy_mem.GetAllocShmMem(image->frameID, image->dataSize, image->seq));

        return shm_ptr;
    }

    Pbuffer GetBuffer(const std::shared_ptr<ara::adsfi::MsgImageData> &image){
        return (Pbuffer)(zero_copy_mem.GetAllocShmMem(image->frameID, image->dataSize, image->seq));
    }

    void Process(Pbuffer front_image, Pbuffer left_image, Pbuffer right_image, Pbuffer back_image,
                 std::shared_ptr<ara::adsfi::MsgImageData> &image) {
        Pbuffer stitch_ptr = FillShmBuffer(image);

        inoutstitch.m_inputdataary[0].m_ptr = front_image;
        inoutstitch.m_inputdataary[0].m_ptr_len = STITCH_IMAGE_SIZE;

        inoutstitch.m_inputdataary[1].m_ptr = back_image;
        inoutstitch.m_inputdataary[1].m_ptr_len = STITCH_IMAGE_SIZE;

        inoutstitch.m_inputdataary[2].m_ptr = left_image;
        inoutstitch.m_inputdataary[2].m_ptr_len = STITCH_IMAGE_SIZE;

        inoutstitch.m_inputdataary[3].m_ptr = right_image;
        inoutstitch.m_inputdataary[3].m_ptr_len = STITCH_IMAGE_SIZE;

        inoutstitch.m_outputdata.m_ptr = stitch_ptr;
        inoutstitch.m_outputdata.m_ptr_len = STITCH_IMAGE_SIZE;

        mdc_stitch_ptr->ioctl(avos::idpware::imgstitchdvpp::IOCTL_STITCH_DVPP, &(inoutstitch));
    }

private:
    std::string so_path;
    std::string config_path;

    u_int8_t *                                                    user_buffer_cache;
    avos::idpware::imgstitchdvpp::SImgDvppStitch                  inoutstitch;
    avos::idpware::imgstitchdvpp::SImgDvppCfgList                 dvpp_cfg_list;
    std::shared_ptr<avos::idpware::CIdpAbstractPlatformInterface> mdc_stitch_ptr;

    std::atomic<int> write_seq;

    ZeroCopyShmMem zero_copy_mem;

public:
    int GetImageInfo(const ara::adsfi::MsgImageDataList &image_list, const std::string &image_name,
                     Pbuffer &image_buffer) {
        // ZeroCopyShmMem zero_copy_mem;
        for (const auto &p : image_list.images) {
            if (p.frameID == image_name) {
                image_buffer = (Pbuffer)p.rawData.data();
                std::cout << "GetImageInfo :" << image_name << "," << p.rawData.size() << std::endl;

                return 0;
            }
        }
        return -1;
    }
};



struct SelfPointGCCS
{
  double xg;
  double yg;
  double angle;

  SelfPointGCCS(){
    angle = -1000000;

  }
 
};
struct SelfPointVCS
{
  double x;
  double y;
  double angle;
};

inline int SelfGeoTransform(const SelfPointGCCS &car_pgccs, const SelfPointGCCS &target_pgccs,
                 SelfPointVCS &output_pvcs)
{
  // RETURN_EQ(const_cast<SelfPointGCCS &>(car_pgccs).Check(), 0);
  // RETURN_EQ(const_cast<SelfPointGCCS &>(target_pgccs).Check(), 0);
  double rad = car_pgccs.angle * 3.1415926/180.0;
  double dx = target_pgccs.xg - car_pgccs.xg;
  double dy = target_pgccs.yg - car_pgccs.yg;
  output_pvcs.x = +dx * cos(rad) + dy * sin(rad);
  output_pvcs.y = -dx * sin(rad) + dy * cos(rad);
  output_pvcs.angle = target_pgccs.angle - car_pgccs.angle;
  if (output_pvcs.angle >= 360)
    output_pvcs.angle -= 360;
  return 0;
}

inline int SelfGeoTransform(const SelfPointGCCS &car_pgccs, const SelfPointVCS &target_pvcs,
                 SelfPointGCCS &output_pgccs)
{
  // RETURN_EQ(const_cast<SelfPointGCCS &>(car_pgccs).Check(), 0);
  double rad = car_pgccs.angle * 3.1415926/180.0;
  output_pgccs.xg = target_pvcs.x * cos(rad) - target_pvcs.y * sin(rad);
  output_pgccs.yg = target_pvcs.x * sin(rad) + target_pvcs.y * cos(rad);
  output_pgccs.xg += car_pgccs.xg;
  output_pgccs.yg += car_pgccs.yg;
  output_pgccs.angle = car_pgccs.angle + target_pvcs.angle;
  if (output_pgccs.angle >= 360)
    output_pgccs.angle -= 360;
  return 0;
}



class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() {
        front_image_name = "front_fisheye";
        right_image_name = "right_fisheye";
        back_image_name = "back_fisheye";
        left_image_name = "left_fisheye";

        camera_stitch_data_ptr = nullptr;
        is_imagelist_use.store(false);
    }
    ~AdsfiInterface() {
    }

    void Init() override {
        SetScheduleInfo("message");
        std::cout << "image stitch init" << std::endl;

        auto ptr = CustomStack::Instance();

        std::string so_file_name = "libimgdvpp_stitch_plugin_v2.so";
        ptr->GetConfig("MdcCameraStitchV2", "front_image_name", front_image_name);
        ptr->GetConfig("MdcCameraStitchV2", "right_image_name", right_image_name);
        ptr->GetConfig("MdcCameraStitchV2", "back_image_name", back_image_name);
        ptr->GetConfig("MdcCameraStitchV2", "left_image_name", left_image_name);
        ptr->GetConfig("MdcCameraStitchV2", "so_file_name", so_file_name);

        ApInfo("stitch") << "front_image_name: " << front_image_name;
        ApInfo("stitch") << "right_image_name: " << right_image_name;
        ApInfo("stitch") << "back_image_name: " << back_image_name;
        ApInfo("stitch") << "left_image_name: " << left_image_name;

        std::string project_config_path = ptr->GetNodeConfigPath();
        std::string so_path = project_config_path + "/config/" + so_file_name;
        ApInfo("stitch") << "load so:" << so_path;
        camera_stitch_data_ptr = std::make_shared<CMdcCmaeraStitchInterfaceV2>(so_path, ptr->GetVehicleConfigPath());

        camera_stitch_data_ptr->Init();


        if(png_loader.load(project_config_path+"/config/vehicle.png")){
            std::cout << "load image OK"<<std::endl;
        }else{
            std::cout << "error load .... "<< project_config_path+"/config/vehicle.png"<<std::endl;
        }

        chassis_config.pixel_per_meter = 100;
        chassis_config.surround_width = STITCH_IMAGE_WIDTH;
        chassis_config.surround_height = STITCH_IMAGE_HIGHT;

        SelfRect  top_rect, back_rect,left_rect, right_rect;
        // 0:前，1:后，2：左， 3：右
        DeserializeBin(GetStitchConfigPath(ptr->GetVehicleConfigPath(), 0), top_rect.x, top_rect.y, top_rect.width, top_rect.height);
        DeserializeBin(GetStitchConfigPath(ptr->GetVehicleConfigPath(), 1), back_rect.x, back_rect.y, back_rect.width, back_rect.height);
        DeserializeBin(GetStitchConfigPath(ptr->GetVehicleConfigPath(), 2), left_rect.x, left_rect.y, left_rect.width, left_rect.height);
        DeserializeBin(GetStitchConfigPath(ptr->GetVehicleConfigPath(), 3), right_rect.x, right_rect.y, right_rect.width, right_rect.height);
        std::vector<SelfRect> rects;
        rects.push_back(top_rect);
        rects.push_back(back_rect);
        rects.push_back(left_rect);
        rects.push_back(right_rect);

        chassis_config.chassis_rect = find_inner_rect(rects);

        // chassis_config.chassis_rect.x = 515;
        // chassis_config.chassis_rect.y = 710;
        // chassis_config.chassis_rect.width = 250;
        // chassis_config.chassis_rect.height = 500;
        chassis_config.vehicle_anchor_x = STITCH_IMAGE_WIDTH/2;
        chassis_config.vehicle_anchor_y = STITCH_IMAGE_HIGHT/2;

        chassis_config.min_enqueue_distance = 0.5;
        chassis_handler.setConfig(chassis_config);
        // yuv_sender.init("26.28.1.22", 10020);

        ApInfo("stitch") << "load so over";
    }

    void DeInit() override {
        ApInfo("stitch") << "image stitch  deinit" ;
    }

    void Callback(const std::string name, const std::shared_ptr<ara::adsfi::MsgImageDataList> &image_list_ptr) {
        ApInfo("stitch") << fmt::format("image_list: {:20.8f}", app_common::DatetimeUtil::double_time_from_sec_nsec(
                                                                   image_list_ptr->header.timestamp));
        for (const auto &img : image_list_ptr->images) {
            ApInfo("stitch") << fmt::format(
                "image, timestamp: {:20.8f}, frameID: {}, width: {}, height: {}, datasize: {}, bufftype: {}, imagetype: "
                "{}, rawdata.size(): {}",
                app_common::DatetimeUtil::double_time_from_sec_nsec(img.timestamp), img.frameID, img.width, img.height,
                img.dataSize, static_cast<std::int32_t>(img.bufferType), static_cast<std::int32_t>(img.imageType),
                img.rawData.size());
        }
        SetImageList(image_list_ptr);
    }

    void Callback(const std::string name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &loc_ptr){
        double t = ADSFI_TO_SEC(loc_ptr->header.timestamp.sec, loc_ptr->header.timestamp.nsec);
        // static SelfPointGCCS first_gccs;

        // if(first_gccs.angle < -1000){
        //     first_gccs.xg = loc_ptr->dr_pose.pose.position.x;
        //     first_gccs.yg = loc_ptr->dr_pose.pose.position.y;
        //     first_gccs.angle = loc_ptr->dr_pose.pose.orientation.z;
        // }

        // SelfPointGCCS temp_gccs;
        // temp_gccs.xg = loc_ptr->dr_pose.pose.position.x;
        // temp_gccs.yg = loc_ptr->dr_pose.pose.position.y;
        // temp_gccs.angle = loc_ptr->dr_pose.pose.orientation.z;

        // SelfPointVCS tmp_out;
        // SelfGeoTransform(first_gccs,temp_gccs,tmp_out );

        ApInfo("stitch") << fmt::format(
                "get loc t|x|y|heading: {} ,{:20.8f}, {:20.8f},{:20.8f}  ",
                name, loc_ptr->dr_pose.pose.position.x, loc_ptr->dr_pose.pose.position.y, loc_ptr->dr_pose.pose.orientation.z);
        

        // std::cout << "rel pos:"<<tmp_out.x<<"|"<<tmp_out.y<<std::endl;
        chassis_handler.setVehiclePose(t,
            loc_ptr->dr_pose.pose.position.x, loc_ptr->dr_pose.pose.position.y, loc_ptr->dr_pose.pose.orientation.z* M_PI / 180.0);
    }

    void overlayVehicle(YuvFrame& frame) {
      
        const uint8_t kGreenY = 64, kGreenU = 112, kGreenV = 116;
        const float   kGreenAlpha = 0.2f;
        for (int dy = 0; dy < chassis_config.chassis_rect.height; ++dy) {
            for (int dx = 0; dx < chassis_config.chassis_rect.width; ++dx) {
                int px = chassis_config.chassis_rect.x + dx;
                int py = chassis_config.chassis_rect.y + dy;

                // 1. 叠加半透明军绿色（YUV）
                chassis_handler.setPixYuv(frame, px, py, kGreenY, kGreenU, kGreenV, kGreenAlpha);

                // 2. 叠加 vehicle.png（alpha 混合，RGB->YUV）
                if (png_loader.loaded()) {
                    uint8_t b, g, r, a;
                    if (png_loader.getpix(chassis_config.chassis_rect.width, 
                        chassis_config.chassis_rect.height, 
                        dx, dy, b, g, r, a) && a > 0) {
                        float af = a / 255.0f;
                        chassis_handler.setPixRgb(frame, px, py, r, g, b, af);
                    }
                }
            }
        }
    }
    

    int Process(const std::string name, std::shared_ptr<ara::adsfi::MsgImageData> &image) {
        std::shared_ptr<ara::adsfi::MsgImageDataList> image_list;
        GetImageList(image_list);

        if (camera_stitch_data_ptr->GetImageInfo(*image_list, front_image_name, front_image) != 0 ||
            nullptr == front_image) {
            ApError("stitch") << "error cannot find image found_front_res:" << front_image_name;
            ReleaseData(image_list);
            return -1;
        }

        if (camera_stitch_data_ptr->GetImageInfo(*image_list, left_image_name, left_image) != 0 ||
            nullptr == left_image) {
            ApError("stitch") << "error cannot find image found_left_res:" << left_image_name;
            ReleaseData(image_list);
            return -1;
        }
        if (camera_stitch_data_ptr->GetImageInfo(*image_list, right_image_name, right_image) != 0 ||
            nullptr == right_image) {
            ApError("stitch") << "error cannot find image found_right_res:" << right_image_name;
            ReleaseData(image_list);
            return -1;
        }
        if (camera_stitch_data_ptr->GetImageInfo(*image_list, back_image_name, back_image) != 0 ||
            nullptr == back_image) {
            ApError("stitch") << "error cannot find image found_back_res:" << back_image_name;
            ReleaseData(image_list);
            return -1;
        }

        image->timestamp = image_list->header.timestamp;
        image->bufferType = 4; // usr shm
        image->imageType = 8;

        image->height = 1920;
        image->width = 1280;
        camera_stitch_data_ptr->Process(front_image, left_image, right_image, back_image, image);

        Pbuffer shm_ptr = camera_stitch_data_ptr->GetBuffer(image);
        YuvFrame fame_data(image->width, image->height, shm_ptr, STITCH_IMAGE_SIZE);
        chassis_handler.processFrame(ADSFI_TO_SEC(image_list->header.timestamp.sec, image_list->header.timestamp.nsec), fame_data);
        overlayVehicle(fame_data);


        // static int seq_frame = 0;
        // std::cout << "send frame "<< seq_frame<<std::endl;
        // seq_frame++;
        // if(seq_frame%3 == 0){
        //     yuv_sender.sendFrame(seq_frame/3, image->width, image->height, shm_ptr, STITCH_IMAGE_SIZE);
        // }
        

        ReleaseData(image_list);
        return 0;
    }

    void SetImageList(const std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
        if (is_imagelist_use.load() == true) {
            return;
        }
        {
            std::lock_guard<std::mutex> lock(image_list_mtx_);
            image_list_ptr = ptr;
            m_cond.notify_one();
        }
    }

    void GetImageList(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
        std::unique_lock<std::mutex> lock(image_list_mtx_);
        m_cond.wait(lock);
        ptr = image_list_ptr;
        FillData(ptr);
    }

    void FillData(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
        dvpp::sync(*ptr);
        is_imagelist_use.store(true);
    }

    void ReleaseData(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
        safe_release(*ptr);
        is_imagelist_use.store(false);
    }

private:
    ThreadSafeVector<std::shared_ptr<ara::adsfi::MsgImageDataList>> camera_image_lists_;
    ThreadSafeVector<std::shared_ptr<ara::adsfi::MsgImageData>>     stiched_images_;

    std::atomic<bool>                             is_imagelist_use;
    std::mutex                                    image_list_mtx_;
    std::condition_variable                       m_cond;
    std::shared_ptr<ara::adsfi::MsgImageDataList> image_list_ptr;

    std::string front_image_name;
    std::string right_image_name;
    std::string back_image_name;
    std::string left_image_name;

    std::shared_ptr<CMdcCmaeraStitchInterfaceV2> camera_stitch_data_ptr;

    Pbuffer front_image;
    Pbuffer left_image;
    Pbuffer right_image;
    Pbuffer back_image;

TransparentChassis  chassis_handler;
TransparentChassisConfig chassis_config;
PngImage png_loader;
// YuvUdpSender yuv_sender;
public:
    BASE_TEMPLATE_FUNCION
};
#endif
