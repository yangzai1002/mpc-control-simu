
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

	// int Process(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr);

	int InitCameraData(const std::vector<int> &channel_id, const std::vector<std::string> &camera_name,
             const std::vector<int> &width, const std::vector<int> &height, int freq, int main_idx); 

    std::shared_ptr<avos::idpware::CIdpAbstractPlatformInterface> mdc_camera_ptr;
    std::shared_ptr<ara::adsfi::MsgImageDataList> adsfi_camera_ptr;
	std::shared_ptr<ara::adsfi::MsgImageDataList> adsfi_compressed_camera_ptr;
    avos::idpware::SCameraDataList src_data_list;
    avos::idpware::SCameraDataList h264_data_list;

    mdc::visual::Image viz_h265_images[MAX_CAMERA_NUM];
    mdc::visual::Publisher viz_h265_images_pub[MAX_CAMERA_NUM];
 	long long m_seq = 0;
	int m_debug = 0;
	int m_n_out_1 = 0;
	int m_hw_fps = 30;
	std::string m_format = "YUYV";
	std::string m_channelId = "0,1";
	std::string m_width = "1920,1920";
	std::string m_height = "1080,1080";
	std::string m_camera_name = "front,back";
	int m_mask_channel = 0;

	int m_main_idx = 0;
	int m_nv_cam_pos = 0;
	int m_node_name = 0;
	int m_cam_num = 0;

	int m_channel_en_mask = 0;

	std::string m_so_name = "";
	std::vector<int>         channelId_int_v;
	BASE_TEMPLATE_FUNCION
};
#endif








