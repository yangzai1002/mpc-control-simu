
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include "ara/adsfi/impl_type_idpsensorimage.h"
#include "ara/adsfi/impl_type_idpsensorimagelist.h"
#include "ara/adsfi/impl_type_idpappvideoparameter.h"
#include "ara/adsfi/impl_type_idpvideoencodingperf.h"
#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include <thread>
#include <stdio.h>
#include <atomic>
#include <deque>
#include <functional>
#include "ImageListDataRecvDvpp.hpp"
#include "XVideoEditor_Dvpp.h"

template <typename T>
class SafeVector
{
	using DataChangeFuncType = std::function<void(const T &)>;
	std::mutex mtx_;
	DataChangeFuncType on_push_ = nullptr;
	DataChangeFuncType on_delete_ = nullptr;
	std::vector<T> data_;

public:
	SafeVector(DataChangeFuncType on_push = nullptr, DataChangeFuncType on_delete = nullptr) : on_push_(on_push), on_delete_(on_delete)
	{
	}
	void wait_data()
	{
		while (true)
		{
			if (data_.empty())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			break;
		}
	}

	SafeVector(SafeVector&& other) noexcept
		: mtx_()                                 // 重新构造新锁
		, on_push_(std::move(other.on_push_))
		, on_delete_(std::move(other.on_delete_))
		, data_(std::move(other.data_))
	{
	}

	void empty()
	{
		std::lock_guard<std::mutex> lock(mtx_);
		return data_.empty();
	}

	void push(T &data)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		if (on_push_)
		{
			on_push_(data);
		}
		data_.push_back(data);
	}

	void clear()
	{
		std::lock_guard<std::mutex> lock(mtx_);
		if (on_delete_)
		{
			for (size_t i = 0; i < data_.size(); i++)
			{
				on_delete_(data_[i]);
			}
		}
		data_.clear();
	}

	T get_last()
	{
		std::lock_guard<std::mutex> lock(mtx_);
		return data_.back();
	}

	~SafeVector()
	{
		clear();
	}
};

class AdsfiInterface : public BaseAdsfiInterface
{
	SafeVector<std::shared_ptr<ara::adsfi::IdpSensorImageList>> camera_image_lists_;
	SafeVector<std::shared_ptr<ara::adsfi::IdpSensorImage>> stiched_images_;
	SafeVector<std::shared_ptr<ara::adsfi::IdpAppVideoParameter>> params_;

public:
AdsfiInterface()
    : camera_image_lists_(
        [](const std::shared_ptr<ara::adsfi::IdpSensorImageList>& image_list)
        { dvpp::sync(*image_list); },
        [](const std::shared_ptr<ara::adsfi::IdpSensorImageList>& image_list)
        { safe_release(*image_list); })
{}

	~AdsfiInterface()
	{
	}

	void Init() override
	{
		SetScheduleInfo("message", 100);
		std::cout << "init" << std::endl;
	}

	void DeInit() override
	{
		std::cout << "deinit" << std::endl;
	}

	void Callback(std::shared_ptr<ara::adsfi::IdpSensorImageList> &image_list)
	{
		camera_image_lists_.push(image_list);
	}

	void Callback(std::shared_ptr<ara::adsfi::IdpAppVideoParameter> &param)
	{

		params_.push(param);
	}

	int Process(std::shared_ptr<ara::adsfi::IdpSensorImage> &image)
	{
		// for test
		auto param = std::make_shared<ara::adsfi::IdpAppVideoParameter>();
		ara::adsfi::IdpAppVideoSplitParam split_param;
		split_param.channel_name = "front_middle";
		split_param.view_area_points.left_top_point_x = 0;
		split_param.view_area_points.left_top_point_y = 0;
		split_param.view_area_points.right_bottom_point_x = 1920;
		split_param.view_area_points.right_bottom_point_y = 1080;
		split_param.crop_area_points.left_top_point_x = 0;
		split_param.crop_area_points.left_top_point_y = 0;
		split_param.crop_area_points.right_bottom_point_x = 1920;
		split_param.crop_area_points.right_bottom_point_y = 1080;
		param->editor_parameter.split_parames_.push_back(split_param);

		
		// ara::adsfi::IdpAppVideoSplitParam split_param_left;
		// split_param_left.channel_name = "left_front_around";
		// split_param_left.view_area_points.left_top_point_x = 0;
		// split_param_left.view_area_points.left_top_point_y = 0;
		// split_param_left.view_area_points.right_bottom_point_x = 640;
		// split_param_left.view_area_points.right_bottom_point_y = 360;
		// split_param_left.crop_area_points.left_top_point_x = 0;
		// split_param_left.crop_area_points.left_top_point_y = 0;
		// split_param_left.crop_area_points.right_bottom_point_x = 640;
		// split_param_left.crop_area_points.right_bottom_point_y = 360;
		// param->editor_parameter.split_parames_.push_back(split_param_left);

		ara::adsfi::IdpAppVideoSplitParam split_param_right;
		split_param_right.channel_name = "right_front_around";
		split_param_right.view_area_points.left_top_point_x =1220;
		split_param_right.view_area_points.left_top_point_y = 0;
		split_param_right.view_area_points.right_bottom_point_x = 1920;
		split_param_right.view_area_points.right_bottom_point_y = 700;
		split_param_right.crop_area_points.left_top_point_x = 610;
		split_param_right.crop_area_points.left_top_point_y = 190;
		split_param_right.crop_area_points.right_bottom_point_x = 1310;
		split_param_right.crop_area_points.right_bottom_point_y = 890;
		param->editor_parameter.split_parames_.push_back(split_param_right);


		param->encoding_parameter.adaptive_mode = 0;
		param->encoding_parameter.rate_control_mode = 0;
		param->encoding_parameter.encoding_gop = 30;
		param->encoding_parameter.quality = 30;
		param->encoding_parameter.bps = 1024;
		param->encoding_parameter.fps = 30;
		param->encoding_parameter.width = 1280;
		param->encoding_parameter.height = 720;
		params_.push(param);

		/// ...
		camera_image_lists_.wait_data();
		auto image_list = camera_image_lists_.get_last();
		params_.wait_data();
		auto param_in = params_.get_last();
		// do something
		ara::adsfi::IdpSensorImage nosence;
		XVideoEditor_Dvpp(*image_list.get(), nosence, *param_in.get(), *image.get());
		// must
		camera_image_lists_.clear();
		params_.clear();
		return 0;
	}

	int Process(std::shared_ptr<ara::adsfi::IdpVideoEncodingPerf> &ptr)
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("IdpVideoEncodingPerf not implement\n");
		}
		return 0;
	}

	BASE_TEMPLATE_FUNCION
};
#endif
