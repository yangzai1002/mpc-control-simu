
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
// #include "ara/adsfi/impl_type_idpsensorlidarpointcloud.h"
// #include "ara/adsfi/impl_type_idpsensorimagelist.h"
#include "adsfi/impl_type_object2darray.h"
// #include "adsfi/impl_type_object3darray.h"
#include "ara/adsfi/impl_type_msgcameravpresult.h"
#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "adsfi_interface/src/mdc_vision_process.h"
// #include "ImageListDataRecvDvpp.hpp"
#include "image_data_interface/image_data_interface.hpp"
#include "adsfi_interface/src/protocol/imgdvppplatform2usrprotocol.h"
// #include "tcp11.hpp"
#include "shm_util/zero_copy_shm_mem.h"
#include "yolo_detection.h"
#include "SafeQueue.hpp"

#define SERVER_HOST "26.28.1.170"
#define SERVER_PORT 8886

class AdsfiInterface: public BaseAdsfiInterface
{
	// SafeVector<std::shared_ptr<ara::adsfi::MsgImageDataList>> camera_image_lists_;
public:
	AdsfiInterface() {

	}
	~AdsfiInterface();

	void Init() override;
	void DeInit() override;

	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageDataList> &image_list_ptr);

	void SetImageList(const std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr);

    void GetImageList(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr);

    void FillData(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr);

    void ReleaseData(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr);

	int Process(const std::string &name,std::shared_ptr<ara::adsfi::MsgCameraVPResult> &vpresults_ptr);
	
	void ConvertToMsgCameraVPResult(const PerceptionVPResult &out_results,ara::adsfi::MsgCameraVPResult &vpresults);
	// ThreadSafeQueue<std::shared_ptr<ara::adsfi::MsgImageDataList>> image_deque;

	avos::perception::mdc_vision_process handle;
	std::shared_ptr<pertarget::FaultDiagnosis> error_handle_ptr;
	    app_common::SafeQueue<ara::adsfi::MsgImageDataList>              que_image_list_for_put_image_in_shm_;

	// tcp11::Socket m_socket_client;
	bool b_use_tcp11 = false;

private:
	unsigned int seq_{0};
	std::atomic<bool>                             is_imagelist_use;
    std::mutex                                    image_list_mtx_;
    std::condition_variable                       m_cond;
    std::shared_ptr<ara::adsfi::MsgImageDataList> image_list_ptr;
	std::unique_ptr<YoloDetection> yolo_;
	int det_seq;
    std::atomic<bool> _stopped{false};
    std::thread thread_put_image_in_shm_;

public:
	BASE_TEMPLATE_FUNCION
};
#endif








