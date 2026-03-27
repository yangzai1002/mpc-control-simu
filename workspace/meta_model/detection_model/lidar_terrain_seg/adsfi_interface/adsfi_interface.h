
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include "ara/adsfi/impl_type_msglidarframe.h"
#include "ara/adsfi/impl_type_sensorlidarpacket.h"
#include <string>
#include <memory>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"

#include <publisher.h>

#include "common/shm_util/zero_copy_shm_mem.h"
#include "image_data_interface/image_data_interface.hpp"

#include "SafeQueue.hpp"

#include "acl/acl.h"
#include "acl/acl_base.h"
#include "acl/dvpp/hi_media_common.h"
#include "acl/dvpp/hi_dvpp.h"
#include "acl/ops/acl_dvpp.h"

#include "terrain_seg.h"

#define VIDEO_IMAGE_MAX_WIDTH 1920
#define VIDEO_IMAGE_MAX_HEIGHT 1080

#define VIDEO_OUPUT_SHM_MAX_WIDTH 3840
#define VIDEO_OUPUT_SHM_MAX_HEIGHT 2160

#define MAX_UNCHANGED 90

class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() {
	}
	~AdsfiInterface() ;

	void Init() ;

	void Callback(const std::string &name, std::shared_ptr<ara::adsfi::MsgLidarFrame> &cloud);
    void Process(const std::string &name, std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ogm);
	// void Callback(const std::string &name,std::shared_ptr<ara::adsfi::MsgImageDataList> &image_list_ptr) ;
    void VisualizeThread();
private:
    std::thread thread_put_image_in_shm_;
    std::atomic<bool> _stopped{false};
	std::atomic<bool>                             is_imagelist_use;
    std::shared_ptr<ara::adsfi::MsgImageDataList> image_list_ptr;
    std::condition_variable                       m_cond;
    std::mutex                                    image_list_mtx_;
	
    app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>> que_image_stitch_;

    std::unique_ptr<TerrainSeg> terrain_segger;
    cv::Mat range_image, seg_image, ogm_image;
    std::vector<unsigned char> ogm_map;


private:

    mdc::visual::Publisher cloud_pub {};
    mdc::visual::Publisher imagePub{};

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_ground;
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_obstacle;

    hi_venc_chn vencChnId = 199;
	int ref_count = 0;


	std::condition_variable tcv;
	std::mutex mtx;
	std::atomic<bool> data_flag;
	std::unique_ptr<std::thread> visual_thread {nullptr};
    int32_t epollFd = 0;
    int32_t fd;
public:
	BASE_TEMPLATE_FUNCION
};
#endif








