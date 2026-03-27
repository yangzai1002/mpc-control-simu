#ifndef YOLO_DETECTION_H
#define YOLO_DETECTION_H

#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <vector>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include <atomic>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "acl/acl.h"
#include "acl/acl_base.h"
#include "acl/dvpp/hi_media_common.h"
#include "acl/dvpp/hi_dvpp.h"
#include "acl/ops/acl_dvpp.h"

#include "camera.h"
#include "publisher.h"
#include "palette.h"
#include "ara/rm/rm.h"

#include <condition_variable>
#include <mutex>
#include <memory>

#include "adsfi_manager/lock_queue.h"
#include <yaml-cpp/yaml.h>

#include "ara/adsfi/impl_type_msgimagedatavec.h"

struct ObjectInfo
{
    float x0;
    float y0;
    float x1;
    float y1;
    float confidence;
    int classId;
    std::string className;
};

class YoloDetection{
public:
    YoloDetection(){
        
    };

    ~YoloDetection(){
        if ((encodeThread != nullptr) &&
            (encodeThread->joinable())) {
            encodeThread->join();
        }
        encodeThread = nullptr;

        if ((detectThread != nullptr) &&
            (detectThread->joinable())) {
            detectThread->join();
        }
        detectThread = nullptr;
    };

    void Init();

    void Deinit();

    int VencWarpperInit();
    void InitDetModelResource();
    void MvizInit();

    void GetStreamThread();

    /**
     * @brief H265 encode
     */
    void ImageEncode(std::vector<ObjectInfo>& objectInfos);

    void DetectionThread();

    bool GetData(std::vector<ObjectInfo>& detections);

    void GenerateColors(std::vector<cv::Scalar>& colors){
        int n = yolov8Label.size();
        for(int i=0; i<n; ++i){
            int idx = i + 3;
            colors.push_back(cv::Scalar(37 * idx % 255, 17 * idx % 255, 29 * idx % 255));
        }
    }

    //void InputCameraData();
    bool Detection(ara::adsfi::MsgImageData image_ptr);
    std::atomic<bool> isend;
private:

    float CalcIou(ObjectInfo boxA, ObjectInfo boxB);
    void FilterByIou(std::vector<ObjectInfo> dets, std::vector<ObjectInfo> &sortBoxes, float iouThresh);
    void NmsSort(std::vector<ObjectInfo> &detBoxes, float iouThresh);

    std::string omModelPath = "yolov7.om";
    size_t modelMemSize_;
    size_t modelWeightSize_;
    void *modelWeightPtr_ = nullptr;
    void *modelMemPtr_ = nullptr;
    uint32_t modelId_;
    void *modelInputBuffer = nullptr;
    aclmdlDataset *input_;
    aclmdlDesc *modelDesc_;
    aclmdlDataset *output_ ;

    hi_vpc_pic_info cvtInputPic;
    hi_vpc_pic_info resizeInputPic;
    hi_vpc_pic_info resizeOutputPic;
    hi_vpc_pic_info paddingOutputPic;
    hi_vpc_pic_info encodeInputPic;

    size_t imgWidth = 1920;
    size_t imgHeight = 1080;
    int count = 0;
    int camId = 0;
    hi_vpc_chn vpcChnId = 100;
    hi_venc_chn vencChnId = 225;
    mdc::visual::Publisher imagePub{};

    std::string camidstr;

    std::condition_variable tcv;
    std::mutex mtx;

    ThreadSafeQueue<std::vector<ObjectInfo> > dets_cache_;
    std::vector<cv::Scalar> colors;

    std::unique_ptr<std::thread> encodeThread {nullptr};
    std::unique_ptr<std::thread> detectThread {nullptr};
    ara::adsfi::MsgImageData image_data_;
    std::mutex image_mutex;
    BuffCfg buffcfg;

    int flag = false;
};

#endif