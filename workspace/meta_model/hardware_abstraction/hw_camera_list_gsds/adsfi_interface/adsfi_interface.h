
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
// #include "ara/adsfi/impl_type_idpsensorcompressedimagelist.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include <string>
#include <memory>
#include <iomanip>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"

#include "protocol/cameraplatform2usrprotocol.h"
#include "cidpabstractplatform.h"
#include "cidpabstractplatforminterface.h"
#include "MdcCameraListV2.hpp"
#include "ap_log/ap_log_interface.h"
#include <publisher.h>

class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() {

	}
	~AdsfiInterface() {

	}

	void Init();

	void DeInit();

	int Process(const std::string &topic,std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr);

	// int Process(const std::string &topic,std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr);

private:
	long long   seq = 0;
	long long   compressed_seq = 0;
    bool   is_init = false;
    int   debug = 0;
    int   n_out_1 = 0;
    int   hw_fps = 30;
    std::string   format = "YUYV";
    std::string   channelId = "0,1";
    std::string   width = "1920,1920";
    std::string   height = "1080,1080";
    std::string   camera_name = "front,back";
    int   mask_channel = 0;

    std::shared_ptr<avos::idpware::SCameraDataList>   camera_data_ptr = nullptr;
    std::shared_ptr<MdcCameraInterface>   mdc_camera_interface_ptr = nullptr;
    std::shared_ptr<avos::idpware::SCameraDataList>   compressed_camera_data_ptr = nullptr;

    mdc::visual::Image viz_h265_images[MAX_CAMERA_NUM];
    mdc::visual::Publisher viz_h265_images_pub[MAX_CAMERA_NUM];

    ThreadSafeBuffer<int> safebuf_flag_info_;
	std::shared_ptr<int> flag_ptr;

    std::mutex                                                    ts_mutex_;
    std::map<int, ara::adsfi::MsgHafTime>                         ts_map_; // <list_idx, MsgHafTime>

	BASE_TEMPLATE_FUNCION
};
#endif








