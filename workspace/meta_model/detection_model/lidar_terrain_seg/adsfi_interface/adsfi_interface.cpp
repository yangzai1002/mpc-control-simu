#include "adsfi_interface.h"


void AdsfiInterface::Init(){
    SetScheduleInfo("message");
    bool result = mdc::visual::Connect();
	if (result) {
	  std::cout  << "[MVIZ INFO] lidar The MViz server address is parsed and the visualization client is created successfully.\n";
	} else {
	  std::cout << "[MVIZ INFO] Failed to parse the MViz server address, or a visualization client has been created.\n";
	}

	cloud_pub = mdc::visual::Publisher::Advertise<mdc::visual::PointCloud2>(ara::core::String("seg_pointcloud"));
    imagePub = mdc::visual::Publisher::Advertise<mdc::visual::Image>("terrain_seg_ogm");

    terrain_segger = std::make_unique<TerrainSeg>();
    terrain_segger->Init();

    pcl::PointCloud<pcl::PointXYZ>::Ptr temp_cloud1( new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr temp_cloud2( new pcl::PointCloud<pcl::PointXYZ>);
    cloud_obstacle = temp_cloud1;
    cloud_ground = temp_cloud2;

    range_image = cv::Mat::zeros(1280,720,CV_8UC3);
    seg_image = cv::Mat::zeros(1280,720,CV_8UC3);
    ogm_image = cv::Mat::zeros(1280,720,CV_8UC3);


    auto ret = hi_mpi_sys_init();
    std::cout << "hi_mpi_sys_init "<<ret << "\n";
    hi_venc_chn_attr attr{};
    attr.venc_attr.type = HI_PT_H265;
    attr.venc_attr.max_pic_width = 4096;
    attr.venc_attr.max_pic_height = 4096;
    attr.venc_attr.pic_width = 1280;
    attr.venc_attr.pic_height = 720;
    attr.venc_attr.buf_size = 2 * attr.venc_attr.max_pic_width * attr.venc_attr.max_pic_height;
    attr.venc_attr.is_by_frame = HI_TRUE;
    attr.rc_attr.rc_mode = HI_VENC_RC_MODE_H265_VBR;
    attr.rc_attr.h265_vbr.gop = 30;
    attr.rc_attr.h265_vbr.stats_time = 1;
    attr.rc_attr.h265_vbr.src_frame_rate = 30;
    attr.rc_attr.h265_vbr.dst_frame_rate = 30;
    attr.rc_attr.h265_vbr.max_bit_rate = 1024;
    attr.gop_attr.gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
    attr.gop_attr.normal_p.ip_qp_delta = 3;

    ret = hi_mpi_venc_create_chn(vencChnId, &attr);
    if (ret != 0) {
        std::cout << "Create venc chn failed" << std::hex << ret << "\n";
    }
    
    fd = hi_mpi_venc_get_fd(vencChnId);
    ret = hi_mpi_sys_create_epoll(10, &epollFd);
    if (ret != 0) {
        std::cout << "Call hi_mpi_sys_create_epoll failed " << std::hex << ret << "\n";
    }

    hi_dvpp_epoll_event event;
    event.events = HI_DVPP_EPOLL_IN;
    event.data = (void*)(unsigned long)(fd);
    ret = hi_mpi_sys_ctl_epoll(epollFd, HI_DVPP_EPOLL_CTL_ADD, fd, &event);

    std::cout  << "Finish Init venc at channel " << vencChnId << "\n";
    hi_venc_start_param recv_param{};
    recv_param.recv_pic_num = -1;
    ret = hi_mpi_venc_start_chn(vencChnId, &recv_param);


	visual_thread = std::make_unique<std::thread>(std::thread(std::bind(&AdsfiInterface::VisualizeThread, this)));
}

AdsfiInterface::~AdsfiInterface() {
    _stopped = true;
	if ((visual_thread != nullptr) &&
        (visual_thread->joinable())) {
        visual_thread->join();
    }
    visual_thread = nullptr;
}

static mdc::visual::Times GetCurrentTime()
{
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % 1000000000UL;
    return mdc::visual::Times { sec, nsec };
}


int count = 0;
void AdsfiInterface::Callback(const std::string &name, std::shared_ptr<ara::adsfi::MsgLidarFrame> &cloud){
    // mdc::visual::PointCloud<mdc::visual::PointXYZI> pcPointXYZIData {};
    std::cout<<"receive lidar data "<<cloud->pointCloud.size()<<"\n";

    pcl::PointCloud<PointXYZIRT>::Ptr cloud_input(new pcl::PointCloud<PointXYZIRT>);
    for (uint32_t i = 0; i < cloud->pointCloud.size(); i++) {

        PointXYZIRT pin;
        pin.x = -cloud->pointCloud[i].y;
        pin.y = cloud->pointCloud[i].x;
        pin.z = cloud->pointCloud[i].z;
        pin.intensity = cloud->pointCloud[i].intensity;
        pin.ring = cloud->pointCloud[i].ring;
        cloud_input->points.push_back(pin);
    }
    if(cloud_input->points.size() > 10){
        terrain_segger->InputPointcloud(*cloud_input);
    }
}

void AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ogm){
    std::unique_lock<std::mutex> lock(mtx);
    cloud_ground->points.clear();
    cloud_obstacle->points.clear();
    ogm_map.clear();

    terrain_segger->GetData(ogm_map, *cloud_ground, *cloud_obstacle, range_image, seg_image, ogm_image);

    std::vector<float> param;
    terrain_segger->GetOgmInfo(param);
    ogm->cell_size = param[2];
    ogm->terrain_patch_size = ogm_map.size();
    ogm->map_height = param[0];
    ogm->map_width = param[1];
    ogm->vehicle_posx = param[3];
    ogm->vehicle_posy = param[4];

    for(int i=0; i<ogm_map.size(); ++i){
        ara::adsfi::PerceptionTerrainPatch patch;
        patch.type = ogm_map[i];
        ogm->terrain_patches.push_back(patch);
    }
    std::cout<<"Process GetData \n";
    lock.unlock();
    data_flag = true;
    tcv.notify_one();
}

void AdsfiInterface::VisualizeThread(){

	while(!_stopped){
		std::unique_lock<std::mutex> lock(mtx);
		tcv.wait(lock, [this](){return data_flag.load();});
        
		std::cout<<"[VIZ INFO] publish viz \n";

		//################ pub lidar data ###################

		mdc::visual::PointCloud<mdc::visual::PointXYZI> pcPointXYZIData {};

		pcPointXYZIData.header.stamp = GetCurrentTime();
		pcPointXYZIData.header.frameId = "vehicle";
		for (uint32_t i = 0; i < cloud_obstacle->points.size(); i++) {
			mdc::visual::PointXYZI p(cloud_obstacle->points[i].x, cloud_obstacle->points[i].y, cloud_obstacle->points[i].z,1);
			pcPointXYZIData.points.push_back(p);
		}

		for (uint32_t i = 0; i < cloud_ground->points.size(); i++) {
			mdc::visual::PointXYZI p(cloud_ground->points[i].x, cloud_ground->points[i].y, cloud_ground->points[i].z, 0);
			pcPointXYZIData.points.push_back(p);
		}
		pcPointXYZIData.isDense = false;
		bool flag = cloud_pub.Publish(pcPointXYZIData);
        data_flag = false;
		std::cout<<"[VIZ INFO] publish SGEMENT lidar flag "<<flag<<"\n";

        uint8_t* inputAddr = nullptr;
        auto ret = hi_mpi_dvpp_malloc(0, (void**)&inputAddr, 1280 * 720 * 3 /2);

		cv::Mat backg_mat = cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3);
        cv::Mat resized_mat;
		cv::resize(ogm_image, resized_mat, cv::Size(300, 720)); // 确保尺寸与编码器一致

        cv::Mat roi = backg_mat(cv::Rect((int)((1280 - 300)/2.), 0, resized_mat.cols, resized_mat.rows));
        resized_mat.copyTo(roi);

		int width = backg_mat.cols;  // 1280
		int height = backg_mat.rows; // 720

		// 2. 先转换为YUV420P（I420格式）
		cv::Mat yuv420p_mat;
		cv::cvtColor(backg_mat, yuv420p_mat, cv::COLOR_BGR2YUV_I420);

		// 3. 计算YUV各分量的大小（关键：U/V分量的宽高是原图像的1/2，且向上取整避免截断）
		size_t y_size = width * height;                  // Y分量大小（1280*720 = 921600）
		int uv_width = (width + 1) / 2;                  // U/V分量宽度（1280/2=640）
		int uv_height = (height + 1) / 2;                // U/V分量高度（720/2=360）
		size_t uv_size = uv_width * uv_height;           // 单个U或V分量的大小（640*360=230400）

		// 4. 分配NV12格式的内存（Y + UV交错，总大小：y_size + 2*uv_size）
		cv::Mat nv12_mat(height * 3 / 2, width, CV_8UC1);

		// 5. 复制Y分量（YUV420P的Y分量与NV12的Y分量完全一致）
		memcpy(nv12_mat.data, yuv420p_mat.data, y_size);

		// 6. 提取YUV420P的U和V分量指针
		uint8_t* yuv420p_u = yuv420p_mat.data + y_size;       // U分量起始地址
		uint8_t* yuv420p_v = yuv420p_u + uv_size;             // V分量起始地址

		// 7. 将U/V分量交错复制到NV12的UV区域（关键：按UV分量的实际宽高遍历，避免越界）
		uint8_t* nv12_uv = nv12_mat.data + y_size;             // NV12的UV区域起始地址
		for (int h = 0; h < uv_height; h++) {
			for (int w = 0; w < uv_width; w++) {
				size_t idx = h * uv_width + w;                 // 当前U/V分量的索引
				nv12_uv[2 * idx] = yuv420p_u[idx];             // 复制U分量
				nv12_uv[2 * idx + 1] = yuv420p_v[idx];         // 复制V分量（交错排列）
			}
		}
		size_t nv12_size = y_size + 2 * uv_size;  // 总大小：921600 + 2*230400 = 1,382,400（正确）


		// 发送帧前检查内存分配是否成功
		if (ret != 0 || inputAddr == nullptr) {
			std::cerr << "hi_mpi_dvpp_malloc failed, ret=" << ret << std::endl;
		}else
		{
			std::cout << "hi_mpi_dvpp_malloc success, ret=" << ret << std::endl;
		}
		memcpy(inputAddr,nv12_mat.data,nv12_size);
		std::cout <<"ret-2"<<ret<<std::endl;

		ref_count += 2;
		hi_video_frame_info frame{};
		frame.mod_id = HI_ID_VENC;
		frame.v_frame.width = 1280;
		frame.v_frame.height = 720;
		frame.v_frame.field = HI_VIDEO_FIELD_FRAME;
		frame.v_frame.pixel_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		frame.v_frame.video_format = HI_VIDEO_FORMAT_LINEAR;
		frame.v_frame.compress_mode = HI_COMPRESS_MODE_NONE;
		frame.v_frame.dynamic_range = HI_DYNAMIC_RANGE_SDR8;
		frame.v_frame.color_gamut = HI_COLOR_GAMUT_BT709;
		frame.v_frame.width_stride[0] = 1280;
		frame.v_frame.width_stride[1] = 1280;
		// frame.v_frame.width_stride[2] = 1920;

		frame.v_frame.height_stride[0] = 720;
		frame.v_frame.height_stride[1] = 720;
		// frame.v_frame.height_stride[2] = 1080;

		frame.v_frame.virt_addr[0] = inputAddr;
		frame.v_frame.virt_addr[1] = (hi_void *)((uintptr_t)frame.v_frame.virt_addr[0] + frame.v_frame.width * frame.v_frame.height);
		// frame.v_frame.virt_addr[2] = (hi_void *)((uintptr_t)frame.v_frame.virt_addr[0] + frame.v_frame.width * frame.v_frame.height * 2);

		
		frame.v_frame.frame_flag = 0;
		frame.v_frame.time_ref = ref_count;
		frame.v_frame.pts = 0;
		auto rett = hi_mpi_venc_send_frame(vencChnId, &frame, 10000);
		
		std::cout << "[CUSTOM INFO] Start GetStreamThread channelId"  << "\n";


		std::cout << "[CUSTOM INFO] Start GetStreamThread channelId"  <<fd<< "\n";


		int32_t eventCount = 0;
		hi_dvpp_epoll_event events[1024];
		rett = hi_mpi_sys_wait_epoll(epollFd, events, 3, 10, &eventCount);

		hi_venc_chn_status stat;
		rett = hi_mpi_venc_query_status(vencChnId, &stat);
		hi_venc_stream stream;
		stream.pack_cnt = stat.cur_packs;
		hi_venc_pack pack[stream.pack_cnt];
		stream.pack = pack;
		if (stream.pack == nullptr) {
			std::cout << "Malloc memory failed in GetStreamThread." << "\n";
		}
		rett = hi_mpi_venc_get_stream(vencChnId, &stream, 10);
		if (rett != 0) {
			std::cout  << "Failed to get venc stream." <<std::hex<<" "<<rett<< "\n";
			usleep(10000);
            continue;
		}
		auto streamSize = stream.pack[0].len - stream.pack[0].offset;
		if (stream.pack[0].len <= stream.pack[0].offset) {
			std::cout << "Streamsize is invalid." << "\n";
		}

		std::cout  << "[CUSTOM INFO]  GetStreamThread " << "\n";
		mdc::visual::Image imageEncode{};
		imageEncode.header.frameId = "camera_instance_80";
		imageEncode.frameData.resize(streamSize);
		memcpy(imageEncode.frameData.data(), stream.pack[0].addr + stream.pack[0].offset, streamSize);
		imagePub.Publish(imageEncode);
		hi_mpi_dvpp_free(reinterpret_cast<void*>(stream.pack->input_addr));
		hi_mpi_venc_release_stream(vencChnId, &stream);

    }
}

// void AdsfiInterface::Callback(const std::string &name,std::shared_ptr<ara::adsfi::MsgImageDataList> &msg_ptr){
//     if (__glibc_unlikely(nullptr == msg_ptr)) {
//         // AERROR << "image_list is nullptr";
//         return;
//     }
//     double time_stamp = msg_ptr->header.timestamp.sec+msg_ptr->header.timestamp.nsec/1e9;
//     std::cout << "  IdpSensorImageList:" << msg_ptr->header.seq <<"time_stamp:"<<std::fixed<<time_stamp<< std::endl;
//     // que_image_list_for_put_image_in_shm_.push(*msg_ptr);
//     AINFO << "queue size of que_image_list_for_put_image_in_shm_: " << que_image_list_for_put_image_in_shm_.size();
//     // que_image_list_for_layout_.push(*image_list);
//     for (const auto& img : msg_ptr->images) {
//         AINFO << "XImageLayout input image name: " << img.frameID << ", time:" << std::setprecision(20)<<"\n";
//     }
//     SetImageList(msg_ptr);

//     // AINFO << "queue size of que_image_list_for_layout_: " << que_image_list_for_layout_.size();
// }