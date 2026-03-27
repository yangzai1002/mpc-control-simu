#ifndef XIMAGEPATHPLAN_H_
#define XIMAGEPATHPLAN_H_
#include <math.h>
#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <string>
#include <vector>

#include "VideoEditYUV420UV12.hpp"
#include "custom_stack.h"
#include "log/avos_node.h"
// #include "faulthandle_api.hpp"

#define MODEL_PINHOLE 0
#define MODEL_FISHEYE 1

typedef struct DistortionParams {
    int   camera_model = 0;
    float params_d[5] = {0, 0, 0, 0, 0};
    float epsilon = 0;
} DataType;

struct CameraIntrinsics {
    std::string                camera_type_;
    float                      camera_width_ = 0.0;
    float                      camera_height_ = 0.0;
    Eigen::Matrix<float, 3, 3> intr_K_;

    DistortionParams           intr_D_;
    float                      epsilon;
    Eigen::Matrix<float, 3, 3> extr_R_init_;
    Eigen::Matrix<float, 3, 3> extr_R_;
    Eigen::Matrix<float, 3, 1> extr_t_;
};

class XImagePathPlan {
private:
    XImagePathPlan() {
        cameras_intrinsics_.clear();
        AINFO << "XImagePathPlan init" << std::endl;
        // std::string usr_config_path = CONFIG_PATH;
        // AINFO << "usr_config_path = " << usr_config_path << std::endl;

        auto ptr = CustomStack::Instance();

        std::string front_ktdx_, back_ktdx_;
        ptr->GetConfig("XVideoEditor","front_ktdx_", front_ktdx_);
        ptr->GetConfig("XVideoEditor","back_ktdx_", back_ktdx_);
        std::string camera_intrinsics_path1 = "/opt/usr/zxz/config/cameras_distortion.proj_cfg";

        setCameraDistortion(front_ktdx_);
        setCameraDistortion(back_ktdx_);
    };

public:
    std::uint32_t ec_add(const std::string& ec) {
        std::lock_guard<std::mutex> lock(_mtx);
        auto                        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            _ec_map[ec] = 0;
        }
        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            return 0;
        }

        ++ptr->second;
        if (ptr->second % 20 == 1) {
            // FaultHandle::FaultApi::Instance()->SetFaultCode(ec);
        }
        return ptr->second;
    }

    void ec_remove(const std::string& ec) {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_ec_map.find(ec) != _ec_map.end()) {
            // FaultHandle::FaultApi::Instance()->ResetFaultCode(ec);
            _ec_map.erase(ec);
        }
    }

    void ec_clear() {
        std::lock_guard<std::mutex> lock(_mtx);
        for (const auto& ec : _ec_map) {
            // FaultHandle::FaultApi::Instance()->ResetFaultCode(ec.first);
        }
        _ec_map.clear();
    }

public:
    static XImagePathPlan* Instance() {
        static XImagePathPlan* in = nullptr;
        if (in == nullptr) {
            in = new XImagePathPlan();
        }
        return in;
    }

    ~XImagePathPlan(){};

public:
    //获取辅助线
    void setLeftPathPoint(cv::Point3f point);
    void setRightPathPoint(cv::Point3f point);

    void setWorningPathPointLeft(cv::Point3f point);
    void setWornintPathPointRight(cv::Point3f point);

    void setRunPathPointLeft(cv::Point3f point);
    void setRunPathPointRight(cv::Point3f point);

    void setPathPlanPointL(cv::Point3f point);
    void setPathPlanPointR(cv::Point3f point);

    void setPathPlanPoints(cv::Point3f point);

    void clearPathPoint();

    void getLeftPathPoint(const std::string& type, std::vector<cv::Point>& points);
    void getRightPathPoint(const std::string& type, std::vector<cv::Point>& points);

    void getWorningPathPointLeft(const std::string& type, std::vector<cv::Point>& points);
    void getWornintPathPointRight(const std::string& type, std::vector<cv::Point>& points);

    void getRunPathPointLeft(const std::string& type, std::vector<cv::Point>& point);
    void getRunPathPointRight(const std::string& type, std::vector<cv::Point>& point);

    void getPathPlanPointL(const std::string& type, std::vector<cv::Point>& points);
    void getPathPlanPointR(const std::string& type, std::vector<cv::Point>& points);

    // 辅助线绘制
    // void drawRemotePathPlan(std::shared_ptr<YUYV420UV12> ptr,const std::string& type);
    // void drawAutoPathPlan(std::shared_ptr<YUYV420UV12> ptr,const std::string& type);

    void drawRemotePathPlan(void* ptr, const std::string& type);
    void drawAutoPathPlan(void* ptr, const std::string& type);

protected:
    void getPointByPoint3f(const std::string& camera_type, std::vector<cv::Point>& points,
                           std::vector<cv::Point3f> point_vec, std::string type = "");

    std::vector<cv::Point> createPolygon(std::vector<cv::Point>& curve1, std::vector<cv::Point>& curve2);

public:
    bool setCameraDistortion(const std::string& type);

    bool ConvertPointVCS2IMG(const std::string& type, const cv::Point3f& p3fVcs, cv::Point3f& p3fImg);

private:
    bool FileExists(const std::string file_path);
    bool LoadCameraIntrinsics(const std::string& yaml_file, std::string type = "");
    bool LoadCameraExtrinsics(const std::string& yaml_file);
    void PixelDenormalize(const std::string& type, cv::Point3f& pt);
    bool UndistortPointFisheye(const cv::Point2f& pt, cv::Point2f& upoint);
    void ProjectInImageFisheye(const std::string& type, cv::Point3f& pt);

    /***********************************************前相机参数*******************************/
    float                      camera_width_ = 0.0;
    float                      camera_height_ = 0.0;
    Eigen::Matrix<float, 3, 3> intr_K_;

    DistortionParams           intr_D_;
    float                      epsilon;
    Eigen::Matrix<float, 3, 3> extr_R_init_;
    Eigen::Matrix<float, 3, 3> extr_R_;
    Eigen::Matrix<float, 3, 1> extr_t_;

    /*****************************************应用相机内参和外参*****************************************************/

private:
    std::vector<cv::Point3f> pathPoints_l;
    std::vector<cv::Point3f> pathPoints_r;

    std::vector<cv::Point3f> warningPathPoint_l;
    std::vector<cv::Point3f> warningPathPoint_r;

    std::vector<cv::Point3f> runPathPoint_l;
    std::vector<cv::Point3f> runPathPoint_r;

    std::vector<cv::Point3f> pathplanPoint_l;
    std::vector<cv::Point3f> pathplanPoint_r;

    std::vector<cv::Point3f> pathplan_Points;

    cv::Point3f target_Point{0, 0, 0};
    cv::Point3f head_Point{0, 0, 0};

    std::string camera_type = "front_h100";

    int bottom_length = 0;

    std::mutex                           _mtx;
    std::map<std::string, std::uint32_t> _ec_map;

    std::vector<std::shared_ptr<CameraIntrinsics>> cameras_intrinsics_;
    //错误信息
public:
    static const constexpr char* _ERRORCODE_PARAMETER_ERROR = "4002001";  // 配置参数读取失败
    static const constexpr char* _ERRORCODE_DATA_ERROR = "4002002";       //相机数据无输入
    static const constexpr char* _ERRORCODE_DATA_INVALID = "4002003";     // 接收数据无效
    static const constexpr char* _ERRORCODE_ENCODING_ERROR = "4002004";   // 相机的编码格式错误
    static const constexpr char* _ERRORCODE_FREQUENCY_ERROR = "4002005";  // 数据接收频率异常
    static const constexpr char* _ERRORCODE_POS_JUMP = "4002006";         // 共享内存申请失败
    static const constexpr char* _ERRORCODE_DTAALIST_EMPTY = "4002007";   // 相机显示列表中无数据
    static const constexpr char* _ERRORCODE_DATARESULT_ERROR = "4002008"; // 合并数据结果为空
    static const constexpr char* _ERRORCODE_SPLIT_INVALID = "4002009";    //拼接参数无效
};

#endif // XIMAGEPATHPLAN_H_
