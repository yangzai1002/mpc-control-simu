#include "DatetimeUtil.hpp"
#include "EC400.hpp"
#include "StringUtils.hpp"
#include "VideoParameter.h"
#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include "geometry_tools.hpp"
#include "image_data_interface/image_data_interface.hpp"

AdsfiInterface::AdsfiInterface() {
    // input state
    dl_vision_result_ = std::make_shared<ara::adsfi::MsgCameraVPResult>();
    sematic_map_ = std::make_shared<ara::adsfi::PerceptionStaticEnv>();
    video_profile_ = std::make_shared<ara::adsfi::AppVideoProfile>();
    vehicle_information_ = std::make_shared<ara::adsfi::VehicleInformation>();
    location_fusion_ = std::make_shared<ara::adsfi::MsgHafLocation>();
    path_auto_ = std::make_shared<ara::adsfi::MsgHafEgoTrajectory>();
    path_rc_ = std::make_shared<ara::adsfi::MsgHafEgoTrajectory>();
    business_command_ = std::make_shared<ara::adsfi::BusinessCommand>();
    planing_status_ = std::make_shared<ara::adsfi::PlanningStatus>();
    control_ = std::make_shared<ara::adsfi::AppRemoteControl>();

    // runtime state
    obstacles_ = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
    video_parameter_ = std::make_shared<compatible::VideoParameter>();

    // 读取频率阈值配置
    std::string freq_path = "video/edit/freq_threshold/image_list";
    if (!CustomStack::Instance()->GetProjectConfigValue(freq_path, freq_threshold_)) {
        EC400::Instance()->ec_add(EC400::CONFIG, freq_path);
        auto error = fmt::format("read project config {} failed", freq_path);
        ApInfo("video_editor") << error;
        throw std::runtime_error(error);
    }

    // put image in shm thread --> draw assist line thread --> draw obstacles --> layout thread
    thread_put_image_in_shm_ = std::thread([this]() {
        pthread_setname_np(pthread_self(), "put_image_in_shm");
        compatible::VideoParameter video_parameter;
        while (!_stopped.load()) {
            ara::adsfi::MsgImageDataList image_list;
            if (!que_image_list_for_put_image_in_shm_.timeout_pop_latest(image_list, 10000)) {
                continue;
            }
            ApInfo("video_editor") << fmt::format(
                "put image in shm get image list, size: {}, time: {:20.8f}", image_list.images.size(),
                app_common::DatetimeUtil::double_time_from_sec_nsec(image_list.header.timestamp));
            {
                std::lock_guard<std::mutex> lg(runtime_mtx_);
                video_parameter = *video_parameter_;
            }

            extern bool XPutImageInShm(const ara::adsfi::MsgImageDataList &video_list, // 视频组
                                       const compatible::VideoParameter &  parame,     // 视频绘制参数
                                       ara::adsfi::MsgImageData &          image);

            dvpp::sync(image_list);
            ara::adsfi::MsgImageData image;
            try { // 现阶段不知道内部是否会抛异常，先try-catch包裹一下
                if (!XPutImageInShm(image_list, video_parameter, image)) {
                    ApError("video_editor") << "XPutImageInShm error, put image in shm failed";
                } else {
                    que_image_in_shm_.push(std::make_shared<ara::adsfi::MsgImageData>(image));
                    ApInfo("video_editor") << "queue size of que_image_in_shm_: " << que_image_in_shm_.size();
                }
            } catch (const std::exception &e) {
                ApError("video_editor") << "XPutImageInShm exception: " << e.what();
            } catch (...) {
                ApError("video_editor") << "XPutImageInShm unknown exception";
            }
            safe_release(image_list);
        }
    });

    thread_draw_assist_lines_ = std::thread([this]() {
        pthread_setname_np(pthread_self(), "draw_assist_lines");
        std::shared_ptr<ara::adsfi::MsgImageData> image;
        compatible::VideoParameter                video_parameter;
        while (!_stopped.load()) {
            if (!que_image_in_shm_.timeout_pop_latest(image, 10000)) {
                continue;
            }
            ApInfo("video_editor") << fmt::format(
                "draw assist line get image from shm, name: {}, time: {:20.8f}", image->frameID,
                app_common::DatetimeUtil::double_time_from_sec_nsec(image->timestamp));
            {
                std::lock_guard<std::mutex> lg(runtime_mtx_);
                video_parameter = *video_parameter_;
            }

            extern bool XDrawAssistLines(const ara::adsfi::MsgImageData &  image_in, // 绘制图像数据
                                         const compatible::VideoParameter &parame,   // 视频绘制参数
                                         ara::adsfi::MsgImageData &        image_out);

            ara::adsfi::MsgImageData image_with_assist_line;
            try { // 现阶段不知道内部是否会抛异常，先try-catch包裹一下
                if (!XDrawAssistLines(*image, video_parameter, image_with_assist_line)) {
                    ApError("video_editor") << "XDrawAssistLines error, draw assist lines failed";
                } else {
                    que_image_drew_assist_lines_.push(
                        std::make_shared<ara::adsfi::MsgImageData>(image_with_assist_line));
                    ApInfo("video_editor")
                        << "queue size of que_image_drew_assist_lines_: " << que_image_drew_assist_lines_.size();
                }
            } catch (const std::exception &e) {
                ApError("video_editor") << "XDrawAssistLines exception: " << e.what();
            } catch (...) {
                ApError("video_editor") << "XDrawAssistLines unknown exception";
            }
        }
    });

    thread_draw_obstacles_ = std::thread([this]() {
        pthread_setname_np(pthread_self(), "draw_obstacles");
        std::shared_ptr<ara::adsfi::MsgImageData> image;
        compatible::VideoParameter                video_parameter;
        while (!_stopped.load()) {
            if (!que_image_drew_assist_lines_.timeout_pop_latest(image, 10000)) {
                continue;
            }

            ApInfo("video_editor") << fmt::format(
                "draw obstacles get image with assist line, name: {}, time: {:20.8f}", image->frameID,
                app_common::DatetimeUtil::double_time_from_sec_nsec(image->timestamp));

            {
                std::lock_guard<std::mutex> lg(runtime_mtx_);
                video_parameter = *video_parameter_;
            }

            ApInfo("video_editor") << "obstacles size: " << video_parameter.editor_parameter.objects.fusionOut.size();

            extern bool XDrawObstacles(const ara::adsfi::MsgImageData &  image_in, // 绘制图像数据
                                       const compatible::VideoParameter &parame,   // 视频绘制参数
                                       ara::adsfi::MsgImageData &        image_out);

            ara::adsfi::MsgImageData image_with_obstacles;
            try { // 现阶段不知道内部是否会抛异常，先try-catch包裹一下
                if (!XDrawObstacles(*image, video_parameter, image_with_obstacles)) {
                    ApError("video_editor") << "XDrawObstacles error, draw obstacles failed";
                } else {
                    que_image_drew_obstacles_.push(std::make_shared<ara::adsfi::MsgImageData>(image_with_obstacles));
                    ApInfo("video_editor")
                        << "queue size of que_image_drew_obstacles_: " << que_image_drew_obstacles_.size();
                }
            } catch (const std::exception &e) {
                ApError("video_editor") << "XDrawObstacles exception: " << e.what();
            } catch (...) {
                ApError("video_editor") << "XDrawObstacles unknown exception";
            }
        }
    });

    thread_draw_drivable_area_ = std::thread([this]() {
        pthread_setname_np(pthread_self(), "draw_drivable_area");
        std::shared_ptr<ara::adsfi::MsgImageData>      image;
        compatible::VideoParameter                video_parameter;
        std::shared_ptr<ara::adsfi::MsgCameraVPResult> vp_result{};
        
        while (!_stopped.load()) {
            if (!que_image_drew_obstacles_.timeout_pop_latest(image, 10000)) {
                continue;
            }

            ApInfo("video_editor") << fmt::format(
                "draw drivable area get image with obstacles, name: {}, time: {:20.8f}", image->frameID,
                app_common::DatetimeUtil::double_time_from_sec_nsec(image->timestamp));

            {
                std::lock_guard<std::mutex> lg(runtime_mtx_);
                video_parameter = *video_parameter_;
            }
            
            {
                std::lock_guard<std::mutex> lg(visbox_mtx_);
                vp_result = dl_vision_result_;
            }

            extern bool XDrawDrivableArea(const ara::adsfi::MsgImageData &     image_data,
                                          const ara::adsfi::MsgCameraVPResult &vp_result,
                                          const compatible::VideoParameter &   video_parameter);

            ara::adsfi::MsgImageData image_with_drivable_area = *image;
            try {
                if (!XDrawDrivableArea(image_with_drivable_area, *vp_result, video_parameter)) {
                    ApError("video_editor") << "XDrawDrivableArea error, draw drivable area failed";
                } else {
                    que_image_drew_drivable_area_.push(
                        std::make_shared<ara::adsfi::MsgImageData>(image_with_drivable_area));
                    ApInfo("video_editor")
                        << "queue size of que_image_drew_drivable_area_: " << que_image_drew_drivable_area_.size();
                }
            } catch (const std::exception &e) {
                ApError("video_editor") << "XDrawDrivableArea exception: " << e.what();
            } catch (...) {
                ApError("video_editor") << "XDrawDrivableArea unknown exception";
            }
        }
    });

    thread_layout_ = std::thread([this]() {
        pthread_setname_np(pthread_self(), "layout");
        ara::adsfi::MsgImageData   stitch;
        ara::adsfi::MsgImageData   drew;
        compatible::VideoParameter video_parameter;
        while (!_stopped.load()) {
            ara::adsfi::MsgImageDataList image_list;
            if (!que_image_list_for_layout_.timeout_pop_latest(image_list, 10000)) {
                continue;
            }
            ApInfo("video_editor") << fmt::format(
                "layout get image list, size: {}, time: {:20.8f}", image_list.images.size(),
                app_common::DatetimeUtil::double_time_from_sec_nsec(image_list.header.timestamp));

            std::shared_ptr<ara::adsfi::MsgImageData> ptr_stitch;
            if (que_image_stitch_.pop_latest(ptr_stitch)) {
                stitch = *ptr_stitch;
            }

            std::shared_ptr<ara::adsfi::MsgImageData> ptr_drew;
            if (que_image_drew_drivable_area_.pop_latest(ptr_drew)) {
                drew = *ptr_drew;
            }

            {
                std::lock_guard<std::mutex> lg(runtime_mtx_);
                video_parameter = *video_parameter_;
            }

            extern bool XImageLayout(const ara::adsfi::MsgImageDataList &video_list, // 视频组
                                     const compatible::VideoParameter &  parame,     // 视频绘制参数
                                     const ara::adsfi::MsgImageData &    stitch,     // 环视拼接图像
                                     const ara::adsfi::MsgImageData &    image_in, // 前/后/前夜视/后夜视相机
                                     ara::adsfi::MsgImageData &          image_out);

            ara::adsfi::MsgImageData image_laied_out;
            dvpp::sync(image_list);
            try { // 现阶段不知道内部是否会抛异常，先try-catch包裹一下
                if (!XImageLayout(image_list, video_parameter, stitch, drew, image_laied_out)) {
                    ApError("video_editor") << "XLayout error, layout failed";
                } else {
                    que_image_laied_out_.push(std::make_shared<ara::adsfi::MsgImageData>(image_laied_out));
                    ApInfo("video_editor") << "queue size of que_image_laied_out_: " << que_image_laied_out_.size();
                }
            } catch (const std::exception &e) {
                ApError("video_editor") << "XLayout exception: " << e.what();
            } catch (...) {
                ApError("video_editor") << "XLayout unknown exception";
            }
            safe_release(image_list);
        }
    });
}

AdsfiInterface::~AdsfiInterface() {
    _stopped = true;

    if (thread_put_image_in_shm_.joinable()) {
        thread_put_image_in_shm_.join();
    }

    if (thread_draw_assist_lines_.joinable()) {
        thread_draw_assist_lines_.join();
    }

    if (thread_draw_obstacles_.joinable()) {
        thread_draw_obstacles_.join();
    }

    if (thread_draw_drivable_area_.joinable()) {
        thread_draw_drivable_area_.join();
    }

    if (thread_layout_.joinable()) {
        thread_layout_.join();
    }
}

void AdsfiInterface::Init() {
    SetScheduleInfo("message");
    // ApLogInterface::Instance()->Init("XvideoEditor");
}

// 视觉检测结果
void AdsfiInterface::Callback(const std::string &                                   name,
                              const std::shared_ptr<ara::adsfi::MsgCameraVPResult> &dl_vision_result) {
    if (__glibc_unlikely(nullptr == dl_vision_result)) {
        ApError("video_editor") << "dl_vision_result is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lg(visbox_mtx_);
    dl_vision_result_ = dl_vision_result;

    extern void XVisWariningBox(const ara::adsfi::MsgCameraVPResult &  dl_vision_result, // 视觉检测结果
                                const ara::adsfi::PerceptionStaticEnv &out_sematic_map,
                                ara::adsfi::MsgHafFusionOutArray &     tracking_result);

    auto obstacles = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
    XVisWariningBox(*dl_vision_result_, *sematic_map_, *obstacles);

    std::lock_guard lg1(runtime_mtx_);
    obstacles_ = obstacles;
}

void AdsfiInterface::Callback(const std::string &                                     name,
                              const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &out_sematic_map) {
    if (__glibc_unlikely(nullptr == out_sematic_map)) {
        ApError("video_editor") << "out_sematic_map is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lg(visbox_mtx_);
    sematic_map_ = out_sematic_map;
}

void AdsfiInterface::Callback(const std::string &                                 name,
                              const std::shared_ptr<ara::adsfi::AppVideoProfile> &video_profile) {
    if (__glibc_unlikely(nullptr == video_profile)) {
        ApError("video_editor") << "video_profile is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lg(parameter_mtx);
    video_profile_ = video_profile;
}

void AdsfiInterface::Callback(const std::string &                                    name,
                              const std::shared_ptr<ara::adsfi::VehicleInformation> &vehicle_information) {
    if (__glibc_unlikely(nullptr == vehicle_information)) {
        ApError("video_editor") << "vehicle_information is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lg(parameter_mtx);
    vehicle_information_ = vehicle_information;
}

void AdsfiInterface::Callback(const std::string &                                name,
                              const std::shared_ptr<ara::adsfi::MsgHafLocation> &location_fusion) {
    if (__glibc_unlikely(nullptr == location_fusion)) {
        ApError("video_editor") << "location_fusion is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lg(parameter_mtx);
    location_fusion_ = location_fusion;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &param) {
    if (__glibc_unlikely(nullptr == param)) {
        ApError("video_editor") << "path is nullptr";
        return;
    }

    std::lock_guard<std::mutex> lg(parameter_mtx);
    if ("tpremote_path" == name) {
        path_rc_ = param;
    } else if ("tppathplan_auto" == name) {
        path_auto_ = param;
    } else {
        ApInfo("video_editor") << "invalid Callback name:" << name;
        return;
    }
}

void AdsfiInterface::Callback(const std::string &                                 name,
                              const std::shared_ptr<ara::adsfi::BusinessCommand> &business_command) {
    if (__glibc_unlikely(nullptr == business_command)) {
        ApError("video_editor") << "business_command is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lg(parameter_mtx);
    business_command_ = business_command;

    extern void     XVideoSelector(const ara::adsfi::AppVideoProfile &     video_profile,
                                   const ara::adsfi::MsgHafLocation &      localition_fusion,   //位置信息
                                   const ara::adsfi::VehicleInformation &  vehicle_information, // 车辆状态
                                   const ara::adsfi::MsgHafFusionOutArray &objects,             // 目标识别结果
                                   const ara::adsfi::MsgHafEgoTrajectory & remote_control, // 遥控时的路径规划
                                   const ara::adsfi::MsgHafEgoTrajectory & path,           // 路径规划
                                   const ara::adsfi::PlanningStatus &      planning_status,
                                   const ara::adsfi::AppRemoteControl &    control,        // 遥控信息
                                   compatible::VideoParameter &            video_parameter // 视频参数
        );
    auto            video_parameter = std::make_shared<compatible::VideoParameter>();
    std::lock_guard lg1(runtime_mtx_);
    XVideoSelector(*video_profile_, *location_fusion_, *vehicle_information_, *obstacles_, *path_rc_, *path_auto_,
                   *planing_status_, *control_, *video_parameter);
    video_parameter_ = video_parameter;
}

void AdsfiInterface::Callback(const std::string &                                name,
                              const std::shared_ptr<ara::adsfi::PlanningStatus> &planning_status) {
    if (__glibc_unlikely(nullptr == planning_status)) {
        ApError("video_editor") << "planning_status is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lg(parameter_mtx);
    planing_status_ = planning_status;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppRemoteControl> &control) {
    if (__glibc_unlikely(nullptr == control)) {
        ApError("video_editor") << "control is nullptr";
        return;
    }
    std::lock_guard<std::mutex> lg(parameter_mtx);
    control_ = control;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageData> &image_stitich) {
    if (__glibc_unlikely(nullptr == image_stitich)) {
        ApError("video_editor") << "image_stitich is nullptr";
        return;
    }
    que_image_stitch_.push(image_stitich);
    ApInfo("video_editor") << "queue size of que_image_stitch_: " << que_image_stitch_.size();
}

void AdsfiInterface::Callback(const std::string &                                  name,
                              const std::shared_ptr<ara::adsfi::MsgImageDataList> &image_list) {
    if (__glibc_unlikely(nullptr == image_list)) {
        ApError("video_editor") << "image_list is nullptr";
        return;
    }

    // 检查数据接收频率（阈值从配置文件读取）
    double data_time = app_common::DatetimeUtil::double_time_from_sec_nsec(image_list->header.timestamp);
    EC400::Instance()->check_freq(data_time, freq_threshold_);

    // 检查图像编码格式
    std::string invalid_frame;
    for (const auto &img : image_list->images) {
        // imageType: 0=YUV420SP(NV12), 期望非负值；若未知格式则上报
        if (img.imageType < 0) {
            invalid_frame = img.frameID;
        }
    }
    if (!invalid_frame.empty()) {
        EC400::Instance()->ec_add(EC400::ENCODING_ERROR, invalid_frame);
    } else {
        EC400::Instance()->ec_remove(EC400::ENCODING_ERROR);
    }

    que_image_list_for_put_image_in_shm_.push(*image_list);
    ApInfo("video_editor") << "queue size of que_image_list_for_put_image_in_shm_: "
                           << que_image_list_for_put_image_in_shm_.size();
    que_image_list_for_layout_.push(*image_list);
    ApInfo("video_editor") << "queue size of que_image_list_for_layout_: " << que_image_list_for_layout_.size();
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgImageData> &image) {
    if (que_image_laied_out_.timeout_pop_latest(image, 10000)) {
        return 0;
    }

    return -1;
}
