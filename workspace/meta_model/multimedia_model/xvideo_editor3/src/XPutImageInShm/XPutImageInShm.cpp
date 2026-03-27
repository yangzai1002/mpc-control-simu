#include <chrono>
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>

#include "DatetimeUtil.hpp"
#include "VideoParameter.h"
#include "EC400.hpp"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_appvideosplitparam.h"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include "shm_util/shm_param.h"
#include "shm_util/zero_copy_shm_mem.h"
#include "zero_copy_shm_mem.h"

#define VIDEO_IMAGE_MAX_WIDTH 1920
#define VIDEO_IMAGE_MAX_HEIGHT 1080

#define VIDEO_OUPUT_SHM_MAX_WIDTH 3840
#define VIDEO_OUPUT_SHM_MAX_HEIGHT 2160

#define MAX_UNCHANGED 90

void setMaxAreaToMainCamera(const ara::adsfi::MsgImageDataList&                video_list,
                            const std::vector<ara::adsfi::AppVideoSplitParam>& split_parames) {
    float       max_area = -1.0f;
    std::string max_area_camera_ = "";
    for (const auto& item : split_parames) {
        ApInfo("video_editor") << "setMaxAreaToMainCamera item.channel_name:" << item.channel_name;
        // 查找同名图像
        auto it = std::find_if(video_list.images.begin(), video_list.images.end(),
                               [&](const ara::adsfi::MsgImageData& img) { return img.frameID == item.channel_name; });
        if (it == video_list.images.end())
            continue;

        float w = item.view_area_points.right_bottom_point_x - item.view_area_points.left_top_point_x;
        float h = item.view_area_points.right_bottom_point_y - item.view_area_points.left_top_point_y;
        float area = w * h;
        if (area <= 0)
            continue; // 跳过无效区域

        if (area > max_area) {
            max_area = area;
            max_area_camera_ = item.channel_name;
        }
    }

    // 最终只保留最大的
    if (max_area_camera_ != "") {
        for (int index = 0; index < video_list.images.size(); index++) {
            if (video_list.images[index].frameID == max_area_camera_) {
                SHMParamApi::Instance()->AddModuleParam("global");
                SHMParamApi::Instance()->SetIntModuleParam("mdc_camera_main_idx", index);
            }
        }
    }
}

bool XPutImageInShm(const ara::adsfi::MsgImageDataList& video_list, // 视频组
                    const compatible::VideoParameter&   parame,     // 视频绘制参数
                    ara::adsfi::MsgImageData&           image) {
    static ZeroCopyShmMem zero_shm_mem;
    ApInfo("video_editor") << "XPutImageInShm start";
    for (const auto& img : video_list.images) {
        ApInfo("video_editor") << fmt::format(
            "XPutImageInShm input image name: {}, time: {:20.8f}, width: {}, height: {}, dataSize: {}", img.frameID,
            app_common::DatetimeUtil::double_time_from_sec_nsec(img.timestamp), img.width, img.height, img.dataSize);
    }

    if (parame.editor_parameter.split_parames_.size() <= 0) {
        ApError("video_editor") << "XVideoEditor_Dvpp error "
                                   "parame.editor_parameter.split_parames_.size()  <= 0";
        auto ptr = CustomStack::Instance();
        EC400::Instance()->ec_add(EC400::SPLIT_INVALID, "XPutImageInShm");
        return false;
    } else {
        EC400::Instance()->ec_remove(EC400::SPLIT_INVALID);
    }

    const auto& video_list_ = video_list.images;
    if (video_list_.size() <= 0) {
        ApInfo("video_editor") << "XPutImageInShm error video_list_ size is 0, 相机数据无输入";
        EC400::Instance()->ec_add(EC400::DATA_EMPTY, "XPutImageInShm");
        auto ptr = CustomStack::Instance();
        return false;
    } else {
        EC400::Instance()->ec_remove(EC400::DATA_EMPTY);
    }

    //设置主相机
    setMaxAreaToMainCamera(video_list, parame.editor_parameter.split_parames_);

    //根据参数筛选需要绘制辅助线的数据
    ApInfo("video_editor") << "XPutImageInShm parame.editor_parameter.split_parames_.size(): "
                           << parame.editor_parameter.split_parames_.size();
    ara::adsfi::AppVideoSplitParam split_param = {};
    for (auto& item : parame.editor_parameter.split_parames_) {
        if (item.channel_name == "front" || item.channel_name == "back" || item.channel_name == "night") {
            int view_width = item.view_area_points.right_bottom_point_x - item.view_area_points.left_top_point_x;
            int view_height = item.view_area_points.right_bottom_point_y - item.view_area_points.left_top_point_y;
            if (view_width > 1920 / 2 && view_height >= 1080) {
                split_param = item;
                break;
            }
        }
    }

    if (split_param.channel_name == "") {
        EC400::Instance()->ec_add(EC400::SPLIT_INVALID, "XPutImageInShm");
        ApError("video_editor") << "XPutImageInShm error split_param.channel_name is empty";
        return false;
    } else {
        EC400::Instance()->ec_remove(EC400::SPLIT_INVALID);
    }

    // 判断是否需要走shm流程
    bool has_obstacles = parame.editor_parameter.objects.fusionOut.size() > 0;
    bool has_path = parame.editor_parameter.path.trajectoryPoints.size() > 0;
    bool has_remote = parame.editor_parameter.remote_control.trajectoryPoints.size() > 0;

    // 可通行区域：front相机且开关开启
    bool need_drivable_area = (parame.editor_parameter.show_drivable_area == 1) && (split_param.channel_name == "front");

    // 倒车预警：back相机（无论是否有目标都走shm流程，后续在obstacles中判断）
    bool is_back_camera = (split_param.channel_name == "back");

    // 只要有任一需求，就走shm流程
    if (!has_obstacles && !has_path && !has_remote && 
        !need_drivable_area && !is_back_camera) {
        ApInfo("video_editor") << "XPutImageInShm: No drawing requirements, skip shm process. "
                               << "has_obstacles=" << has_obstacles 
                               << ", has_path=" << has_path
                               << ", has_remote=" << has_remote
                               << ", need_drivable_area=" << need_drivable_area
                               << ", is_back_camera=" << is_back_camera;
        return false;
    }

    ApInfo("video_editor") << "XPutImageInShm split_param.channel_name:" << split_param.channel_name;
    if (split_param.channel_name == "front" || split_param.channel_name == "back" ||
        split_param.channel_name == "night") {
        auto it =
            std::find_if(video_list.images.begin(), video_list.images.end(),
                         [&](const ara::adsfi::MsgImageData& img) { return img.frameID == split_param.channel_name; });
        if (it == video_list.images.end()) {
            ApInfo("video_editor") << "XPutImageInShm error item.channel_name:" << split_param.channel_name
                                   << " not found in video_list";
            return false;
        }

        if (0 == it->dataSize || 0 == it->width || 0 == it->height || it->rawData.empty()) {
            ApInfo("video_editor") << "XPutImageInShm error, datasize: " << it->dataSize << ", width: " << it->width
                                   << ", height: " << it->height << ", channel_name: " << split_param.channel_name;
            EC400::Instance()->ec_add(EC400::DATA_INVALID);
            return false;
        } else {
            EC400::Instance()->ec_remove(EC400::DATA_INVALID);
        }

        image.seq = it->seq;
        image.timestamp = it->timestamp;
        image.frameID = "put_" + split_param.channel_name;

        image.height = it->height;
        image.width = it->width;
        image.imageType = it->imageType;
        image.dataSize = it->dataSize;

        auto shm_ptr = zero_shm_mem.GetAllocShmMem(
            image.frameID, VIDEO_OUPUT_SHM_MAX_WIDTH * VIDEO_OUPUT_SHM_MAX_HEIGHT * 3 / 2, image.seq);
        if (nullptr == shm_ptr) {
            EC400::Instance()->ec_add(EC400::SHM_FAILED, "XPutImageInShm");
            ApError("video_editor") << "XPutImageInShm error this is shm_ptr is nullther";
            return false;
        } else {
            ApInfo("video_editor") << "shm_ptr: " << (void*)shm_ptr;
            EC400::Instance()->ec_remove(EC400::SHM_FAILED);
        }

        memcpy(shm_ptr, it->rawData.data(), it->dataSize);
    }
    ApInfo("video_editor") << "XPutImageInShm end";

    return true;
}
