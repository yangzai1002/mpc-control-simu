
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <stdio.h>

#include <atomic>
#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "SafeQueue.hpp"
#include "VideoParameter.h"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_appvideoparameter.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_msgcameravpresult.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_videoencodingperf.h"
#include "common/shm_util/zero_copy_shm_mem.h"
#include "image_data_interface/image_data_interface.hpp"

#define VIDEO_IMAGE_MAX_WIDTH 1920
#define VIDEO_IMAGE_MAX_HEIGHT 1080

#define VIDEO_OUPUT_SHM_MAX_WIDTH 3840
#define VIDEO_OUPUT_SHM_MAX_HEIGHT 2160

#define MAX_UNCHANGED 90

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    AdsfiInterface(const AdsfiInterface &) = delete;

    AdsfiInterface &operator=(const AdsfiInterface &) = delete;

    void Init() override;

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgCameraVPResult> &dl_vision_result);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &out_sematic_map);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppVideoProfile> &video_profile);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &vehicle_information);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &location_fusion);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &path);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &business_command);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PlanningStatus> &planing_status);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppRemoteControl> &control);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageData> &image_stitich);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageDataList> &image_list);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgImageData> &image);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::VideoEncodingPerf> &ptr);

private:
    // input
    std::mutex                                       visbox_mtx_;
    std::shared_ptr<ara::adsfi::MsgCameraVPResult>   dl_vision_result_;
    std::shared_ptr<ara::adsfi::PerceptionStaticEnv> sematic_map_;

    std::mutex                                       parameter_mtx;
    std::shared_ptr<ara::adsfi::AppVideoProfile>     video_profile_;
    std::shared_ptr<ara::adsfi::VehicleInformation>  vehicle_information_;
    std::shared_ptr<ara::adsfi::MsgHafLocation>      location_fusion_;
    std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> path_auto_;
    std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> path_rc_;
    std::shared_ptr<ara::adsfi::BusinessCommand>     business_command_;
    std::shared_ptr<ara::adsfi::PlanningStatus>      planing_status_;
    std::shared_ptr<ara::adsfi::AppRemoteControl>    control_;

    app_common::SafeQueue<ara::adsfi::MsgImageDataList>              que_image_list_for_put_image_in_shm_;
    app_common::SafeQueue<ara::adsfi::MsgImageDataList>              que_image_list_for_layout_;
    app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>> que_image_stitch_;

    // runtime state
    std::mutex                                        runtime_mtx_;
    std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> obstacles_;
    std::shared_ptr<compatible::VideoParameter>       video_parameter_;

private:
    std::atomic<bool> _stopped{false};

    double freq_threshold_{5.0};

    std::thread thread_put_image_in_shm_;
    std::thread thread_draw_assist_lines_;
    std::thread thread_draw_obstacles_;
    std::thread thread_layout_;
    // sync between threads
    app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>> que_image_in_shm_;
    app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>> que_image_drew_assist_lines_;
    app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>> que_image_drew_obstacles_;
    app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>> que_image_laied_out_;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
