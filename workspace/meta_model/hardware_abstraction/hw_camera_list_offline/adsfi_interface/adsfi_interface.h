
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <iostream>
#include <memory>
#include <string>

#include "DatetimeUtil.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include "custom_stack.h"
#include "image_data_interface/image_data_interface.hpp"
#include "log/log.h"

#include "zero_copy_shm_mem.h"

std::vector<std::string> split(const std::string &s, char delimiter) {
	std::vector<std::string> tokens;
	std::string              token;
	std::stringstream        ss(s);
	while (std::getline(ss, token, delimiter)) {
		if (!token.empty())
			tokens.push_back(token); // 忽略空字符串
	}
	return tokens;
}
	void printCurrentTimeMillis(int id, const char* file, int line, std::string context = "") 
	{
		auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
		pid_t threadId = syscall(SYS_gettid); // 获取当前线程的系统线程ID
		// 打印文件名、行号、线程ID和格式化的日期时间
		std::cout << "[" << file << ":" << line << ":" << threadId << ":"
				<< std::fixed << std::setprecision(6) << static_cast<double>(ns.count()) / 1e9f << "] " << context << std::endl;
	}

class MdcCameraListOfflineInterface
{
public:
	MdcCameraListOfflineInterface() {

	}


	void Init(const std::vector<std::string> &_camera_names, std::vector<int> _width_vec, std::vector<int> _height_vec) {
		if (_camera_names.size() == 0) {
			std::cout << "error _camera_names  is empty" << std::endl;
			return;
		}

		if (_width_vec.size() == 0) {
			std::cout << "error _width_vec  is empty" << std::endl;
			return;
		}

		if (_height_vec.size() == 0) {
			std::cout << "error _height_vec  is empty" << std::endl;
			return;
		}

		if (_camera_names.size() != _width_vec.size() || _camera_names.size() != _height_vec.size()) {
			std::cout << "error  size matching" << std::endl;
			return;
		}
		camera_names = _camera_names;
		width_vec = _width_vec;
		height_vec = _height_vec;

		// open

		InitBuffer();

	}
	~MdcCameraListOfflineInterface() {

	}

	int RegisterCameraName(const std::string &name) {
		for (int i = 0; i < camera_names.size() ; i++) {
			if (camera_names[i] == name) {
				camera_index_map[name] = i;
				return 0;
			}
		}
		return -1;
	}

	// 函数：将 YUV 数据追加到文件中
	void appendYUVToFile(void* yuvDataAddress, int yuvDataSize, const std::string& fileName) {
		// 打开文件，以二进制追加模式
		std::ofstream outFile(fileName, std::ios::binary | std::ios::app);
		if (!outFile) {
			std::cerr << "无法打开文件用于追加：" << fileName << std::endl;
			return;
		}

		// 将数据追加到文件
		outFile.write(static_cast<char*>(yuvDataAddress), yuvDataSize);

		// 关闭文件
		outFile.close();
	}
    /*
	int DoConvert(const std::string &name, const unsigned char *h264_data, const int &data_len,   ShmBufferInfo &shm_buffer) {
		if (isCameraNameReady(name) < 0) {
			std::cout << "cannot find camera name " << name << std::endl;
			return -1;
		}

		int index = camera_index_map[name];
		unsigned char * shm_buffer_ptr = GetNextCamera(index);

		// invoke
		dvpp_io_data.m_indta.m_ptr = (u_int8_t*)h264_data;
		dvpp_io_data.m_indta.m_ptr_len = data_len;
		dvpp_io_data.m_outdata.m_ptr = shm_buffer_ptr;
		dvpp_io_data.m_outdata.m_ptr_len = GetSize(index);

		std::cout << "index:" << index << ",data_len:" << dvpp_io_data.m_indta.m_ptr_len << "," << dvpp_io_data.m_outdata.m_ptr_len << std::endl;
		if(camera_310p_ptr_vec[index]->ioctl(avos::idpware::imgdvpp::IOCTL_VDEC_IOIMG,
		                                  (void *) &dvpp_io_data) == 0)
		{
			// if(index == 0)
			{
				std::string file_name_422 = "/opt/usr/0.yuv422";
				// appendYUVToFile(shm_buffer_ptr, dvpp_io_data.m_outdata.m_ptr_len, file_name_422);
	            for (size_t j = 0; j < 128; j++) {
					printf("%02x ", ((u_int8_t*)shm_buffer_ptr)[j]);
				}
				printf("\n");
			}
			
		}
		else
		{
			std::cout << "get error index = " << index << std::endl;
		}

		shm_buffer.shm_key = GetKey(camera_names[index]);
		shm_buffer.buffer_size = GetBufferSize(index);
		shm_buffer.current_index = index_vec[index];
		shm_buffer.queue_len = 10;

		return 0;
	}
    */
    int DoConvert(const std::string &name, const unsigned char *h264_data, const int &data_len,   
                    std::string &shm_buffer_key, uint32_t &shm_buffer_index) {

		printCurrentTimeMillis(0,__FILE__, __LINE__);
		if (isCameraNameReady(name) < 0) {
			std::cout << "cannot find camera name " << name << std::endl;
			return -1;
		}

		int index = camera_index_map[name];
		unsigned char * shm_buffer_ptr = GetNextCamera(index);

		// invoke
		dvpp_io_data.m_indta.m_ptr = (u_int8_t*)h264_data;
		dvpp_io_data.m_indta.m_ptr_len = data_len;
		dvpp_io_data.m_outdata.m_ptr = shm_buffer_ptr;
		dvpp_io_data.m_outdata.m_ptr_len = GetSize(index);
		printCurrentTimeMillis(0,__FILE__, __LINE__);
		std::cout << "index:" << index << ",data_len:" << dvpp_io_data.m_indta.m_ptr_len << "," << dvpp_io_data.m_outdata.m_ptr_len << std::endl;
		if(camera_310p_ptr_vec[index]->ioctl(
											5,
											//avos::idpware::imgdvpp2::IOCTL_VDEC_IOIMG,
		                                  (void *) &dvpp_io_data) == 0)
		{
			// if(index == 0)
			{
				std::string file_name_422 = "/opt/usr/0.yuv422";
				// appendYUVToFile(shm_buffer_ptr, dvpp_io_data.m_outdata.m_ptr_len, file_name_422);
	            for (size_t j = 0; j < 128; j++) {
					printf("%02x ", ((u_int8_t*)shm_buffer_ptr)[j]);
				}
				printf("\n");
			}
			
		}
		else
		{
			std::cout << "get error index = " << index << std::endl;
		}
		printCurrentTimeMillis(0,__FILE__, __LINE__);
		shm_buffer_key = GetKey(camera_names[index]);
		shm_buffer_index = index_vec[index];
		printCurrentTimeMillis(0,__FILE__, __LINE__);
		return 0;
	}

	int GetImageInfo(const std::string &cam_name, uint32_t &width, uint32_t &height) {
		if (isCameraNameReady(cam_name) < 0) {
			std::cout << "cannot find camera cam_name " << cam_name << std::endl;
			return -1;
		}
		int index = camera_index_map[cam_name];
		width = width_vec[index];
		height = height_vec[index];
		return 0;
	}






private:

	int GetSize(int index) {
		return width_vec[index] * height_vec[index] * 1.5;
	}

	int isCameraNameReady(const std::string &cam_name) {
		auto it = camera_index_map.find(cam_name);
		if (it == camera_index_map.end()) {
			return -1;
		}
		return 0;
	}
	unsigned char * GetNextCamera(int i) {
		index_vec[i]++;
		return zero_shm_mem.GetAllocShmMem(GetKey(camera_names[i]), GetBufferSize(i), index_vec[i]);
	}

	void InitBuffer() {
		index_vec.resize(camera_names.size());
		camera_310p_ptr_vec.resize(camera_names.size());
		for (int i = 0; i < camera_names.size(); i++) {
			index_vec[i] = 0;
			camera_310p_ptr_vec[i] = std::make_shared<avos::idpware::CIdpAbstractPlatformInterface>();

			AINFO << std::to_string(i)<< "=========================================== so_path: " << so_path << " ===========================================";

			camera_310p_ptr_vec[i]->open(so_path.c_str(), RTLD_LAZY);


			init_cfg.m_inputtype = avos::idpware::imgdvpp::IMGTYPE_IN_H265;
			init_cfg.m_outputtype = avos::idpware::imgdvpp::IMGTYPE_OUT_YUV420NV12;
			init_cfg.m_functype = avos::idpware::imgdvpp::IOCTL_VDEC_DVPP;
			init_cfg.m_inwidth = width_vec[i];
			init_cfg.m_inheight = height_vec[i];
			init_cfg.m_outwidth = width_vec[i];
			init_cfg.m_outheight = height_vec[i];

			std::cout << "init :" << camera_names[i] << " m_inwidth=" << init_cfg.m_inwidth 
						<< ",m_inheight=" << init_cfg.m_inheight 
						<< ",m_inputtype=" << init_cfg.m_inputtype 
						<< ",m_outputtype=" << init_cfg.m_outputtype 
						<< ",m_functype=" << init_cfg.m_functype 
						<< std::endl;
			camera_310p_ptr_vec[i]->ioctl(avos::idpware::imgdvpp::IOCTL_INIT_DVPP,
			                              (void *) &init_cfg);
			zero_shm_mem.GetAllocShmMem(GetKey(camera_names[i]), GetBufferSize(i), index_vec[i]);
		}
	}


	std::string GetKey(const std::string &camera_name) {
		return std::string("prefix_camera_name") + camera_name ;
	}

	int GetBufferSize(int index) {
		return width_vec[index] * height_vec[index] * 1.5;
	}

private:
	ZeroCopyShmMem                         zero_shm_mem;
public:
	std::string so_path;
	std::vector<std::string> camera_names;
	std::vector<int> width_vec;
	std::vector<int> height_vec;
	std::vector<int> index_vec;

	std::vector<std::shared_ptr<avos::idpware::CIdpAbstractPlatformInterface>> camera_310p_ptr_vec;

	std::map<std::string, int> camera_index_map;

	avos::idpware::imgdvpp::SImgDvppCfgV2  init_cfg;
	avos::idpware::imgdvpp::SImgDvppIOData dvpp_io_data;
};


class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() {

        is_imagelist_use.store(false);
    }
    ~AdsfiInterface() {
    }

    void Init() override {
        SetScheduleInfo("message");
        // avos::common::AvosNode::Init("camera_stitch", true, "/opt/usr/zxz/log/", "/opt/usr/zxz/log/",
        //                              "/opt/usr/zxz/log/");
        std::cout << "image offline init" << std::endl;

        auto ptr = CustomStack::Instance();

        std::string so_file_name = "libimgdvpp_stitch_plugin_v2.so";
        ptr->GetConfig("HwCameraListOffLine", "format", format);
        ptr->GetConfig("HwCameraListOffLine", "width", width);
        ptr->GetConfig("HwCameraListOffLine", "height", height);
        ptr->GetConfig("HwCameraListOffLine", "camera_name", camera_name);
        ptr->GetConfig("HwCameraListOffLine", "debug", debug);
        std::string so_name = "";

		ptr->GetConfig("HwCameraListOffLine", "so_name", so_name);

		std::cout << "================== debug is " << debug << " ==================" << std::endl;
		std::cout << "================== format is " << format << " ==================" << std::endl;
		std::cout << "[HZ] init START" << std::endl;
		std::cout << "width str is " << width << std::endl;
		std::cout << "height str is " << height << std::endl;
		std::cout << "camera_name str is " << camera_name << std::endl;

		std::vector<std::string> camera_name_str_v = split(camera_name, ',');

		std::vector<std::string> width_str_v = split(width, ',');
		std::vector<int>         width_int_v;
		for (const auto &token : width_str_v) {
			width_int_v.push_back(atoi(token.c_str()));
		}

		std::vector<std::string> height_str_v = split(height, ',');
		std::vector<int>         height_int_v;
		for (const auto &token : height_str_v) {
			height_int_v.push_back(atoi(token.c_str()));
		}


		mdc_cameralist_interface_ptr = std::make_shared<MdcCameraListOfflineInterface>();



		std::string project_config_path = ptr->GetNodeConfigPath();


		printf("================== so_name is %s  ==================\n", so_name.c_str());
		mdc_cameralist_interface_ptr->so_path = project_config_path + "/" + so_name;
		
		AINFO << "=========================================== so_path: " << mdc_cameralist_interface_ptr->so_path << " ===========================================";

		mdc_cameralist_interface_ptr->Init(camera_name_str_v, width_int_v, height_int_v);

		std::cout << "[HZ] init ALL DONE " << std::endl;
        AINFO << "load so over";
    }

    void DeInit() override {
        std::cout << "image stitch  deinit" << std::endl;
    }

    void Callback(const std::string name, const std::shared_ptr<ara::adsfi::MsgImageDataList> &image_list_ptr) {
        AINFO << "image_list: " << std::setprecision(20)
              << app_common::DatetimeUtil::double_time_from_sec_nsec(image_list_ptr->header.timestamp);
        // for (const auto &img : image_list_ptr->images) {
        //     AINFO << "image, timestamp: " << std::setprecision(20)
        //           << app_common::DatetimeUtil::double_time_from_sec_nsec(img.timestamp) << ", frameID: " << img.frameID
        //           << ", width: " << img.width << ", height: " << img.height << ", datasize: " << img.dataSize
        //           << ", bufftype: " << static_cast<std::int32_t>(img.bufferType)
        //           << ", imagetype: " << static_cast<std::int32_t>(img.imageType)
        //           << ", rawdata.size(): " << img.rawData.size();
        // }
        SetImageList(image_list_ptr);



    }

    int Process(const std::string &topic, std::shared_ptr<ara::adsfi::MsgImageDataList> &image_list) {

		printCurrentTimeMillis(0,__FILE__, __LINE__);
        std::shared_ptr<ara::adsfi::MsgImageDataList> compressed_image_list;
        GetImageList(compressed_image_list);
		
		std::cout << "compressed_image_list->images.size()=" << compressed_image_list->images.size() << std::endl; 

        for (const auto p : compressed_image_list->images) {
			std::cout << p.frameID << "\n";
			mdc_cameralist_interface_ptr->RegisterCameraName(p.frameID);
		}

    	image_list->header = compressed_image_list->header;
        image_list->images.resize(compressed_image_list->images.size());

        for (int i = 0; i <  compressed_image_list->images.size(); i++) {
			// if(i!=0)
			// 	continue;

            image_list->images[i].timestamp = compressed_image_list->images[i].timestamp;

            mdc_cameralist_interface_ptr->DoConvert(compressed_image_list->images[i].frameID,
                                                    &(compressed_image_list->images[i].rawData[0]),
                                                    compressed_image_list->images[i].rawData.size(),
                                                    image_list->images[i].frameID,
                                                    image_list->images[i].seq
                                                );
            image_list->images[i].frameID = compressed_image_list->images[i].frameID;
            mdc_cameralist_interface_ptr->GetImageInfo(compressed_image_list->images[i].frameID,
                    image_list->images[i].width, image_list->images[i].height);
            image_list->images[i].imageType = 2;
            //image_list->images[i].step = 1.5 * image_list->images[i].width;
        }
		printCurrentTimeMillis(0,__FILE__, __LINE__);
        return 0;
    }


    void SetImageList(const std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
        // if (is_imagelist_use.load() == true) {
        //     return;
        // }
        {
            std::lock_guard<std::mutex> lock(image_list_mtx_);
            image_list_ptr = ptr;
            m_cond.notify_one();
        }
    }

    void GetImageList(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
        std::unique_lock<std::mutex> lock(image_list_mtx_);
        m_cond.wait(lock);
        ptr = image_list_ptr;
        FillData(ptr);
    }

    void FillData(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
        // dvpp::sync(*ptr);
        // is_imagelist_use.store(true);
    }

    void ReleaseData(std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
        // safe_release(*ptr);
        // is_imagelist_use.store(false);
    }

private:
    std::atomic<bool>                             is_imagelist_use;
    std::mutex                                    image_list_mtx_;
    std::condition_variable                       m_cond;
    std::shared_ptr<ara::adsfi::MsgImageDataList> image_list_ptr;

    std::string format;
    std::string width;
    std::string height;
    std::string camera_name;

    int debug = 0;

    std::shared_ptr<MdcCameraListOfflineInterface> mdc_cameralist_interface_ptr;


public:
    BASE_TEMPLATE_FUNCION
};
#endif
