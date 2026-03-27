#include <chrono>
#include <iostream>
#include <map>
#include <opencv2/imgproc.hpp>
#include <set>
#include <tuple>
#include <unordered_map>

#include "../VideoEditYUV420UV12.hpp"
#include "DatetimeUtil.hpp"
#include "EC400.hpp"
#include "VideoParameter.h"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include "protocol/cusermdc610funcinterface.h"
#include "shm_util/zero_copy_shm_mem.h"
#include "zero_copy_shm_mem.h"

#define VIDEO_IMAGE_MAX_WIDTH 1920
#define VIDEO_IMAGE_MAX_HEIGHT 1080

#define VIDEO_OUPUT_SHM_MAX_WIDTH 3840
#define VIDEO_OUPUT_SHM_MAX_HEIGHT 2160

#define MAX_UNCHANGED 90

bool ImageProcessToData(const std::vector<SplitImageParame>& splitImages, void* tmp_image,
                        std::shared_ptr<avos::idpware::CUserMdc610FuncInterface> img_process_ptr, int width, int height,
                        void* canvas) {
    ApInfo("video_editor") << "setMaxAreaToMainCamera [ImageProcess] Split count: " << splitImages.size();
    if (splitImages.empty()) {
        EC400::Instance()->ec_add(EC400::SPLIT_INVALID, "ImageProcessToData");
        return false;
    }

    // 筛选有效输入
    std::vector<SplitImageParame> validImages;
    validImages.reserve(splitImages.size());
    for (auto& p : splitImages) {
        int vw = static_cast<int>(p.view_right_bottom_point.x - p.view_left_top_point.x);
        int vh = static_cast<int>(p.view_right_bottom_point.y - p.view_left_top_point.y);
        ApInfo("video_editor") << "setMaxAreaToMainCamera channel name: " << p.channel_name << ",width=" << p.width
                               << ",height=" << p.height << ",vw=" << vw << ",vh=" << vh;
        if (!p.src_ptr || p.width <= 0 || p.height <= 0) {
            continue;
        }

        ApInfo("video_editor") << "setMaxAreaToMainCamera channel name: " << p.channel_name << "   width: " << vw
                               << "   height: " << vh;
        if (vw <= 0 || vh <= 0) {
            continue;
        }

        validImages.push_back(p);
    }

    ApInfo("video_editor") << "setMaxAreaToMainCamera validImages size=" << validImages.size();
    if (validImages.empty()) {
        EC400::Instance()->ec_add(EC400::SPLIT_INVALID);
        return false;
    }
    EC400::Instance()->ec_remove(EC400::SPLIT_INVALID);

    static int  index_value = 1;
    std::string idx_str = std::to_string(index_value++);

    ApInfo("video_editor") << "setMaxAreaToMainCamera name:" << validImages[0].channel_name
                           << ",width=" << validImages[0].width << ",height=" << validImages[0].height;

    bool main_done = false;
    for (auto& p : validImages) {
        int view_x = static_cast<int>(p.view_left_top_point.x);
        int view_y = static_cast<int>(p.view_left_top_point.y);
        int view_w = static_cast<int>(p.view_right_bottom_point.x - p.view_left_top_point.x);
        int view_h = static_cast<int>(p.view_right_bottom_point.y - p.view_left_top_point.y);
        int crop_x = static_cast<int>(p.crop_left_top_point.x);
        int crop_y = static_cast<int>(p.crop_left_top_point.y);
        int crop_w = static_cast<int>(p.crop_right_bottom_point.x - p.crop_left_top_point.x);
        int crop_h = static_cast<int>(p.crop_right_bottom_point.y - p.crop_left_top_point.y);

        // 初始化内存
        if (p.index == 0) {
            avos::idpware::SImgProcessInfo meminfo;
            // meminfo.m_buf_len = width * height * 3 / 2;
            // ApInfo("video_editor") << "idpDvppMalloc";
            // img_process_ptr->idpDvppMalloc(meminfo);
            // p.src_ptr = static_cast<u_int8_t*>(meminfo.m_buf);
            EC400::Instance()->ec_add(EC400::DATA_INVALID, "ImageProcessToData");
            return false;
        }
        EC400::Instance()->ec_remove(EC400::DATA_INVALID);

        // 统一封装处理过程
        auto process = [&](const std::string& name, auto setupFunc) {
            avos::idpware::SImgProcess param;
            param.SetProcessName(name);
            param.InitInputImage(p.src_ptr, p.totalSize, p.width, p.height, avos::idpware::BUF_CPU_TYPE,
                                 avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420);
            param.InitOutputImage(canvas, width * height * 3 / 2, width, height, avos::idpware::BUF_CPU_TYPE,
                                  avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420);
            setupFunc(param);
            img_process_ptr->idpImgProcess(param);
        };

        // 裁剪 + 粘贴逻辑
        if (crop_w > 0 && crop_h > 0 && crop_x + crop_w <= p.width && crop_y + crop_h <= p.height) {
            if (crop_w == view_w && crop_h == view_h) {
                process("crop_" + p.channel_name,
                        [&](auto& p_) { p_.SetCropPaste(crop_w, crop_h, crop_x, crop_y, view_x, view_y); });
            } else {
                process("resize_" + p.channel_name, [&](auto& p_) {
                    p_.SetCropResizePaste(crop_w, crop_h, crop_x, crop_y, view_w, view_h, view_x, view_y);
                });
            }
        } else {
            process("resize_new_" + p.channel_name,
                    [&](auto& p_) { p_.SetResizePaste(p.width, p.height, view_w, view_h, view_x, view_y); });
        }
    }

    return true;
}

bool XImageLayout(const ara::adsfi::MsgImageDataList& video_list, // 视频组
                  const compatible::VideoParameter&   parame,     // 视频绘制参数
                  const ara::adsfi::MsgImageData&     stitch,     // 环视拼接图像
                  const ara::adsfi::MsgImageData&     image_in,   // 前/后/前夜视/后夜视相机
                  ara::adsfi::MsgImageData&           image_out) {
    ApInfo("video_editor") << "XImageLayout  start";
    for (const auto& img : video_list.images) {
        ApInfo("video_editor") << fmt::format("XImageLayout input image name: {}, time: {:20.8f}", img.frameID,
                                              app_common::DatetimeUtil::double_time_from_sec_nsec(img.timestamp));
    }
    ApInfo("video_editor") << fmt::format("XImageLayout input stitch name: {}, time: {:20.8f}", stitch.frameID,
                                          app_common::DatetimeUtil::double_time_from_sec_nsec(stitch.timestamp));
    ApInfo("video_editor") << fmt::format("XImageLayout input image_in name: {}, time: {:20.8f}", image_in.frameID,
                                          app_common::DatetimeUtil::double_time_from_sec_nsec(image_in.timestamp));

    static bool                                                     init = false;
    static void*                                                    result_image = nullptr;
    static void*                                                    temp_image = nullptr;
    static unsigned int                                             seq;
    static ZeroCopyShmMem                                           zero_shm_mem;
    static std::shared_ptr<avos::idpware::CUserMdc610FuncInterface> img_process_ptr =
        avos::idpware::CUserMdc610FuncInterface::Instance();

    //执行周期时间
    //检测数据有效性
    if (parame.editor_parameter.split_parames_.size() <= 0) {
        ApError("video_editor") << "XImageLayout error "
                                   "parame.editor_parameter.split_parames_.size()  <= 0";
        EC400::Instance()->ec_add(EC400::SPLIT_INVALID, "XImageLayout");
        return false;
    } else {
        EC400::Instance()->ec_remove(EC400::SPLIT_INVALID);
    }

    const auto& video_list_ = video_list.images;
    if (video_list_.size() <= 0) {
        ApInfo("video_editor") << "XImageLayout error video_list_ size is 0, 相机数据无输入";
        EC400::Instance()->ec_add(EC400::DATA_EMPTY, "XImageLayout");
        return false;
    } else {
        EC400::Instance()->ec_remove(EC400::DATA_EMPTY);
    }

    if (!init) {
        avos::idpware::SImgProcessInfo meminfo;
        meminfo.m_buf_len = VIDEO_IMAGE_MAX_WIDTH * VIDEO_IMAGE_MAX_HEIGHT * 3 / 2;
        img_process_ptr->idpDvppMalloc(meminfo);
        result_image = meminfo.m_buf;

        avos::idpware::SImgProcessInfo tmp_meminfo;
        tmp_meminfo.m_buf_len = VIDEO_IMAGE_MAX_WIDTH * VIDEO_IMAGE_MAX_HEIGHT * 3 / 2;
        img_process_ptr->idpDvppMalloc(tmp_meminfo);
        temp_image = tmp_meminfo.m_buf;

        init = true;
    }

    //筛选需要拼接的数据
    std::vector<SplitImageParame> split_images;
    ApInfo("video_editor") << "XImageLayout start parse name";
    for (const auto& item : parame.editor_parameter.split_parames_) {
        ApInfo("video_editor") << "XImageLayout item.channel_name:" << item.channel_name;
        if (item.channel_name == "fisheye_stitched") {
            ApInfo("video_editor") << "fisheye_stitched";
            auto             shm_stitch_ptr = zero_shm_mem.GetAllocShmMem(stitch.frameID, stitch.dataSize, stitch.seq);
            SplitImageParame image_parame;
            image_parame.src_ptr = shm_stitch_ptr;
            if (image_parame.src_ptr == nullptr) {
                image_parame.index = 0;
            } else {
                image_parame.index = 1;
            }

            if (stitch.width <= 0 || stitch.height <= 0) {
                ApInfo("video_editor") << "XImageLayout error,size error width=" << stitch.width
                                       << ",height=" << stitch.height;
                EC400::Instance()->ec_add(EC400::DATA_INVALID, "stitch");
                return false;
            }
            EC400::Instance()->ec_remove(EC400::DATA_INVALID);

            image_parame.width = stitch.width;
            image_parame.height = stitch.height;
            image_parame.totalSize = stitch.width * stitch.height * 1.5;
            image_parame.channel_name = item.channel_name;
            image_parame.view_left_top_point =
                cv::Point2f(item.view_area_points.left_top_point_x, item.view_area_points.left_top_point_y);
            image_parame.view_right_bottom_point =
                cv::Point2f(item.view_area_points.right_bottom_point_x, item.view_area_points.right_bottom_point_y);
            image_parame.crop_left_top_point =
                cv::Point2f(item.crop_area_points.left_top_point_x, item.crop_area_points.left_top_point_y);
            image_parame.crop_right_bottom_point =
                cv::Point2f(item.crop_area_points.right_bottom_point_x, item.crop_area_points.right_bottom_point_y);
            split_images.push_back(std::move(image_parame));
        } else {
            // 查找同名图像
            auto it =
                std::find_if(video_list.images.begin(), video_list.images.end(),
                             [&](const ara::adsfi::MsgImageData& img) { return img.frameID == item.channel_name; });
            if (it == video_list.images.end()) {
                ApInfo("video_editor") << "XImageLayout error item.channel_name:" << item.channel_name
                                       << " not found in video_list";
                continue;
            }

            SplitImageParame image_parame;
            image_parame.width = it->width;
            image_parame.height = it->height;
            image_parame.totalSize = it->width * it->height * 1.5;
            image_parame.channel_name = item.channel_name;
            image_parame.view_left_top_point =
                cv::Point2f(item.view_area_points.left_top_point_x, item.view_area_points.left_top_point_y);
            image_parame.view_right_bottom_point =
                cv::Point2f(item.view_area_points.right_bottom_point_x, item.view_area_points.right_bottom_point_y);
            image_parame.crop_left_top_point =
                cv::Point2f(item.crop_area_points.left_top_point_x, item.crop_area_points.left_top_point_y);
            image_parame.crop_right_bottom_point =
                cv::Point2f(item.crop_area_points.right_bottom_point_x, item.crop_area_points.right_bottom_point_y);

            // 绘制路径（仅前/后相机）
            std::string main_camera = "put_" + item.channel_name;
            ApInfo("video_editor") << "XImageLayout main_camera:" << main_camera;
            ApInfo("video_editor") << "XImageLayout item.channel_name:" << image_in.frameID;

            int view_width = item.view_area_points.right_bottom_point_x - item.view_area_points.left_top_point_x;
            if (!(parame.editor_parameter.objects.fusionOut.size() <= 0 &&
                  parame.editor_parameter.path.trajectoryPoints.size() <= 0 &&
                  parame.editor_parameter.remote_control.trajectoryPoints.size() <= 0) &&
                main_camera == image_in.frameID && view_width > VIDEO_IMAGE_MAX_WIDTH / 2) {
                auto shm_front_ptr = zero_shm_mem.GetAllocShmMem(
                    image_in.frameID, VIDEO_OUPUT_SHM_MAX_WIDTH * VIDEO_OUPUT_SHM_MAX_HEIGHT * 3 / 2, image_in.seq);
                if (shm_front_ptr == nullptr) {
                    ApError("video_editor") << "zero_shm_mem_back_ptr error this is shm_ptr is nullther";
                    EC400::Instance()->ec_add(EC400::SHM_FAILED, "XImageLayout");
                    return false;
                }
                EC400::Instance()->ec_remove(EC400::SHM_FAILED);
                image_parame.src_ptr = (void*)shm_front_ptr;
            } else {
                image_parame.src_ptr = (void*)(it->rawData.data());
            }

            // image_parame.index = ImageIsNull(item.channel_name, stuck_channels) ? 0 : 1;
            image_parame.index = 1;
            split_images.push_back(std::move(image_parame));
        }
    }

    if (split_images.empty()) {
        ApError("video_editor") << "XImageLayout error video is empty";
        EC400::Instance()->ec_add(EC400::SPLIT_INVALID, "XImageLayout");
        return false;
    }
    EC400::Instance()->ec_remove(EC400::SPLIT_INVALID);

    bool reult = ImageProcessToData(split_images, temp_image, img_process_ptr, VIDEO_IMAGE_MAX_WIDTH,
                                    VIDEO_IMAGE_MAX_HEIGHT, result_image);
    if (!reult) {
        ApInfo("video_editor") << "XImageLayout error result is false";
        EC400::Instance()->ec_add(EC400::DATA_RESULT_NULL);
        return false;
    } else if (result_image == nullptr) {
        ApInfo("video_editor") << "XImageLayout error result_image is null";
        EC400::Instance()->ec_add(EC400::DATA_RESULT_NULL);
        return false;
    } else {
        EC400::Instance()->ec_remove(EC400::DATA_RESULT_NULL);
    }

    //     //处理筛选后的数据
    image_out.timestamp = video_list.header.timestamp;
    image_out.seq = seq++;
    image_out.height = VIDEO_IMAGE_MAX_HEIGHT;
    image_out.width = VIDEO_IMAGE_MAX_WIDTH;
    image_out.imageType = video_list.images[0].imageType;
    image_out.bufferType = 4;

    image_out.frameID = "yuyv_merge_dvp";
    image_out.dataSize = VIDEO_IMAGE_MAX_WIDTH * VIDEO_IMAGE_MAX_HEIGHT * 3 / 2;

    auto shm_ptr = zero_shm_mem.GetAllocShmMem(image_out.frameID, image_out.dataSize, image_out.seq);
    ApInfo("video_editor") << "[GetAllocShmMem AA]" << image_out.frameID << " " << image_out.dataSize;
    if (nullptr == shm_ptr || nullptr == result_image) {
        ApError("video_editor") << "XImageLayout error this is shm_ptr is nullther";
        EC400::Instance()->ec_add(EC400::SHM_FAILED, "XImageLayout");
        return false;
    }
    EC400::Instance()->ec_remove(EC400::SHM_FAILED);

    memcpy(shm_ptr, result_image, image_out.width * image_out.height * 3 / 2);

    ApInfo("video_editor") << "XImageLayout end";
    return true;
}