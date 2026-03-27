#include "yolo_detection.h"

using namespace mdc;
using namespace mdc::camera;

void YoloDetection::Init(){
	isend = false;

    const char *CONFIG_FILE_PATH = std::getenv("MODEL_FILE_PATH");
    printf("CONFIG_FILE_PATH = %s\n", CONFIG_FILE_PATH);

    std::string  usr_config_path = CONFIG_FILE_PATH;
    std::cout << "usr_config_path = " << usr_config_path << std::endl;

    try {
        // 1. 加载 YAML 文件
        YAML::Node config = YAML::LoadFile(usr_config_path);
        std::cout << "loadpapth  " << std::endl;

        if (config.IsNull()) {
            throw std::runtime_error("YAML 文件为空或不存在：" + usr_config_path);
        }

        omModelPath = config["yolo"]["model_path"].as<std::string>();
        camId = config["yolo"]["camid"].as<int>();
        std::cout << "omModelPath  " << omModelPath << std::endl;
    }catch(const std::exception& e) {
        throw std::runtime_error("加载配置失败：" + std::string(e.what()));
    }

    // MDCMbufSysInit();
    //注册RM节点，按照“rm_share_group.yaml”文件的配置，以分配Mbuf多组权限。
    ara::rm::RegisterHisiResource();
    BuffCfg buffcfg = {0};
    //调用该接口初始化buff模块，初始化过程会初始化本进程需要的Buff内存池，初始化完后则可以直接申请buff内存使用。
	auto ret = halBuffInit(&buffcfg);
	if (ret != DRV_ERROR_NONE) {
		std::cout << "[ERROR INFO] YoloDetection Init Fail" << ret << "\n";
	}

    GenerateColors(colors);

    VencWarpperInit();
    MvizInit();

    const std::vector<std::uint32_t> camList = {(uint32_t)camId};
    int res = mdc::camera::CameraInit(camList);
    if (res != 0) {
        std::cout << "[ERROR INFO] camera Init Fail" << res << "\n";
    }else{
        std::cout << "[CUSTOM INFO] camera Init sucess" << res << "\n";
    }

    int fps = 30;
    byte_tracker_ = std::make_unique<BYTETracker>(fps);
    byte_tracker_person_ = std::make_unique<BYTETracker>(fps);
    std::cout << "[CUSTOM INFO] CAM MOT Track init" << std::endl;

    bool result = mdc::visual::Connect();
	if (result) {
	  std::cout  << "[MVIZ INFO] The MViz server address is parsed and the visualization client is created successfully.\n";
	} else {
	  std::cout << "[MVIZ INFO] Failed to parse the MViz server address, or a visualization client has been created.\n";
	}

    node_name = "track_result";
    trackResultPub = mdc::visual::Publisher::Advertise<mdc::visual::ObjectArray>(ara::core::String(node_name));

    detectThread = std::make_unique<std::thread>(std::thread(std::bind(&YoloDetection::DetectionThread, this)));
}    

void YoloDetection::Deinit(){
    isend = true;
    // if ((encodeThread != nullptr) &&
    //     (encodeThread->joinable())) {
    //     encodeThread->join();
    // }
    // encodeThread = nullptr;

    if ((detectThread != nullptr) &&
        (detectThread->joinable())) {
        detectThread->join();
    }
    detectThread = nullptr;

    mdc::camera::CameraDeinit();
}

int  YoloDetection::VencWarpperInit(){
    hi_venc_chn_attr attr{};
    attr.venc_attr.type = HI_PT_H265;
    attr.venc_attr.max_pic_width = 4096;
    attr.venc_attr.max_pic_height = 4096;
    attr.venc_attr.pic_width = 1920;
    attr.venc_attr.pic_height = 1080;
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

    auto ret = hi_mpi_venc_create_chn(vencChnId, &attr);
    if (ret != 0) {
        std::cout << "Create venc chn failed" << std::hex << ret << "\n";
        return ret;
    }
   
    std::cout  << "Finish Init venc at channel " << vencChnId << "\n";
    hi_venc_start_param recv_param{};
    recv_param.recv_pic_num = -1;
    ret = hi_mpi_venc_start_chn(vencChnId, &recv_param);
    std::cout << "hi_mpi_venc_start_chn " << ret << "\n";
    // encodeThread = std::make_unique<std::thread>(std::thread(std::bind(&YoloDetection::GetStreamThread, this)));
    return 0;
}

void YoloDetection::InitDetModelResource(){
    // 初始化
    std::cout<<"[CUSTOM INFO] Init Model resource \n";
    auto aclRet = aclInit(nullptr);
    std::cout << aclRet << "\n";
    aclRet = aclrtSetDevice(0);
    std::cout << aclRet << "\n";
    aclrtContext g_context;
    aclRet = aclrtCreateContext(&g_context, 0);
    std::cout << aclRet << "\n";
    // 初始化模型
    aclError ret = aclmdlQuerySize(omModelPath.c_str(), &modelMemSize_, &modelWeightSize_);
    std::cout << "aclmdlQuerySize ret:" << ret << "\n";
    ret = aclrtMalloc(&modelMemPtr_, modelMemSize_, ACL_MEM_MALLOC_HUGE_FIRST);
    ret = aclrtMalloc(&modelWeightPtr_, modelWeightSize_, ACL_MEM_MALLOC_HUGE_FIRST);

    // 加载模型
    ret = aclmdlLoadFromFileWithMem(
         omModelPath.c_str(), &modelId_, modelMemPtr_, modelMemSize_, modelWeightPtr_, modelWeightSize_);
    modelDesc_ = aclmdlCreateDesc();
    std::cout << "aclmdlLoadFromFileWithMem " << ret << "\n";

    // 创建模型描述
    ret = aclmdlGetDesc(modelDesc_, modelId_);

    // 创建输入dataset
    std::cout << "aclmdlGetNumInputs " << aclmdlGetNumInputs(modelDesc_) << "\n";
    auto modelInputSize = aclmdlGetInputSizeByIndex(modelDesc_, 0);
    std::cout << "modelInputSize " << modelInputSize << "\n";
    aclRet = aclrtMalloc(&modelInputBuffer, modelInputSize, ACL_MEM_MALLOC_NORMAL_ONLY);
    input_ = aclmdlCreateDataset();
    auto inputData = aclCreateDataBuffer(modelInputBuffer, modelInputSize);
    ret = aclmdlAddDatasetBuffer(input_, inputData);
    
    // 创建输入dataset   
    output_ = aclmdlCreateDataset();
    size_t outputSize = aclmdlGetNumOutputs(modelDesc_);
    for (size_t i = 0; i < outputSize; ++i)
    {
        size_t buffer_size = aclmdlGetOutputSizeByIndex(modelDesc_, i);
        void *outputBuffer = nullptr;
        ret = aclrtMalloc(&outputBuffer, buffer_size, ACL_MEM_MALLOC_NORMAL_ONLY);
        std::cout << " outputSize: " << outputSize << " buffer_size: " << buffer_size << "\n";
        aclDataBuffer *outputData = aclCreateDataBuffer(outputBuffer, buffer_size);
        ret = aclmdlAddDatasetBuffer(output_, outputData);
    }

    ret = hi_mpi_sys_init();
    std::cout << "hi_mpi_sys_init "<<ret << "\n";
    
    hi_vpc_chn_attr stChnAttr; 
    ret = hi_mpi_vpc_sys_create_chn(&vpcChnId, &stChnAttr);
    std::cout << "hi_mpi_vpc_sys_create_chn "<<ret << "\n";
  
    cvtInputPic.picture_width = imgWidth;
    cvtInputPic.picture_height = imgHeight;
    cvtInputPic.picture_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    cvtInputPic.picture_width_stride = imgWidth;
    cvtInputPic.picture_height_stride = imgHeight;
    cvtInputPic.picture_buffer_size = cvtInputPic.picture_width_stride * cvtInputPic.picture_height_stride * 3/2;

    resizeInputPic.picture_width = imgWidth;
    resizeInputPic.picture_height  = imgHeight;
    resizeInputPic.picture_format = HI_PIXEL_FORMAT_BGR_888;
    resizeInputPic.picture_width_stride = imgWidth * 3;
    resizeInputPic.picture_height_stride = imgHeight;
    resizeInputPic.picture_buffer_size = resizeInputPic.picture_width_stride * resizeInputPic.picture_height_stride;
    ret = hi_mpi_dvpp_malloc(0, &resizeInputPic.picture_address, resizeInputPic.picture_buffer_size);
    
    resizeOutputPic.picture_width = 640;/*3 bei suo fang 1920/3*/
    resizeOutputPic.picture_height  = 360;/*1200/3*/
    resizeOutputPic.picture_format = HI_PIXEL_FORMAT_BGR_888;
    resizeOutputPic.picture_width_stride = 640 * 3;
    resizeOutputPic.picture_height_stride = 360;
    resizeOutputPic.picture_buffer_size = resizeOutputPic.picture_width_stride * resizeOutputPic.picture_height_stride;
    ret = hi_mpi_dvpp_malloc(0, &resizeOutputPic.picture_address, resizeOutputPic.picture_buffer_size);
    aclRet = aclrtMemset(resizeOutputPic.picture_address, resizeOutputPic.picture_buffer_size, 0, resizeOutputPic.picture_buffer_size);

    paddingOutputPic.picture_width = 640;
    paddingOutputPic.picture_height  = 640;
    paddingOutputPic.picture_format = HI_PIXEL_FORMAT_BGR_888;
    paddingOutputPic.picture_width_stride = 640 * 3;
    paddingOutputPic.picture_height_stride = 640;
    paddingOutputPic.picture_buffer_size = paddingOutputPic.picture_width_stride * paddingOutputPic.picture_height_stride;
    paddingOutputPic.picture_address = modelInputBuffer;

    encodeInputPic.picture_width = imgWidth;
    encodeInputPic.picture_height = imgHeight;
    encodeInputPic.picture_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    encodeInputPic.picture_width_stride = imgWidth;
    encodeInputPic.picture_height_stride = imgHeight;
    encodeInputPic.picture_buffer_size = encodeInputPic.picture_width_stride * encodeInputPic.picture_height_stride * 3/2;
    ret = hi_mpi_dvpp_malloc(0, &encodeInputPic.picture_address, encodeInputPic.picture_buffer_size);
    aclRet = aclrtMemset(encodeInputPic.picture_address, encodeInputPic.picture_buffer_size, 0, encodeInputPic.picture_buffer_size);
    std::cout << "Init mode resource finished" << "\n";
}

void YoloDetection::MvizInit(){
    std::cout << "[CUSTOM INFO] Init Mviz Start" << "\n";
    mdc::visual::Connect();
    imagePub = mdc::visual::Publisher::Advertise<mdc::visual::Image>("image_for_perception_" + std::to_string(camId));
    std::cout << "[CUSTOM INFO] Init Mviz finished" << "\n";
}

float YoloDetection::CalcIou(ObjectInfo boxA, ObjectInfo boxB)
{

    float boxAWidth = boxA.x1 - boxA.x0;
    float boxBWidth = boxB.x1 - boxB.x0;
    float boxAHeight = boxA.y1 - boxA.y0;
    float boxBHeight = boxB.y1 - boxB.y0;
    float left = std::max(boxA.x0, boxB.x0);
    float right = std::min(boxA.x1, boxB.x1);
    float top = std::max(boxA.y0, boxB.y0);
    float bottom = std::min(boxA.y1, boxB.y1);
    if (top > bottom || left > right)
    {
        return 0.0f;
    }
    float area = (right - left) * (bottom - top);
    return area / (boxAWidth * boxAHeight + boxBWidth * boxBHeight - area);
}

void YoloDetection::FilterByIou(std::vector<ObjectInfo> dets, 
                                std::vector<ObjectInfo> &sortBoxes, float iouThresh)
{
    for (size_t m = 0; m < dets.size(); ++m)
    {
        auto &item = dets[m];
        sortBoxes.push_back(item);
        for (size_t n = m + 1; n < dets.size(); ++n)
        {
            if (CalcIou(item, dets[n]) > iouThresh)
            {
                dets.erase(dets.begin() + n);
                --n;
            }
        }
    }
}

void YoloDetection::NmsSort(std::vector<ObjectInfo> &detBoxes, float iouThresh)
{
    if (detBoxes.size() == 0)
    {
        return;
    }
    std::vector<ObjectInfo> sortBoxes;
    std::map<int, std::vector<ObjectInfo>> resClassMap;
    for (const auto &item : detBoxes)
    {
        resClassMap[static_cast<int>(item.classId)].push_back(item);
    }
    std::map<int, std::vector<ObjectInfo>>::iterator iter;
    for (iter = resClassMap.begin(); iter != resClassMap.end(); ++iter)
    {
        std::sort(iter->second.begin(), iter->second.end(), [=](const ObjectInfo &a, const ObjectInfo &b)
                  { return a.confidence > b.confidence; });

        FilterByIou(iter->second, sortBoxes, iouThresh);
    }
    detBoxes = std::move(sortBoxes);
}

void YoloDetection::GetStreamThread(){
    std::cout << "[CUSTOM INFO] Start GetStreamThread channelId" << isend << "\n";
    aclrtContext g_context;
    aclrtSetDevice(0);
    aclrtCreateContext(&g_context, 0);
    int32_t epollFd = 0;
    int32_t fd = hi_mpi_venc_get_fd(vencChnId);
    std::cout << "isend" << isend << "\n";
    auto ret = hi_mpi_sys_create_epoll(10, &epollFd);
    if (ret != 0) {
        std::cout << "Call hi_mpi_sys_create_epoll failed " << std::hex << ret << "\n";
        return;
    }
    hi_dvpp_epoll_event event;
    event.events = HI_DVPP_EPOLL_IN;
    event.data = (void*)(unsigned long)(fd);
    ret = hi_mpi_sys_ctl_epoll(epollFd, HI_DVPP_EPOLL_CTL_ADD, fd, &event);
    if (ret != 0) {
        std::cout << "Call hi_mpi_sys_ctl_epoll failed " << std::hex << ret << "\n";
        return;
    }
    std::cout  << "Start GetStreamThread loog at" << "\n";
    while (!isend) {
        int32_t eventCount = 0;
        hi_dvpp_epoll_event events[1024];
        ret = hi_mpi_sys_wait_epoll(epollFd, events, 3, 1000, &eventCount);

        hi_venc_chn_status stat;
        ret = hi_mpi_venc_query_status(vencChnId, &stat);
        hi_venc_stream stream;
        stream.pack_cnt = stat.cur_packs;
        hi_venc_pack pack[stream.pack_cnt];
        stream.pack = pack;
        if (stream.pack == nullptr) {
            std::cout << "Malloc memory failed in GetStreamThread." << "\n";
            break;
        }
        ret = hi_mpi_venc_get_stream(vencChnId, &stream, 1000);
        if (ret != 0) {
            std::cout  << "Failed to get venc stream." <<std::hex<<" "<<ret<< "\n";
            usleep(10000);
            continue;
        }
        auto streamSize = stream.pack[0].len - stream.pack[0].offset;
        if (stream.pack[0].len <= stream.pack[0].offset) {
            std::cout << "Streamsize is invalid." << "\n";
            break;
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
    hi_mpi_sys_ctl_epoll(epollFd, HI_DVPP_EPOLL_CTL_DEL, fd, NULL);
    hi_mpi_sys_close_epoll(epollFd);
}

void YoloDetection::ImageEncode(std::vector<ObjectInfo>& objectInfos){
    cv::Mat image = cv::Mat(1080, 1920, CV_8UC3, static_cast<uint8_t*>(resizeInputPic.picture_address));
    for (auto& obj : objectInfos) {
         cv::rectangle(image, cv::Point(obj.x0, obj.y0), cv::Point(obj.x1, obj.y1), colors[obj.classId], 2); 
    }

    uint32_t taskID = 0;
    auto ret = hi_mpi_vpc_convert_color(vpcChnId, &resizeInputPic, &encodeInputPic, &taskID, -1);
    ret = hi_mpi_vpc_get_process_result(vpcChnId, taskID, -1);
    void* encodeData = nullptr;
    ret = hi_mpi_dvpp_malloc(0, &encodeData, 1920 * 1080 * 3 / 2);
    memcpy(encodeData, encodeInputPic.picture_address, 1920 * 1080 * 3 / 2);

    count += 2;
    hi_video_frame_info frame{};
    frame.mod_id = HI_ID_VENC;
    frame.v_frame.width = 1920;
    frame.v_frame.height = 1080;
    frame.v_frame.field = HI_VIDEO_FIELD_FRAME;
    frame.v_frame.pixel_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    frame.v_frame.video_format = HI_VIDEO_FORMAT_LINEAR;
    frame.v_frame.compress_mode = HI_COMPRESS_MODE_NONE;
    frame.v_frame.dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    frame.v_frame.color_gamut = HI_COLOR_GAMUT_BT709;
    frame.v_frame.width_stride[0] = 1920;
    frame.v_frame.width_stride[1] = 1920;
    frame.v_frame.height_stride[0] = 1080;
    frame.v_frame.height_stride[1] = 1080;
    frame.v_frame.virt_addr[0] = encodeData;
    frame.v_frame.virt_addr[1] = (hi_void *)((uintptr_t)frame.v_frame.virt_addr[0] + frame.v_frame.width * frame.v_frame.height);
    frame.v_frame.frame_flag = 0;
    frame.v_frame.time_ref = count;
    frame.v_frame.pts = 0;
    ret = hi_mpi_venc_send_frame(vencChnId, &frame, 10000);
    if (ret != 0) {
        std::cout << "Decode by hi_mpi_venc_send_frame failed" << std::hex << ret;
    }
}

void YoloDetection::DetectionThread(){
    std::cout << "[CUSTOM INFO] Start DetectionThread" << isend << "\n";
    InitDetModelResource();

    while (!isend) {
        std::vector<uint32_t> camIds = CameraWaitEvents(1000);
        std::cout <<"[CUSTOM INFO]  camIds:"<<camIds.size()<<std::endl;
        for (auto &cam : camIds) {
          std::vector<CameraData> camDatas;
          auto ret = CameraGetData(cam, camDatas, 1000);
          for (auto &camData : camDatas) {

            gettimeofday(&camera_timestamp, NULL);

            auto start = std::chrono::steady_clock::now();
            MDCMbuf mbuf;
            mbuf.SetMbuf(camData.yuvData);
            void* data = nullptr;
            uint64_t size;
            mbuf.GetBuffData(data, size);
            mbuf.GetDataLen(size);
            mbuf.Free();

            cvtInputPic.picture_address = data;
            uint32_t taskID = 0;
            ret = hi_mpi_vpc_convert_color(vpcChnId, &cvtInputPic, &resizeInputPic, &taskID, -1);

            ret = hi_mpi_vpc_get_process_result(vpcChnId, taskID, -1);

            ret = hi_mpi_vpc_resize(vpcChnId, &resizeInputPic, &resizeOutputPic, 1.0, 1.0, 0, &taskID, -1);

            ret = hi_mpi_vpc_get_process_result(vpcChnId, taskID, -1);

            hi_vpc_scalar scalar_value;
            scalar_value.val[0] = 127.0;
            scalar_value.val[1] = 127.0;
            scalar_value.val[2] = 127.0;
            scalar_value.val[3] = 127.0;
            hi_vpc_make_border_info borderInfo = {140, 140, 0, 0, HI_BORDER_CONSTANT, scalar_value};

            ret = hi_mpi_vpc_copy_make_border(vpcChnId, &resizeOutputPic, &paddingOutputPic, borderInfo, &taskID, -1);
            //std::cout <<"ret-7:"<<ret<<std::endl;
            //printf("ret-7:%04X\n",ret);
            ret = hi_mpi_vpc_get_process_result(vpcChnId, taskID, -1);
            //std::cout <<"ret-8:"<<ret<<std::endl;
            //printf("ret-8:%04X\n",ret);
            ret = aclmdlExecute(modelId_, input_, output_);
            //std::cout <<"ret-9:"<<ret<<std::endl;
            aclDataBuffer *dataBuffer = aclmdlGetDatasetBuffer(output_, 0);
            float *output = reinterpret_cast<float *>(aclGetDataBufferAddr(dataBuffer)); // 84 * 8400
            std::vector<ObjectInfo> objectInfos;

            for (int i = 0; i < 25200; i++)
            {
                size_t index = i * 85;
                if (output[index + 4] <= 0.3) {
                    continue;
                }
                int classId = -1;
                float maxProb = 0.3;
                for (int j = 5; j < 85; j++)
                {
                if (output[index + j] * output[index + 4] > maxProb)
                {
                    maxProb = output[index + j] * output[index + 4];
                    classId = j - 5;
                }
                }
                if (classId < 0 || classId > 10)
                {
                continue;
                }
                size_t offsetTop = 140;
                size_t offsetLeft = 0;
                double division = 1.0 / 3;
                auto leftX = (output[index] - output[index + 2] / 2 - offsetLeft) / division;
                auto leftY = (output[index + 1] - output[index + 3] / 2 - offsetTop) / division;
                auto rightX = (output[index] + output[index + 2] / 2 - offsetLeft) / division;
                auto rightY = (output[index + 1] + output[index + 3] / 2 - offsetTop) / division;

                ObjectInfo obj;
                obj.x0 = leftX < 0.0 ? 0.0 : leftX;
                obj.y0 = leftY < 0.0 ? 0.0 : leftY;
                obj.x1 = rightX > 1920 ? 1920 : rightX;
                obj.y1 = rightY > 1080 ? 1080 : rightY;
                obj.confidence = maxProb;
                obj.classId = classId;
                obj.className = yolov8Label[classId];
                //std::cout << "[CUSTOM INFO ] ############### detection size "<<obj.className<<" "<<obj.confidence<<"\n";

                objectInfos.push_back(obj);
            }

            NmsSort(objectInfos, 0.5);

            std::cout << "[CUSTOM INFO ] ############### detection size "<<objectInfos.size()<<"\n";
            for (auto &objInfo : objectInfos)
            {
                auto number = 0.5;
                std::cout << "Find object: className(" << objInfo.className<<" "<<objInfo.classId  << "), confidence(" << objInfo.confidence 
                            << "), scoreThresh(" << number << "), Coordinates (x0, y0)=(" << objInfo.x0 << ", "
                            << objInfo.y0 << "), (x1, y1)=(" << objInfo.x1 << "," << objInfo.y1 << ")." << std::endl;
            }
            

            if(objectInfos.size()> 0){
                std::vector<Object> boxes;
                std::vector<Object> person_boxes;

                for(auto &objInfo : objectInfos){

                    std::cout<<"cam_det_ptr->object[i].cls "<<(int)objInfo.classId<<" "<<"clsDescription "<<ara::core::String(objInfo.className)<<
                            "cam_det_ptr->object[i].clsConfidence "<<objInfo.confidence<<"\n";


                    if(	objInfo.classId == 2 
                        || objInfo.classId == 5 
                        || objInfo.classId == 6 
                        || objInfo.classId == 7){

                        Object obj;
                        obj.rect.x = (objInfo.x1 - objInfo.x0)/2 + objInfo.x0;
                        obj.rect.y = (objInfo.y1 - objInfo.y0)/2 + objInfo.y0;
                        obj.rect.width = (objInfo.x1 - objInfo.x0);
                        obj.rect.height = (objInfo.y1 - objInfo.y0);
                        obj.label = objInfo.classId;
                        obj.prob = objInfo.confidence;
                        boxes.push_back(obj);
                    }

                    if( objInfo.classId == 0){
                        Object obj;
                        obj.rect.x = (objInfo.x1 - objInfo.x0)/2 + objInfo.x0;
                        obj.rect.y = (objInfo.y1 - objInfo.y0)/2 + objInfo.y0;
                        obj.rect.width = (objInfo.x1 - objInfo.x0);
                        obj.rect.height = (objInfo.y1 - objInfo.y0);
                        obj.label = objInfo.classId;
                        obj.prob = objInfo.confidence;
                        person_boxes.push_back(obj);
                    }
                }


                std::cout<<"[CUSTOM INFO] objects size"<<boxes.size()<<"\n";
                std::cout<<"[CUSTOM INFO] person objects size"<<person_boxes.size()<<"\n";

                std::vector<STrack> output_stracks = byte_tracker_->update(boxes);
                std::vector<STrack> output_stracks_person = byte_tracker_person_->update(person_boxes);

                std::cout<<"[CUSTOM INFO] output_stracks "<<output_stracks.size()<<"\n";
                std::cout<<"[CUSTOM INFO] output_stracks "<<output_stracks_person.size()<<"\n";


                mdc::visual::ObjectArray track_data;
                track_data.header.frameId = "camera_instance_80";

                timeval tm;
                gettimeofday(&tm, NULL);
                track_data.header.stamp.sec = (uint32_t)tm.tv_sec;
                track_data.header.stamp.nsec = (uint32_t)tm.tv_usec*1e3;

                // ara::adsfi::Object2dArray cam_tracks;
                // cam_tracks.header = cam_det_ptr->header;
                std::vector<ObjectInfo> trackInfos;

                for(auto track:output_stracks){
                    mdc::visual::Object obj;
                    obj.trackId = track.track_id;
                    obj.classification = mdc::visual::CLASSIFICATION_CAR;
                    obj.classificationConfidence = track.score;
                    std::vector<float> tlwh = track.tlwh;
                    obj.boxImage = mdc::visual::Rectangle(tlwh[0] - tlwh[2]/2 ,tlwh[1] - tlwh[3]/2 ,tlwh[2],tlwh[3]);
                    track_data.objectList.push_back(obj);

                    ObjectInfo track_det;
                    track_det.track_id = track.track_id;
                    track_det.centerx = tlwh[0];
                    track_det.centery = tlwh[1];
                    track_det.width = tlwh[2];
                    track_det.height =tlwh[3];
                    track_det.classId =  track.type;
                    track_det.confidence = track.score;
                    trackInfos.push_back(track_det);
                    std::cout<<"Track result trackId "<<obj.trackId<<" obj.classification  "<<obj.classification <<" obj.classificationConfidence  "<<obj.classificationConfidence <<"\n";
                }



                for(auto track:output_stracks_person){
                    mdc::visual::Object obj;
                    obj.trackId = track.track_id;
                    obj.classification = mdc::visual::CLASSIFICATION_PEDESTRIAN;
                    obj.classificationConfidence = track.score;
                    std::vector<float> tlwh = track.tlwh;
                    obj.boxImage = mdc::visual::Rectangle(tlwh[0] - tlwh[2]/2 ,tlwh[1] - tlwh[3]/2 ,tlwh[2],tlwh[3]);
                    track_data.objectList.push_back(obj);

                    ObjectInfo track_det;
                    track_det.track_id = track.track_id;
                    track_det.centerx = tlwh[0];
                    track_det.centery = tlwh[1];
                    track_det.width = tlwh[2];
                    track_det.height =tlwh[3];
                    track_det.classId =  track.type;
                    track_det.confidence = track.score;
                    trackInfos.push_back(track_det);
                    std::cout<<"Track result trackId "<<obj.trackId<<" obj.classification  "<<obj.classification <<" obj.classificationConfidence  "<<obj.classificationConfidence <<"\n";
                }

                std::unique_lock<std::mutex> lock(mtx);
                dets_cache_.push(trackInfos);
                tcv.notify_one();
                lock.unlock();
                // cam_tracks_cache.push(cam_tracks);
                // tcv.notify_one();
                trackResultPub.Publish(track_data);
            }

            // ImageEncode(objectInfos);
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Time cost " << duration / 1000.0 << std::endl;
            camData.yuvData.Free();
        }
      }
    }
}

bool YoloDetection::GetData(std::vector<ObjectInfo>& detections, timeval& time){
    std::unique_lock<std::mutex> lock(mtx);
    tcv.wait(lock, [this](){return !dets_cache_.empty();});
    std::cout<<"[CUSTOM INFO ] ############### get data size \n";
    if(dets_cache_.tryPop(detections)){
        time.tv_sec = camera_timestamp.tv_sec;
        time.tv_usec = camera_timestamp.tv_usec;
        return true;
    }else{
        return false;
    }
    return false;
}
