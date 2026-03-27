
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
#include "tcp11.hpp"
#include "shm_util/zero_copy_shm_mem.h"
#define SERVER_HOST "26.28.1.170"
#define SERVER_PORT 8886

// template <typename T>
// class SafeVector
// {
// 	using DataChangeFuncType = std::function<void(const T &)>;
// 	std::mutex mtx_;
// 	DataChangeFuncType on_push_ = nullptr;
// 	DataChangeFuncType on_delete_ = nullptr;
// 	std::vector<T> data_;

// public:
// 	SafeVector(DataChangeFuncType on_push = nullptr, DataChangeFuncType on_delete = nullptr) : on_push_(on_push), on_delete_(on_delete)
// 	{
// 	}
// 	void wait_data()
// 	{
// 		while (true)
// 		{
// 			if (data_.empty())
// 			{
// 				std::this_thread::sleep_for(std::chrono::milliseconds(1));
// 				continue;
// 			}
// 			break;
// 		}
// 	}

// 	SafeVector(SafeVector&& other) noexcept
// 		: mtx_()                                 // 重新构造新锁
// 		, on_push_(std::move(other.on_push_))
// 		, on_delete_(std::move(other.on_delete_))
// 		, data_(std::move(other.data_))
// 	{
// 	}

// 	void empty()
// 	{
// 		std::lock_guard<std::mutex> lock(mtx_);
// 		return data_.empty();
// 	}

// 	void push(const T &data)
// 	{
// 		std::lock_guard<std::mutex> lock(mtx_);
// 		if (on_push_)
// 		{
// 			on_push_(data);
// 		}
// 		data_.push_back(data);
// 	}

// 	void clear()
// 	{
// 		std::lock_guard<std::mutex> lock(mtx_);
// 		if (on_delete_)
// 		{
// 			for (size_t i = 0; i < data_.size(); i++)
// 			{
// 				on_delete_(data_[i]);
// 			}
// 		}
// 		data_.clear();
// 	}

// 	T get_last()
// 	{
// 		std::lock_guard<std::mutex> lock(mtx_);
// 		return data_.back();
// 	}

// 	~SafeVector()
// 	{
// 		clear();
// 	}
// };

class AdsfiInterface: public BaseAdsfiInterface
{
	// SafeVector<std::shared_ptr<ara::adsfi::MsgImageDataList>> camera_image_lists_;
public:
	AdsfiInterface() {

	}
	~AdsfiInterface() {

	}

	void Init() override;

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
	tcp11::Socket m_socket_client;
	bool b_use_tcp11 = false;
	private:
	unsigned int seq_{0};
	std::atomic<bool>                             is_imagelist_use;
    std::mutex                                    image_list_mtx_;
    std::condition_variable                       m_cond;
    std::shared_ptr<ara::adsfi::MsgImageDataList> image_list_ptr;
public:
	BASE_TEMPLATE_FUNCION
};
#endif








